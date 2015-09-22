/*  worldmap.c  contains the datastructure and functions for a map of the maze

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "worldmap.h"
#include "amazing.h"
#include "queue.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/** inferWalls
 *      goes through and adds all the walls that are implied by the wall from the adjacent
 *      square
 * @wm: the reference worldmap
 */
result_t inferWalls(WorldMap *wm);

/** isStatus
*       checks if a wall is of certain state
* @wm: the reference worldmap
* @pos: position of the square
*/
bool isStatus(WorldMap wm, XYPos pos, Direction dir, WallState s);

/** connectedToN
*       determines a square is connected to a certain Avatar
* @wm: the reference worldmap
* @pos: position of the square
* @n: Avartar number
* @return: boolean as to whether it is connected
*/
bool connectedToN(WorldMap wm, XYPos pos, int n);

/** hasMask
*        determines if the square has a mask
* @wm: the reference worldmap
* @pos: position of the square
* @mask: mask
* @return: boolean as to whether it is masked
*/
bool hasMask(WorldMap wm, XYPos pos, int mask);


/*====================================================================*/

result_t initWorldMap(WorldMap *wm)
{
        size_t x, y;
        int i, j;
        if (wm == NULL)
                return RESULT_FAILURE;
        if (wm->xlen == 0 || wm->ylen == 0)
                return RESULT_FAILURE;
        x = wm->xlen;
        y = wm->ylen;
        if (x > 100 || y > 100)
                return RESULT_FAILURE;

        if (wm->grid == NULL)
                return RESULT_FAILURE;

        for (j = 0; j < y; j++) {
                for (i = 0; i < x; i++) {
                        ((wm->grid)[j*x + i]).state = 0;

                        wm->grid[j*x + i].N = UNKNOWN;
                        wm->grid[j*x + i].S = UNKNOWN;
                        wm->grid[j*x + i].E = UNKNOWN;
                        wm->grid[j*x + i].W = UNKNOWN;
                        if (i == 0)
                                wm->grid[j*x + i].W = WALL;
                        if (i == (x - 1))
                                wm->grid[j*x + i].E = WALL;
                        if (j == 0) {
                                wm->grid[j*x + i].N = WALL;
                        }
                        if (j == (y - 1))
                                wm->grid[j*x + i].S = WALL;
                }
        }

        if(pthread_mutexattr_init(&wm->mapMutexAttr) < 0){
                fprintf(stderr,"mutex attr init failed");
                return RESULT_FAILURE;
        }
        if(pthread_mutexattr_setpshared(&wm->mapMutexAttr, PTHREAD_PROCESS_SHARED) < 0) {
                fprintf(stderr,"mutex attr setting failure");
                return RESULT_FAILURE;
        }
        // initialize mutex using default attributes
        if (pthread_mutex_init(&wm->mapMutex, &wm->mapMutexAttr) < 0) {
                fprintf(stderr, "mutex init failed");
                return RESULT_FAILURE;
        }
        pthread_mutexattr_destroy(&wm->mapMutexAttr);
        return RESULT_SUCCESS;
}

char *serializeWorldMap(WorldMap wm, int nAvatars, XYPos positions[])
{
        size_t x, y;
        char *res;
        char *pos;
        int i, j, k;
        j = 0;
        if (wm.xlen == 0 || wm.ylen == 0)
                return NULL;

        x = wm.xlen;
        y = wm.ylen;

        res = calloc(((2 * x + 2) * (2 * y + 2)), sizeof(char));
        pos = res;

        /* Print first line */
        for (i = 0; i < x; i++) {
                /* Diagonal square */
                strncpy(pos, "+", 1);
                pos++;

                if (wm.grid[i].N == UNKNOWN)
                        strncpy(pos, ".", 1);
                else if (wm.grid[i].N == WALL)
                        strncpy(pos, "-", 1);
                else
                        strncpy(pos, " ", 1);
                pos++;
        }
        strncpy(pos, "+", 1);
        pos++;

        strncpy(pos, "\n", 1);
        pos++;

        /* Print all the lines in the middle */
        for (i = 0; i < y; i++) {
                /* Left wall */
                if (wm.grid[i*x + 0].W == WALL)
                        strncpy(pos, "|", 1);
                else if (wm.grid[i*x + 0].W == UNKNOWN)
                        strncpy(pos, ".", 1);
                pos++;

                /* Print square themselves for this go-round */
                for (j = 0; j < x; j++) {
                        /* Print the square */
                        if (!(wm.grid[i*x + j].state & DEAD_END)) {
                                strncpy(pos, " ", 1);
                        } else if (wm.grid[i*x + j].state & DEAD_END) {
                                strncpy(pos, "X", 1);
                        }
                        /* check whether there is an avatar in the square */
                        for (k = 0; k < nAvatars; k++) {
                                if (i == positions[k].y && j == positions[k].x)
                                        *pos = '1' + k;
                        }
                        pos++;

                        /* Print the right wall of this square */
                        if (wm.grid[i*x + j].E == UNKNOWN) {
                                strncpy(pos, ".", 1);
                        } else if (wm.grid[i*x + j].E == WALL) {
                                strncpy(pos, "|", 1);
                        } else {
                                strncpy(pos, " ", 1);
                        }
                        pos++;
                }
                /* New line */
                strncpy(pos, "\n", 1);
                pos++;

                /* Row of walls or gaps */
                for (j = 0; j < x; j++) {
                        /* Diagonal square */
                        strncpy(pos, "+", 1);
                        pos++;

                        /* Directly above or below square */
                        if (wm.grid[i*x + j].S == WALL)
                                strncpy(pos, "-", 1);
                        else if (wm.grid[i*x + j].S == OPEN)
                                strncpy(pos, " ", 1);
                        else
                                strncpy(pos, ".", 1);
                        pos++;
                }
                /* Final square on the outside */
                strncpy(pos, "+", 1);
                pos++;
                strncpy(pos, "\n", 1);
                pos++;
        }


        return res;

}




result_t inferWalls(WorldMap *wm)
{
        size_t i, j;
        size_t width;

        if (wm == NULL)
                return RESULT_FAILURE;
        width = wm->xlen;

        for (j = 0; j < wm->ylen; j++) {
                for (i = 0; i < wm->xlen; i++) {
                        /* Infer walls */
                        if (wm->grid[j*width + i].W == WALL &&
                                        i > 0)
                                wm->grid[j*width + (i-1)].E = WALL;
                        if (wm->grid[j*width + i].N == WALL &&
                                        j > 0)
                                wm->grid[(j-1)*width + i].S = WALL;
                        if (wm->grid[j*width + i].E == WALL &&
                                        i < (width - 1))
                                wm->grid[j*width + (i+1)].W = WALL;
                        if (wm->grid[j*width + i].S == WALL &&
                                        j < (wm->ylen-1))
                                wm->grid[(j+1)*width + i].N = WALL;

                        /* Also fill in spaces */
                        if (wm->grid[j*width + i].W == OPEN &&
                                        i > 0)
                                wm->grid[j*width + (i-1)].E = OPEN;
                        if (wm->grid[j*width + i].N == OPEN &&
                                        j > 0)
                                wm->grid[(j-1)*width + i].S = OPEN;
                        if (wm->grid[j*width + i].E == OPEN &&
                                        i < (width - 1))
                                wm->grid[j*width + (i+1)].W = OPEN;
                        if (wm->grid[j*width + i].S == OPEN &&
                                        j < (wm->ylen-1))
                                wm->grid[(j+1)*width + i].N = OPEN;
                }
        }
        return RESULT_SUCCESS;
}

result_t markSquareAsDead(WorldMap *wm, XYPos pos) {
        if( wm->xlen <= pos.x || wm->ylen <= pos.y)
                return RESULT_FAILURE;
        maskState(wm, pos, DEAD_END);
        return RESULT_SUCCESS;
}


result_t addToWorldMap(WorldMap *wm, XYPos pos, Direction dir, WallState wstate)
{
        if (wm == NULL)
                return RESULT_FAILURE;

        switch (dir) {
                case N:
                        wm->grid[pos.y*wm->xlen + pos.x].N = wstate;
                        break;
                case S:
                        wm->grid[pos.y*wm->xlen + pos.x].S = wstate;
                        break;
                case E:
                        wm->grid[pos.y*wm->xlen + pos.x].E = wstate;
                        break;
                case W:
                        wm->grid[pos.y*wm->xlen + pos.x].W = wstate;
                        break;
                default:
                        return RESULT_FAILURE;
        }
        inferWalls(wm);
        return RESULT_SUCCESS;
}


bool isValidMove(WorldMap wm, XYPos pos, Direction dir, bool unknownsValid)
{
        bool target_unknown;
        bool invalidUnknownMove;
        if (wm.xlen == 0 || wm.ylen == 0)
                return false;
        target_unknown = isUnknown(wm, pos, dir);

        invalidUnknownMove = ((!unknownsValid) && target_unknown);

        if (isWall(wm, pos, dir) ||
                        isDeadEnd(wm, pos, dir) ||
                        invalidUnknownMove)
                return false;
        return true;
}

XYPos posAfterMove(WorldMap wm, XYPos newpos, Direction dir)
{
        switch (dir) {
                case N:
                        if (newpos.y > 0)
                                newpos.y -= 1;
                        break;
                case S:
                        if (newpos.y < wm.ylen-1)
                                newpos.y += 1;
                        break;
                case E:
                        if (newpos.x < wm.xlen - 1)
                                newpos.x += 1;
                        break;
                case W:
                        if (newpos.x > 0)
                                newpos.x -= 1;
                        break;
                default:
                        break;
        }
        return newpos;
}

int countAdjacentOpenings(WorldMap wm, XYPos pos, bool unknownsValid)
{
        int i;
        int count = 0;
        Direction dirs[] = {N, S, E, W};

        for (i = 0; i < 4; i++) {
                if (isValidMove(wm, pos, dirs[i],unknownsValid))
                        count++;
        }
        return count;
}

int countAdjacentUnknowns(WorldMap wm, XYPos pos)
{
        int i;
        int count = 0;
        Direction dirs[] = {N, W, S, E};

        for (i = 0; i < 4; i++) {
                if (isUnknown(wm, pos, dirs[i]))
                        count++;
        }
        return count;
}

bool isDeadEnd(WorldMap wm, XYPos pos, Direction dir)
{
        int i, j;
        i = pos.y*wm.xlen + pos.x;

        if (wm.xlen == 0 || wm.ylen == 0)
                return false;

        pos = posAfterMove(wm, pos, dir);
        j = pos.y*wm.xlen + pos.x;

        if (i != j) {
                if (wm.grid[j].state & DEAD_END)
                        return true;
                else {
                        return false;
                }
        } else {
                return true;
        }
}

bool isUnknown(WorldMap wm, XYPos pos, Direction dir)
{
        return isStatus(wm, pos, dir, UNKNOWN);
}

bool isWall(WorldMap wm, XYPos pos, Direction dir)
{
        return isStatus(wm, pos, dir, WALL);
}

bool isStatus(WorldMap wm, XYPos pos, Direction dir, WallState s)
{
        int i;
        i = pos.y*wm.xlen + pos.x;

        if (wm.xlen == 0 || wm.ylen == 0)
                return false;

        switch (dir) {
                case N:
                        if (wm.grid[i].N == s)
                                return true;
                        break;
                case W:
                        if (wm.grid[i].W == s)
                                return true;
                        break;
                case S:
                        if (wm.grid[i].S == s)
                                return true;
                        break;
                case E:
                        if (wm.grid[i].E == s)
                                return true;
                        break;
                default:
                        return false;
        }
        return false;
}


bool connectedToN(WorldMap wm, XYPos pos, int n)
{
        int mask = 0;
        int i = pos.y*wm.xlen + pos.x;
        mask = pow(2, n);
        return ((wm.grid[i].state & mask) > 0);
}

bool connectedToAll(WorldMap wm, XYPos pos, int nAvatars)
{
        int i= pos.y*wm.xlen + pos.x;
        int mask = 0;
        int k;
        for(k=0; k<nAvatars; k++)
                mask += pow(2, k);
        return ((wm.grid[i].state & mask) == mask);
}

void maskState(WorldMap *wm, XYPos pos, int mask) {
        int i = pos.y*wm->xlen + pos.x;
        wm->grid[i].state = wm->grid[i].state | mask;
}

void printPos(XYPos pos) {
        fprintf(stderr,"(%d,%d)\n",pos.x,pos.y);
}

int posToIndex(WorldMap wm, XYPos pos) {
        return pos.y*wm.xlen + pos.x;
}

/*
 * Flood fills the maze
 * This is a non-recursive version of the previous flood fill function.
 * Recursive flood fill was too stack heavy
 */
void floodMaze(WorldMap *global_wm, XYPos starting_pos, int curAvatar) {
        XYPos new_pos = starting_pos;
        Queue *q = calloc(1,sizeof(Queue));
        int i, j, ind;

        initializeQueue(q);
        push(q, new_pos);

        while(q->head->next != q->tail) {
                new_pos = pop(q);
                /*fprintf(stderr, "new_pos=(%d,%d)\n", new_pos.x, new_pos.y);*/

                /* if we got it in the queue mark it connected */
                maskState(global_wm, new_pos, (int)pow(2, curAvatar));
                maskState(global_wm, new_pos, TEMP_FLOOD);

                if(isValidMove(*global_wm, new_pos, N, false)) { /* false sets unknowns invalid */
                        if (!hasMask(*global_wm,
                                                posAfterMove(*global_wm, new_pos, N),
                                                TEMP_FLOOD))
                                push(q, posAfterMove(*global_wm, new_pos, N));
                }

                if(isValidMove(*global_wm, new_pos, S, false)) {
                        if (!hasMask(*global_wm,
                                                posAfterMove(*global_wm, new_pos, S),
                                                TEMP_FLOOD))
                                push(q, posAfterMove(*global_wm, new_pos, S));
                }

                if(isValidMove(*global_wm, new_pos, E, false)) {
                        if (!hasMask(*global_wm,
                                                posAfterMove(*global_wm, new_pos, E),
                                                TEMP_FLOOD))
                                push(q, posAfterMove(*global_wm, new_pos, E));
                }

                if(isValidMove(*global_wm, new_pos, W, false)) {
                        if (!hasMask(*global_wm,
                                                posAfterMove(*global_wm, new_pos, W),
                                                TEMP_FLOOD))
                                push(q, posAfterMove(*global_wm, new_pos, W));
                }
        }
        for (i = 0; i < global_wm->xlen; i++) {
                for (j = 0; j < global_wm->ylen; j++) {
                        ind = j*global_wm->xlen + i;
                        global_wm->grid[ind].state &= ~TEMP_FLOOD;
                }
        }
        destroyQueue(q);
        free(q);
}


bool hasMask(WorldMap wm, XYPos pos, int mask)
{
        int i = pos.y*wm.xlen + pos.x;
        return ((wm.grid[i].state & mask) == mask);
}


float percentKnown(WorldMap wm, int nAvatars)
{
        int i, j;
        int index;
        int mask = 0;
        int n = 0;
        int known = 0;
        for (i = 0; i < nAvatars; i++) {
                mask |= (int)pow(2, i);
        }
        for (i = 0; i < wm.xlen; i++) {
                for (j = 0; j < wm.ylen; j++) {
                        index = j*wm.xlen + i;
                        n++;
                        if (mask & wm.grid[index].state)
                                known++;
                }
        }
        return (float)known/(float)n;
}
