/*  startup.c  starts up the maze and connects to the server

    Copyright 2015 (if any)

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    Special considerations:
    (e.g., special compilation options, platform limitations, etc.)

    ======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h> /* printf, scanf */
#include <stdlib.h> /* malloc, calloc, realloc, free */
#define _GNU_SOURCE
#include <string.h> /* strlen, strcpy */
#include <unistd.h> /* fork */
#include <sys/wait.h> /* wait */
#include <sys/stat.h> /* mkfifo */
#include <libgen.h> /* basename */
#include <sys/ipc.h> /* ftok */
#include <signal.h> /* signal */
#include <sys/shm.h> /* shmat shmdt shmget */
#include <errno.h> /* global error value for system calls */
#include <fcntl.h>


// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "amazing.h"
#include "network.h"
#include "worldmap.h"
#include "agent.h"
#include "log.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
key_t k;
// ---------------- Private prototypes
/* validateArgs
 *      validates the arguments to startup. Returns true if arguemnts are
 *      valid. If there are invalid arguments, prints a specific error
 *      message and returns false.
 *      Does not validate server responding, that is done in main.
 */
bool validateArgs(int argc, char *argv[]);
void freeSharedMemory();
/*====================================================================*/
#ifndef TESTING
/*
 * int main
 * Validates arguments, parses options, and loops through the provided files.
*/
int main(int argc, char *argv[])
{

        // default values for checking if not passed
        uint32_t nAvatars = 0;
        uint32_t difficulty = 11;

        int s;
        AM_Message initMessage;
        AM_Message initResponse;
        int i;
        pid_t pid;
        int fd_log;
        char *key_path = NULL;
        char *res;
        char *exec_name;
        char *bname;
        size_t path_len = 1;
        char *logFileName;
        int status;
        int algorithm;
        int logLevel;
        FILE *lfile;
        LogMessage lmsg;
        char *hostname = NULL;
        char *usr;
        time_t timeval;
        char *timestr;

        /* catch SIGINT */
        signal(SIGINT, &freeSharedMemory);

        /* Read in args */
        int tempdifficulty;
        algorithm = 2; // Flood maze is default
        logLevel = LVL_INFO; // LVL_INFO is default
        int option = -1;
        while ((option = getopt(argc, argv, "n:d:h:a:l:")) != -1) {
                switch (option) {
                case 'n' :
                        if (0 == sscanf(optarg, "%" SCNu32, &nAvatars)) {
                                printf("%s: nAvatars not a number\n", argv[0]);
                                return false;
                        }
                        if(nAvatars > 10 || nAvatars < 1) {
                            printf("%s: invalid nAvatars: (1 <= nAvatars <= 10)\n",argv[0]);
                            return false;
                        }
                        break;
                case 'd' :
                        if (0 == sscanf(optarg, "%d", &tempdifficulty)) {
                                printf("%s: Difficulty not a number\n", argv[0]);
                                return false;
                        } else if (tempdifficulty > 9 || tempdifficulty < 0) {
                                printf("%s: invalid difficulty: (0 <= difficulty <= 9)\n", argv[0]);
                                return false;
                        }
                        sscanf(optarg, "%" SCNu32, &difficulty);
                    break;
                case 'h' :
                        hostname = calloc(strlen(optarg)+1,sizeof(char));
                        strcpy(hostname, optarg);
                        break;
                case 'a' :
                        if (0 == sscanf(optarg, "%d", &algorithm)) {
                                printf("%s: Algorithm not a number\n", argv[0]);
                                return false;
                        }
                        if(algorithm > 2 || algorithm < 0) {
                            printf("%s: invalid algorithm: (0 <= algorithm <= 2)\n", argv[0]);
                            return false;
                        }
                        break;
                case 'l' :
                        if(0 == sscanf(optarg, "%d", &logLevel)) {
                                printf("%s: Logging level not a number\n", argv[0]);
                                return false;
                        }
                        if(logLevel > 6 || logLevel < 0) {
                            printf("%s: invalid logging level: (0 <= logLevel <= 6)\n", argv[0]);
                            return false;
                        }
                        break;
                default:
                        printf("Usage:\n%s -n nAvatars -d Difficulty -h Hostname [-a algorithm] [-l logLevel]\n", argv[0]);
                        return false;
                }
        }

        if(difficulty == 11 || nAvatars == 0 || NULL == hostname) {
            printf("Usage:\n%s -n nAvatars -d Difficulty -h Hostname [-a algorithm] [-l logLevel] \n", argv[0]);
            return false;
        }

        printf("algorithm is %d\n", algorithm);

        /* connect to server */
        s = connectToServer(hostname, AM_SERVER_PORT);
        if (s == -1) {
                printf("Failed to connect to server\n");
                return -1;
        }

        /* build message to send */
        memset(&initMessage, 0, sizeof(initMessage));
        initMessage.type = AM_INIT;
        initMessage.init.nAvatars = nAvatars;
        initMessage.init.Difficulty = difficulty;

        sendAMMessage(s, initMessage);
        printf("Sending initial message...\n");

        initResponse = receiveAMMessage(s);
        printf("Got server response...\n");

        if (initResponse.type & AM_INIT_FAILED) {
                printAMInitError(initResponse.init_failed.ErrNum);
                return -1;
        }

        /* Get key for shared memory to pass to the agents */
        /* Get working directory */
        do {
                if (key_path != NULL) {
                        free(key_path);
                        path_len *= 2;
                }
                key_path = calloc(path_len, sizeof(char));
                if (key_path == NULL) {
                        fprintf(stderr, "%s:%d: calloc failed. Exiting...\n", __FILE__,
                                        __LINE__);
                        exit(-2);
                }
                res = getcwd(key_path, path_len);
        } while (res == NULL);

        /* Get executable name */
        exec_name = calloc(strlen(argv[0]) + 1, sizeof(char));
        if (exec_name == NULL) {
                fprintf(stderr, "%s:%d: calloc failed. Exiting...\n", __FILE__, __LINE__);
                free(key_path);
                exit(-2);
        }
        strcpy(exec_name, argv[0]);
        bname = basename(exec_name);

        /* Combine path and basename */
        if (path_len < (strlen(key_path) + strlen(bname) + 2))
                key_path = realloc(key_path, strlen(key_path) + strlen(bname) + 2);

        if (key_path == NULL) {
                fprintf(stderr, "%s:%d: realloc failed. Exiting...\n", __FILE__, __LINE__);
                free(exec_name);
                exit(-2);
        }
        key_path[strlen(key_path) + 1] = '\0';
        key_path[strlen(key_path)] = '/';
        strcpy(&key_path[strlen(key_path)], bname);
        if (exec_name != NULL)
                free(exec_name);

        /* make key based on filename of the executable and the character 'A' */
        k = ftok(key_path, 'A');
        if (key_path != NULL)
                free(key_path);
        if (k == -1) {
                perror("ftok");
                exit(-1);
        }
/* fork process */
        /* Construct the name of the logfile */
        logFileName = malloc(strlen("Avatar_csick_#_#.log") + 1);
        if (logFileName == NULL) {
                fprintf(stderr, "%s:%d: realloc failed. Exiting...\n", __FILE__, __LINE__);
                exit(-2);
        }

        sprintf(logFileName, "Avatar_csick_%d_%d.log", (int)nAvatars, (int)difficulty);
        lfile = fopen(logFileName, "a");
        if (lfile == NULL) {
                fprintf(stderr, "[%s]: Could not open log file for appending\n", argv[0]);
                free(logFileName);
                exit(-1);
        }
        free(logFileName);

        /* fork for the log */
        pid = fork();
        if (pid == 0) {
                doLog(lfile, logLevel);
                fclose(lfile);
                printf("Log process exited\n");
                exit(0);
        } else if (pid == -1) {
                perror("fork");
                exit(-1);
        } else {
                printf("spawned log process %ld\n", (long)pid);
        }
        fclose(lfile);

        /* Log basic info to start */
        mkfifo(FIFO_NAME, 0666);
        fd_log = open(FIFO_NAME, O_WRONLY);
        if (fd_log == -1) {
                fprintf(stderr, "%s:%d ", __FILE__, __LINE__);
                perror("open");
                return -1;
        }

        /* Print out inital port and dimensions */
        usr = getenv("USER");
        timeval = time(NULL);
        timestr = ctime(&timeval);
        /* remove trailing newline */
        timestr[strlen(timestr) - 1] = '\0';
        memset(&lmsg, 0, sizeof(lmsg));
        lmsg.level = LVL_INFO;
        sprintf(lmsg.msg, "USER=%s, algorithm=%d, port=%ld, time=%s", usr, algorithm,
                        (long)initResponse.init_ok.MazePort,
                        timestr);
        if (write(fd_log, &lmsg, sizeof(lmsg)) == -1) {
                fprintf(stderr, "%s:%d ", __FILE__, __LINE__);
                perror("write");
                unlink(FIFO_NAME);
                close(fd_log);
                return -1;
        }
        sprintf(lmsg.msg, "Maze size = %ldx%ld", (long)initResponse.init_ok.MazeWidth,\
                        (long)initResponse.init_ok.MazeHeight);
        if (write(fd_log, &lmsg, sizeof(lmsg)) == -1) {
                fprintf(stderr, "%s:%d ", __FILE__, __LINE__);
                perror("write");
                unlink(FIFO_NAME);
                close(fd_log);
                return -1;
        }

        int parentpid = getpid();
        for (i = 0; i < nAvatars; i++) {
                pid = fork();
                switch (pid) {
                        /* fork failed */
                        case -1:
                                perror("fork"); // TODO do I have to free anything?
                                close(fd_log);
                                unlink(FIFO_NAME);
                                exit(-1);

                        /* child function */
                        case 0:
                                status = agent(i, (int)nAvatars, (int)initResponse.init_ok.MazeWidth,
                                                (int)initResponse.init_ok.MazeHeight,hostname,
                                                k, initResponse.init_ok.MazePort,
                                                algorithm);

                                memset(&lmsg, 0, sizeof(lmsg));
                                lmsg.level = LVL_DBG;
                                sprintf(lmsg.msg, "agent %d exited with status %d", i,
                                                status);
                                if (write(fd_log, &lmsg, sizeof(lmsg)) == -1){
                                        fprintf(stderr, "%s:%d ", __FILE__, __LINE__);
                                        perror("write");
                                        close(fd_log);
                                        unlink(FIFO_NAME);
                                        return -1;
                                }
                                if (status < 0) {
                                        lmsg.level = LVL_FATAL;
                                        /* put failure code here */
                                        signal(SIGINT, SIG_IGN);
                                        fprintf(stderr, "%d could not"
                                                        " connect to the "
                                                        "server.\n", i);
                                        kill(-parentpid, SIGINT);
                                }
                                return (status);

                        /* parent function */
                        default:
                                memset(&lmsg, 0, sizeof(lmsg));
                                lmsg.level = LVL_DBG;
                                sprintf(lmsg.msg, "spawned process %ld", (long)pid);
                                if (write(fd_log, &lmsg, sizeof(lmsg)) == -1) {
                                        fprintf(stderr, "%s:%d  ", __FILE__, __LINE__);
                                        perror("write");
                                        close(fd_log);
                                        unlink(FIFO_NAME);
                                        return -1;
                                }
                }
        }

        /* cleanup */
        free(hostname);
        close(s);
        sleep(1);
        close(fd_log);
        unlink(FIFO_NAME);
        while (wait(NULL) != -1) {
        }
        printf("main process exited\n");

        return 0;
}
#endif //TESTING

/** freeSharedMemory
*       Free shared memory in case of SIGINT
**/
void freeSharedMemory(){
        long shmid;
        if ((shmid = shmget(k, sizeof(WorldMap), 0644 | IPC_CREAT)) == -1) {
                perror("shmget");
                fprintf(stderr, "Could not get id for shared memory: %s\n",strerror(errno));
                return;
        }
        shmctl(shmid, IPC_RMID, NULL);
        exit(-2);
}
