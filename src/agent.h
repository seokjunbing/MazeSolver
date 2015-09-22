/*  agent.h one-line description

    Copyright 2015

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    ======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef INC_AGENT_H
#define INC_AGENT_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "worldmap.h"
#include <sys/types.h>
#include <sys/ipc.h>


// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/**
 * agent
 *      agent is the main loop of the solving program. Each agent runs independently.
 *
 * @curAvatar: the 0-based index of the running avatar
 * @nAvatars:  the total number of avatars
 * @xlen:      the x-dimension of the maze
 * @ylen:      the y-dimension of the maze
 * @hostname:  the web address to open to connect to the server
 * @k:         the key used to attach to shared memory
 * @port:      the port on which to connect to the server
 * @algo:      the algorithm with which to solve (1,2, or 3)
 */
int agent(int curAvatar, int nAvatars, int xlen, int ylen, char *hostname, key_t k,
                uint32_t port, int algo);

#endif // INC_AGENT_H

