/* 	file.h one-line description

    Copyright 2015

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    ======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef INC_FILE_H
#define INC_FILE_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants
#define DEAD_END     0x0400
#define TEMP_FLOOD   0x0800

// ---------------- Structures/Types

// ---------------- Public Variables


// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <pthread.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "amazing.h"


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types
typedef enum { NONE, N, W, E, S } Direction;

typedef enum { WALL,
               OPEN,
               UNKNOWN
} WallState;

typedef struct {
        WallState N;
        WallState W;
        WallState E;
        WallState S;
        int state;
} Square;

typedef struct {
        size_t xlen; /* the logical size of the grid */
        size_t ylen;
        Square grid[AM_MAX_GRID_SIZE*AM_MAX_GRID_SIZE];
        pthread_mutex_t mapMutex;
        pthread_mutexattr_t mapMutexAttr;
} WorldMap;


// ---------------- Prototypes/Macros

/** initWorldMap
 *      Takes a worldMap which has had the xlen and ylen set to the correct logical
 *      size and fills the grid with arrays of the correct size
 *
 * @wm: a pointer to the worldmap to be initialized
 */
result_t initWorldMap(WorldMap *wm);

/** addToWorldMap
 *      This function should be a thread-safe add function, which adds a
 *      piece of knowledge to the worldmap. @w will be shared by all the threads.
 *
 * @wm: the worldmap itself, sized for a nxn maze at (2*n+1)x(2*n+1). For an example,
 *      see the project README
 * @x, @y: the logical maze coordinate of the current agent (the function is not
 *      sensitive to which agent, it just cares about building a map of the world)
 * @dir: an enum type which is either N, S, E, W, and tells which direction the
 *      calling agent tried to move
 * @move_success: whether the attempted move was successful (if not, there was a
 *      wall there
 */
result_t addToWorldMap(WorldMap *wm, XYPos pos, Direction dir, WallState wstate);

/** serializeWorldMap
 *      Returns a string of the given world map
 *
 * @wm: the worldmap to print
 * @nAvatars: the number of avatars in the maze. Used to index through @positions
 * @positions: a list of the positions at which the avatar can be found
 */
char *serializeWorldMap(WorldMap wm, int nAvatars, XYPos positions[]);

/** isValidMove
 *      returns whether a possible move is valid (does not hit a known wall or
 *      a known dead-end)
 *
 * @wm: the worldmap to use as reference
 * @pos: the agent's logical coordinates
 * @dir: the N,S,E,W direction of the candidate move
 * @unknownsValid: whether unknown squares are valid recipients of a move
 */
bool isValidMove(WorldMap wm, XYPos pos, Direction dir, bool unknownsValid);

/** countAdjacentUnknowns
 *      returns the number of unknown walls surrounding the square we're in.
 *      used to bias towards exploring unexplored territory first
 * @wm: the worldmap to use as reference
 * @pos: the agent's logical coordinates
 * @dir: the N,S,E,W direction of the candidate move
 */
int countAdjacentUnknowns(WorldMap wm, XYPos pos);

/** isUnknown
*       returns  boolean indicating whether the position of the maze is unknown (unexplored)
* @wm: the worldmap to use as reference
* @pos: the agent's logical coordinates
* @dir: the N,S,E,W direction of the candidate move
*/
bool isUnknown(WorldMap wm, XYPos pos, Direction dir);

/** isWall
*       returns  boolean indicating whether the position of the maze is a wall
* @wm: the worldmap to use as reference
* @pos: the agent's logical coordinates
* @dir: the N,S,E,W direction of the candidate move
*/
bool isWall(WorldMap wm, XYPos pos, Direction dir);

/** isDeadEnd
*       returns  boolean indicating whether the position of the maze is a deadend
* @wm: the worldmap to use as reference
* @pos: the agent's logical coordinates
* @dir: the N,S,E,W direction of the candidate move
* @return: boolean indicating whether the position of the maze is a deadend
*/
bool isDeadEnd(WorldMap wm, XYPos pos, Direction dir);

/**
 * markSquareAsDead
 *      when the Square is marked as dead it is no longer accessible by the Avatars
 * @wm: The worldmap to use as reference
 * @pos: The position of the Square
 * @return: Success  or Failure
 */
result_t markSquareAsDead(WorldMap *wm, XYPos pos);

/**
 * countAdjacentOpenings
 *      counts the number of valid adjacent openings
 * @wm: The worldmap to use as reference
 * @pos: The position of the Square
 * @unknownsValid: whether or not unknown square is a valid path to take
 * @return: integer value of number of openings
 */
int countAdjacentOpenings(WorldMap wm, XYPos pos, bool unknownsValid);

/**
 * connectedToAll
 *      determines whether a square is connected to every Avatar
 * @wm: The worldmap to use as reference
 * @pos: The position of the Square
 * @nAvatars: number of Avatars
 * @return: boolean whether the square is connected to all Avatars
 */
bool connectedToAll(WorldMap wm, XYPos pos, int nAvatars);

/**
 * maskState
 *     Applies a bitmask to the state of a given
 *     square on the grid, as indexed
 *     by an XYPos struct.
 * @param wm:   a pointer to the world map to use as reference
 * @param pos:  the position of the square of whom to mask the state
 * @param mask: the integer mask to apply
 */
void maskState(WorldMap *wm, XYPos pos, int mask);

/**
 * posAfterMove
 *     Returns the XYPos on the map after a move has been made
 *     in a given direction from a given starting position.
 * @param  wm  world map to navigate
 * @param  pos starting position
 * @param  dir direction to move in
 * @return     XYPos showing position after move
 */
XYPos posAfterMove(WorldMap wm, XYPos pos, Direction dir);

/**
 * floodMazeConnectivity
 *     Updates the worldmap with a given avatar's connected squares
 * @param global_wm    [description]
 * @param starting_pos [description]
 * @param curAvatar    [description]
 */
void floodMazeConnectivity(WorldMap *global_wm, XYPos starting_pos, int curAvatar);

/** floodMaze
*       Floods maze and updates WorldMap
* @global_wm- WorldMap
* @starting_pos- starting position
* @curAvatar- current avatar
**/
void floodMaze(WorldMap *global_wm, XYPos starting_pos, int curAvatar);

/** percentKnown
 *      returns a percentage (0<%<1) of the maze that some avatar visited
 * @wm- WorldMap
 * @nAvatars- how many avatars there are in the maze
 */
float percentKnown(WorldMap wm, int nAvatars);


#endif // INC_FILE_H

