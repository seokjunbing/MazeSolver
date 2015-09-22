/* 	mazealgorithm.h  headers for the algorithm functions

    Copyright 2015

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    ======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef INC_MAZEALGORITHM_H
#define INC_MAZEALGORITHM_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "worldmap.h"
#include "amazing.h"

// ---------------- Constants
#define RIGHT_WALL_NAIVE 0
#define UNKNOWN_FIRST 1
#define CHECK_CONNECTION 2

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/**getNextMove0
*   Decides the next move for algorithm 0, right-hand rule
*   @wm- shared worldmap for avatars
*   @poses- positions of avatar
*   @dir- direction to move
*   @oldpos- previous position
*   @nAvatars- total avatars
*   @curAvatar- current avatar
**/
Direction getNextMove0(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar);

/**getNextMove1
*   Decides the next move for algorithm 1, right-hand rule unknowns first
*   @wm- shared worldmap for avatars
*   @poses- positions of avatar
*   @dir- direction to move
*   @oldpos- previous position
*   @nAvatars- total avatars
*   @curAvatar- current avatar
**/
Direction getNextMove1(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar);

/**getNextMove2
*   Decides the next move for algorithm 2:
*       unknowns first if not connected to all
*       no unknows if connected to all
*   @wm- shared worldmap for avatars
*   @poses- positions of avatar
*   @dir- direction to move
*   @oldpos- previous position
*   @nAvatars- total avatars
*   @curAvatar- current avatar
**/
Direction getNextMove2(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                int nAvatars, int curAvatar);

/**getNextMove
*   Switch case that calls getNextMove0, getNextMove1, or getNextMove2
*       based on given algorithm.
*   @wm- shared worldmap for avatars
*   @poses- positions of avatar
*   @dir- direction to move
*   @oldpos- previous position
*   @nAvatars- total avatars
*   @curAvatar- current avatar
*   @alg- number of selected algorithm
**/
Direction getNextMove(WorldMap wm, XYPos poses[], Direction dir, XYPos oldpos,
                      int nAvatars, int curAvatar, int alg);

/**updateWorldMap
 *  Switch case that calls updateWorldMap0, updateWorldMap1, or updateWorldMap2
 *  @wm- pointer to shared worldmap for avatars
 *  @pos- position of the avatar
 *  @dir- the direction the avatar was facing
 *  @inM- the message from the server
 *  @curAvatar- which avatar to update
 *  @nAvatars- total avatars
 *  @alg- 0, 1, or 2, the algorithm used
**/
void updateWorldMap(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                    int curAvatar, int nAvatars, int alg);

/**updateWorldMap0
 *  Updates the current state of the world for algorithm 1
 *  @wm- pointer to shared worldmap for avatars
 *  @pos- position of the avatar
 *  @dir- the direction the avatar was facing
 *  @inM- the message from the server
 *  @curAvatar- which avatar to update
 *  @nAvatars- total avatars
**/
void updateWorldMap0(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                    int curAvatar, int nAvatars);

/**updateWorldMap1
 *  Updates the current state of the world for algorithm 1
 *  @wm- pointer to shared worldmap for avatars
 *  @pos- position of the avatar
 *  @dir- the direction the avatar was facing
 *  @inM- the message from the server
 *  @curAvatar- which avatar to update
 *  @nAvatars- total avatars
**/ 
void updateWorldMap1(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                    int curAvatar, int nAvatars);

/**updateWorldMap2
 *  Updates the current state of the world for algorithm 2, including
 *      flooding connected state.
 *  @wm- pointer to shared worldmap for avatars
 *  @pos- position of the avatar
 *  @dir- the direction the avatar was facing
 *  @inM- the message from the server
 *  @curAvatar- which avatar to update
 *  @nAvatars- total avatars
**/
void updateWorldMap2(WorldMap *wm, XYPos pos, Direction dir, AM_Message inM,
                    int curAvatar, int nAvatars);

/**getHighestOccupant
 *  Retrieves the avatarID of the highest avatar in the given square
 *  @wm- current shared worldmap
 *  @pos- position the agent is in
 *  @d- the direction the agent is moving towards
 *  @nAvatars- how many avatars there are
 *  @all_pos- list of all the positions
**/
int getHighestOccupant(WorldMap wm, XYPos pos, Direction d, int nAvatars, XYPos all_pos[]);

#endif // INC_MAZEALGORITHM_H

