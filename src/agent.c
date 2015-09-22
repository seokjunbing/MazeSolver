/*  agent.c  controls a single one of the agents in the maze

    Project name: AMazing Challenge
    Component name: agent

    Primary Author: Matthew West
    Date Created: 2015-02-25

    Special considerations:
        None

    ======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h> /* NULL */
#include <stdio.h> /* fprintf perror */
#include <string.h> /* strlen */
#include <errno.h> /* global error value for system calls */
#include <unistd.h> /* getcwd */
#include <sys/types.h>
#include <sys/stat.h> /* mkfifo */
#include <sys/shm.h> /* shmat shmdt shmget */
#include <fcntl.h> /* mknod open write (FIFO)*/
#define _GNU_SOURCE

// ---------------- Local includes  e.g., "file.h"
#include "worldmap.h"
#include "network.h"
#include "amazing.h"
#include "agent.h"
#include "mazealgorithm.h"
#include "log.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* dirToMoveDir
 *      Converts our enum type Direction to its corresponding uint32_t
 *
 * @d: the direction to be converted
 */
uint32_t dirToMoveDir(Direction d);


/*====================================================================*/
int agent(int curAvatar, int nAvatars, int xlen, int ylen, char *hostname, key_t k,
                uint32_t port, int algo)
{
        int shmid;
        int s;
        char *outstring;
        WorldMap *globalmap;
        AM_Message outMessage;
        AM_Message inMessage;
        XYPos pos[AM_MAX_AVATAR];
        Direction movdir;
        int iterations = 0;
        int i;
        bool firstMove = true;
        int fd_log;
        LogMessage lmsg;
        float pknown;

        /* makes named pipe to logger */
        if (mkfifo(FIFO_NAME, 0666) == -1) {
                if (errno != EEXIST) {
                        perror("mkfifo");
                        return -1;
                }
        }

        fd_log = open(FIFO_NAME, O_WRONLY);
        if (fd_log == -1) {
                fprintf(stderr, "Failed to connect to log process\n");
                perror("open");
                return -1;
        }

        LOG(lmsg, fd_log, LVL_VERB, "%d connected to server", curAvatar);

        /* start with nonsense values to pos so we never assume we have a wall in front
         * of us on the first turn
         */
        for (i = 0; i < nAvatars; i++) {
                pos[i].x = AM_MAX_GRID_SIZE + 1;
                pos[i].y = AM_MAX_GRID_SIZE + 1;
        }

        /* start facing North */
        movdir = N;

        /* get shared memory */
        if ((shmid = shmget(k, sizeof(WorldMap), 0644 | IPC_CREAT)) == -1) {
                perror("shmget");
                LOG(lmsg, fd_log, LVL_FATAL, "Could not get id for shared memory: %s",\
                        strerror(errno));
        }

        /* attach to the segment and get a pointer to it */
        globalmap = shmat(shmid, (void *)0, 0);
        if (globalmap == (WorldMap *)(-1)) {
                perror("shmat");
                shmctl(shmid, IPC_RMID, NULL);
                fprintf(stderr, "Could not attach to shared memory\n");
                return (-1);
        }

        /* Avatar 0 initialize global map */
        if (curAvatar == 0) {
                LOG(lmsg, fd_log, LVL_VERB, "initializing global worldmap%s", "");
                globalmap->xlen = xlen;
                globalmap->ylen = ylen;
                initWorldMap(globalmap);
        } else {
                sleep(1); /* allow time for the log file to be initialized */
        }

        /* connect to the server and send avatar ready message */
        s = connectToServer(hostname, port);
        if (s < 0) {
                LOG(lmsg, fd_log, LVL_INFO, "Avatar %d Could not connect to server",
                                curAvatar+1);
                shmctl(shmid, IPC_RMID, NULL);
                return -1;
        }

        memset(&outMessage, 0, sizeof(outMessage));
        outMessage.type = AM_AVATAR_READY;
        outMessage.avatar_ready.AvatarId = curAvatar;
        sendAMMessage(s, outMessage);

        /* main agent loop */
        do {
                /* Get the next message from the server */
                inMessage = receiveAMMessage(s);
                if (IS_AM_ERROR(inMessage.type)) {
                        if (inMessage.type & AM_AVATAR_OUT_OF_TURN)
                                continue;
                        printAMError(fd_log, inMessage.type);
                        shmctl(shmid, IPC_RMID, NULL);
                        return -1;
                }
                /* if the message is not the right type */
                if (!(inMessage.type & AM_AVATAR_TURN))
                        continue;

                /* if it is our turn */
                if (inMessage.avatar_turn.TurnId == curAvatar) {
                        pthread_mutex_lock(&globalmap->mapMutex);
                        /* Get our next move */
                        movdir = getNextMove(*globalmap, inMessage.avatar_turn.Pos,
                                        movdir, pos[curAvatar], nAvatars, curAvatar,
                                        algo);

                        /* initialize outMessage */
                        memset(&outMessage, 0, sizeof(outMessage));
                        outMessage.type = AM_AVATAR_MOVE;
                        outMessage.avatar_move.AvatarId = curAvatar;
                        outMessage.avatar_move.Direction = dirToMoveDir(movdir);
                        sendAMMessage(s, outMessage);

                        iterations++;
                        /* Avatar 0 counts iterations */
                        if (curAvatar == 0) {
				fprintf(stdout, "\033[2J\033[1;1H");
                                printf("iteration %d\n", iterations);
                                outstring = serializeWorldMap(*globalmap, nAvatars,
                                                inMessage.avatar_turn.Pos);
                                /* Print to stdout */
                                fprintf(stdout, "%s\n", outstring);
                                free(outstring);
                                if (algo == 2) {
                                        if (!(connectedToAll(*globalmap,
                                                                        pos[0], nAvatars))) {
                                                pknown = percentKnown(*globalmap, nAvatars);
                                        }
                                        fprintf(stdout, "percent known %.1f %%\n",
                                                        pknown*100);
                                }

                                for (i = 0; i < nAvatars; i++) {
                                        LOG(lmsg, fd_log, LVL_INFO,
                                                        "avatar %d (% 3d,% 3d) -> "
                                                        "(% 3d,% 3d)", i+1, (int)pos[i].x,
                                                        (int)pos[i].y,
                                                        (int)inMessage.avatar_turn.Pos[i].x,
                                                        (int)inMessage.avatar_turn.Pos[i].y);
                                }
                        }
                        /* Update our position */
                        for (i = 0; i < nAvatars; i++)
                                pos[i] = inMessage.avatar_turn.Pos[i];
                } else if (inMessage.avatar_turn.TurnId ==
                                (curAvatar + 1)%nAvatars && !firstMove) {
                        /* Update with results of previous move */
                        if (inMessage.type & AM_AVATAR_TURN)
                                updateWorldMap(globalmap, pos[curAvatar], movdir, inMessage,
                                                curAvatar, nAvatars, algo);
                        pthread_mutex_unlock(&globalmap->mapMutex);
                }
                firstMove = false;
        } while (!(inMessage.type & AM_MAZE_SOLVED) &&
                        !(inMessage.type == AM_TOO_MANY_MOVES));


        /* Detach and (if no other process is connected) deallocate shared memory */
        shmctl(shmid, IPC_RMID, NULL);
        close(s);

        if (curAvatar == 0) {
                if (inMessage.type & AM_MAZE_SOLVED) {
                        printf("Moves to solve: %" PRIu32, inMessage.maze_solved.nMoves);
                        printf("\n");
                        printf("Size = %dx%d\n", xlen, ylen);
                        printf("Algorithm = %d\n", algo);
                        printf("# of Avatars = %d\n", nAvatars);

                        /* print to log */
                        LOG(lmsg, fd_log, LVL_INFO, "Moves to Solve: %" PRIu32,
                                        inMessage.maze_solved.nMoves);
                        LOG(lmsg, fd_log, LVL_INFO, "Hash: %" PRIu32,
                                        inMessage.maze_solved.Hash);

                } else {
                        LOG(lmsg, fd_log, LVL_INFO, "Maze not solved after ~%d moves",
                                        iterations * nAvatars);
                        memset(&lmsg, 0, sizeof(lmsg));
                }
        }
        sleep(1);
        close(fd_log);
        unlink(FIFO_NAME);
        pthread_mutex_destroy(&globalmap->mapMutex);
        return 0;
}

/* dirToMoveDir
 *   Takes a Direction struct as an argument and returns the macro defined equivalent.
 *   @d- direction to get macro equivalent
 *
 **/
uint32_t dirToMoveDir(Direction d)
{
        switch (d) {
                case N:
                        return M_NORTH;
                case S:
                        return M_SOUTH;
                case E:
                        return M_EAST;
                case W:
                        return M_WEST;
                default:
                        return M_NULL_MOVE;
        }
}

