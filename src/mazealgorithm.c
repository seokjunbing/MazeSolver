/*  mazealgorithm.c  contains the algorithms used by agent.c

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

// ---------------- Local includes  e.g., "file.h"
#include "mazealgorithm.h"
#include "worldmap.h"
#include "amazing.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
/*
 * getCandidateMove
 *      returns a move following the right-hand rule (right if justMoved, else
 *      left)
 *
 * @wm:        the current state of the world
 * @pos:       the current position of the avatar
 * @prevDir:   the direction the avatar is facing
 * @justMoved: whether the avatar just moved squares
 */
Direction getCandidateMove(WorldMap wm, XYPos pos, Direction prevDir,
                bool justMoved);

/* turnLeft
 *      returns a move one to the left of @d
 *
 * @d:  the direction we're turning from
 */
Direction turnLeft(Direction d);

/* turnRight
 *      returns a move one to the right of @d
 *
 * @d:  the direction we're turning from
 */
Direction turnRight(Direction d);
/*====================================================================*/

Direction getNextMove(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar, int alg)
{
        switch (alg) {
                case 0:
                        return getNextMove0(wm, poses, dir, oldpos, nAvatars,
                                        curAvatar);
                case 1:
                        return getNextMove1(wm, poses, dir, oldpos, nAvatars,
                                        curAvatar);
                case 2:
                        return getNextMove2(wm, poses, dir, oldpos, nAvatars,
                                        curAvatar);
                default:
                        return getNextMove1(wm, poses, dir, oldpos, nAvatars,
                                        curAvatar);
        }
}

Direction getNextMove0(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar)
{
        XYPos pos;
        Direction candidateMove;
        bool justMoved;
        int rotationsMade = 0;
        static Direction d = N;
        bool invalid;
        if (dir != NONE) {
                d = dir;
        }
        candidateMove = d;
        pos = poses[curAvatar];
        justMoved = !(oldpos.x == pos.x && oldpos.y == pos.y);
        do {
                candidateMove = getCandidateMove(wm, pos, candidateMove,
                                justMoved);
                justMoved = false;
                invalid = isWall(wm, pos, candidateMove) ||
                          isDeadEnd(wm, pos, candidateMove) ||
                          (curAvatar < getHighestOccupant(wm, pos,
                                                          candidateMove,
                                                          nAvatars, poses));
        } while(invalid && (rotationsMade++) < 4);
        if (rotationsMade >= 4)
                return NONE;
        else
                return candidateMove;
}

Direction getNextMove1(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar)
{
        XYPos pos;
        Direction candidateMove;
        bool justMoved;
        int rotationsMade = 0;
        static Direction d = N;
        bool invalid;
        bool unknownsNear;
        if (dir != NONE) {
                d = dir;
        }
        candidateMove = d;
        pos = poses[curAvatar];
        unknownsNear = (countAdjacentUnknowns(wm, pos) > 0);
        justMoved = !(oldpos.x == pos.x && oldpos.y == pos.y);

        do {
                candidateMove = getCandidateMove(wm, pos, candidateMove,
                                justMoved);
                justMoved = false;
                invalid = isWall(wm, pos, candidateMove) ||
                          isDeadEnd(wm, pos, candidateMove) ||
                          (curAvatar < getHighestOccupant(wm, pos,
                                                          candidateMove,
                                                          nAvatars, poses));
                /* if there are unknown adjacent to us, choose them but if we
                 * thought they were invalid, once we've checked all
                 * directions, remove that restriction */
                if (unknownsNear && rotationsMade < 4)
                        invalid = invalid || !isUnknown(wm, pos, candidateMove);
        } while(invalid && (rotationsMade++) < 8);
        if (rotationsMade >= 4)
                return NONE;
        else
                return candidateMove;
}


Direction getNextMove2(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar)
{
        static XYPos pos;
        Direction candidateMove;
        bool justMoved;
        int rotationsMade = 0;
        static Direction d = N;
        static Direction prevMove;
        static bool connected = false;
        bool invalid;
        bool unknownsNear;

        if (dir != NONE)
                d = dir;
        candidateMove = d;

        if (pos.x != poses[curAvatar].x || pos.y != poses[curAvatar].y)
                prevMove = dir;
        pos = poses[curAvatar];

        unknownsNear = (countAdjacentUnknowns(wm, pos) > 0);
        justMoved = !(oldpos.x == pos.x && oldpos.y == pos.y);
        if (!connected)
                connected = connectedToAll(wm,pos,nAvatars);
        do {
                candidateMove = getCandidateMove(wm, pos, candidateMove,
                                justMoved);
                justMoved = false;
                invalid = isWall(wm, pos, candidateMove) ||
                          isDeadEnd(wm, pos, candidateMove) ||
                          (curAvatar < getHighestOccupant(wm, pos,
                                                          candidateMove,
                                                          nAvatars, poses));
                /* if we are connected, unknown squares are invalid */
                if (connected)
                        invalid = invalid || isUnknown(wm, pos, candidateMove);
                /* if we aren't connected and there are unknowns next to us,
                 * choose them. if we've rotated an entire rotation and all of
                 * them were invalid for some reason, remove the restriction */
                else if (unknownsNear && rotationsMade < 4)
                        invalid = invalid || !isUnknown(wm, pos, candidateMove);
                else if (rotationsMade == 5)
                        candidateMove = prevMove;
        } while(invalid && (rotationsMade++) < 8);
        if (rotationsMade >= 4)
                return NONE;
        else
                return candidateMove;
}


Direction getCandidateMove(WorldMap wm, XYPos pos, Direction prevDir,
                bool justMoved)
{
        Direction cand;
        if (justMoved)
                cand = turnRight(prevDir);
        else
                cand = turnLeft(prevDir);
        return cand;
}


Direction turnLeft(Direction d)
{
        switch (d) {
                case N:
                        return W;
                case W:
                        return S;
                case S:
                        return E;
                case E:
                        return N;
                default:
                        return NONE;
        }
}

Direction turnRight(Direction d)
{
        switch (d) {
                case N:
                        return E;
                case E:
                        return S;
                case S:
                        return W;
                case W:
                        return N;
                default:
                        return NONE;
        }
}


int getHighestOccupant(WorldMap wm, XYPos pos, Direction d, int nAvatars,
                XYPos all_pos[])
{
        int i;
        int highest = -1;
        XYPos target;
        switch(d) {
                case N:
                        if (pos.y == 0)
                                return -1;
                        else {
                                target.x = pos.x;
                                target.y = pos.y - 1;
                        }
                        break;
                case W:
                        if (pos.x == 0)
                                return -1;
                        else {
                                target.x = pos.x - 1;
                                target.y = pos.y;
                        }
                        break;
                case S:
                        if (pos.y >= wm.ylen - 1)
                                return -1;
                        else {
                                target.x = pos.x;
                                target.y = pos.y + 1;
                        }
                        break;
                case E: if (pos.x >= wm.xlen - 1)
                                return -1;
                        else {
                                target.x = pos.x + 1;
                                target.y = pos.y;
                        }
                        break;
                case NONE:
                        return -1;
        }
        for (i = 0; i < nAvatars; i++) {
                if (all_pos[i].x == target.x && all_pos[i].y == target.y)
                        highest = i;
        }
        return highest;
}

void updateWorldMap(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                int curAvatar, int nAvatars, int alg)
{
        switch (alg) {
                case 0:
                        updateWorldMap0(wm, pos, dir, inM, curAvatar, nAvatars);
                        break;
                case 1:
                        updateWorldMap1(wm, pos, dir, inM, curAvatar, nAvatars);
                        break;
                case 2:
                        updateWorldMap2(wm, pos, dir, inM, curAvatar, nAvatars);
                        break;
        }
}

void updateWorldMap0(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                int curAvatar, int nAvatars)
{
        XYPos newpos;
        newpos = inM.avatar_turn.Pos[curAvatar];
        int i;
        bool occupied = false;
        bool unknownsValid = true;

        /* Add anything we learned about the map to the map */
        if (newpos.x != pos.x || newpos.y != pos.y) {
                /* we made a move, update the log */
                addToWorldMap(wm, pos, dir, OPEN);
                if (1 == countAdjacentOpenings(*wm, pos, unknownsValid)) {
                        for (i = 0; i < nAvatars; i++) {
                                if (inM.avatar_turn.Pos[i].x == pos.x &&
                                                inM.avatar_turn.Pos[i].y ==
                                                pos.y) {
                                        occupied = true;
                                }
                        }
                        if (!occupied)
                                markSquareAsDead(wm, pos);
                }
        } else if ((dir != NONE) &&
                        (newpos.x == pos.x ||
                         newpos.y == pos.y)) {
                /* we tried to move and couldn't, add a wall */
                addToWorldMap(wm, pos, dir, WALL);
        }

        return;
}

void updateWorldMap1(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                int curAvatar, int nAvatars)
{
        XYPos newpos;
        newpos = inM.avatar_turn.Pos[curAvatar];
        int i;
        bool occupied = false;
        bool unknownsValid = true;

        /* Add anything we learned about the map to the map */
        if (newpos.x != pos.x || newpos.y != pos.y) {
                /* we made a move, update the log */
                addToWorldMap(wm, pos, dir, OPEN);
                if (1 == countAdjacentOpenings(*wm, pos, unknownsValid)) {
                        for (i = 0; i < nAvatars; i++) {
                                if (inM.avatar_turn.Pos[i].x == pos.x &&
                                                (inM.avatar_turn.Pos[i].y ==
                                                pos.y)) {
                                        occupied = true;
                                }
                        }
                        if (!occupied)
                                markSquareAsDead(wm, pos);
                }
        } else if ((dir != NONE) &&
                        (newpos.x == pos.x ||
                         newpos.y == pos.y)) {
                /* we tried to move and couldn't, add a wall */
                addToWorldMap(wm, pos, dir, WALL);
        }

        return;
}

void updateWorldMap2(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                int curAvatar, int nAvatars)
{
        XYPos newpos;
        newpos = inM.avatar_turn.Pos[curAvatar];
        int i;
        bool occupied = false;
        bool unknownsValid = true;
        static int connected = 0;

        if(connected < 2) {
                if (connectedToAll(*wm, pos, nAvatars))
                        connected++;
                floodMaze(wm, newpos, curAvatar);
        }

        if(connected)
                unknownsValid = false;

        /* Add anything we learned about the map to the map */
        if (newpos.x != pos.x || newpos.y != pos.y) {
                /* we made a move, update the log */
                addToWorldMap(wm, pos, dir, OPEN);
                if (1 == countAdjacentOpenings(*wm, pos, unknownsValid)) {
                        for (i = 0; i < nAvatars; i++) {
                                if (inM.avatar_turn.Pos[i].x == pos.x &&
                                                (inM.avatar_turn.Pos[i].y ==
                                                 pos.y)) {
                                        occupied = true;
                                }
                        }
                        if (!occupied)
                                markSquareAsDead(wm, pos);
                }
        } else if ((dir != NONE) &&
                        (newpos.x == pos.x ||
                         newpos.y == pos.y)) {
                /* we tried to move and couldn't, add a wall */
                addToWorldMap(wm, pos, dir, WALL);
        }

        return;
}
