/*  agent_test.c  one-line description

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
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// ---------------- Local includes  e.g., "file.h"
#include "../src/worldmap.h"
#include "../src/agent.h"
#include "../src/mazealgorithm.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
void serializeWorldMap_test(void);
void addToWorldMap_test(void);
void countAdjacentOpenings_test(void);
void getHighestOccupant_test(void);
void isValidMove_test(void);
void getNextMove_test(void);
void isDeadEnd_test(void);
void isUnknown_test(void);

/*====================================================================*/

int main(void)
{
        serializeWorldMap_test();
        /*inferWalls_test();*/
        addToWorldMap_test();
        countAdjacentOpenings_test();
        getHighestOccupant_test();
        isValidMove_test();
        isDeadEnd_test();
        isUnknown_test();
        getNextMove_test();
        return 0;
}

void serializeWorldMap_test(void)
{
        WorldMap wm;
        char *serialized;
        char target1[] = "+-+\n"
                         "|1|\n"
                         "+-+\n";
        char target2[] = "+-+-+\n"
                         "|1. |\n"
                         "+.+.+\n"
                         "| .2|\n"
                         "+-+-+\n";
        char target3[] = "+-+-+-+\n"
                         "|1. . |\n"
                         "+.+.+.+\n"
                         "|3.2. |\n"
                         "+.+.+.+\n"
                         "| . . |\n"
                         "+-+-+-+\n";
        XYPos positions[3];
        positions[0].x = 0;
        positions[0].y = 0;
        positions[1].x = 1;
        positions[1].y = 1;
        positions[2].x = 0;
        positions[2].y = 1;

        wm.xlen = 1;
        wm.ylen = 1;
        initWorldMap(&wm);

        serialized = serializeWorldMap(wm, 1, positions);
        assert(strcmp(target1, serialized) == 0);

        free(serialized);

        wm.xlen = 2;
        wm.ylen = 2;
        initWorldMap(&wm);

        serialized = serializeWorldMap(wm, 2, positions);
        assert(0 == strcmp(target2, serialized));

        free(serialized);

        wm.xlen = 3;
        wm.ylen = 3;
        initWorldMap(&wm);

        serialized = serializeWorldMap(wm, 3, positions);
        assert(0 == strcmp(target3, serialized));
        free(serialized);
        return;
}

void addToWorldMap_test(void)
{
        WorldMap wm;
        char *test;
        char result0[] = "+-+-+\n"
                         "| | |\n"
                         "+ +-+\n"
                         "|   |\n"
                         "+-+-+\n";
        XYPos pos;

        wm.xlen = 2;
        wm.ylen = 2;
        initWorldMap(&wm);

        pos.x = 0;
        pos.y = 0;
        addToWorldMap(&wm, pos, E, WALL);

        pos.x = 1;
        pos.y = 0;
        addToWorldMap(&wm, pos, S, WALL);

        pos.x = 0;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, W, OPEN);

        test = serializeWorldMap(wm, 0, NULL);
        assert(0 == strcmp(test, result0));
        free(test);
        return;
}

void countAdjacentOpenings_test(void)
{
        WorldMap wm;
        /* Map we're creating:
         * +-+-+
         * | | |
         * + +-+
         * |   |
         * +-+-+
         */
        XYPos pos;

        wm.xlen = 2;
        wm.ylen = 2;
        initWorldMap(&wm);

        pos.x = 0;
        pos.y = 0;
        addToWorldMap(&wm, pos, E, WALL);

        pos.x = 1;
        pos.y = 0;
        addToWorldMap(&wm, pos, S, WALL);

        pos.x = 0;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, W, OPEN);

        pos.x = 0;
        pos.y = 0;
        assert(1 == countAdjacentOpenings(wm, pos, true));

        pos.x = 1;
        pos.y = 0;
        assert(0 == countAdjacentOpenings(wm, pos, true));

        pos.x = 0;
        pos.y = 1;
        assert(2 == countAdjacentOpenings(wm, pos, true));

        pos.x = 1;
        pos.y = 1;
        assert(1 == countAdjacentOpenings(wm, pos, true));

        return;
}

void getHighestOccupant_test(void)
{
        WorldMap wm;
        XYPos poses[4];
        int test;

        wm.xlen = 2;
        wm.ylen = 2;
        initWorldMap(&wm);

        poses[0].x = 0; // +-+-+
        poses[0].y = 0; // |0.1|
        poses[1].x = 1; // +.+.+
        poses[1].y = 0; // |2.3|
        poses[2].x = 0; // +-+-+
        poses[2].y = 1;
        poses[3].x = 1;
        poses[3].y = 1;

        test = getHighestOccupant(wm, poses[0], N, 4, poses);
        assert(-1 == test);

        test = getHighestOccupant(wm, poses[1], N, 4, poses);
        assert(-1 == test);

        test = getHighestOccupant(wm, poses[2], N, 4, poses);
        assert(0 == test);

        test = getHighestOccupant(wm, poses[3], N, 4, poses);
        assert(1 == test);

        test = getHighestOccupant(wm, poses[0], E, 4, poses);
        assert(1 == test);

        test = getHighestOccupant(wm, poses[0], S, 4, poses);
        assert(2 == test);
        return;
}

void isDeadEnd_test(void)
{
        WorldMap wm;
        XYPos pos;
        bool test;
        /*
         * +-+-+-+
         * | |   |
         * + + +-+
         * |X  . |
         * +-+ + +
         * |  X| |
         * +-+-+-+ */

        wm.xlen = 3;
        wm.ylen = 3;
        initWorldMap(&wm);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);
        addToWorldMap(&wm, pos, S, OPEN);
        addToWorldMap(&wm, pos, W, OPEN);

        pos.x = 0;
        pos.y = 1;
        markSquareAsDead(&wm, pos);

        pos.x = 1;
        pos.y = 2;
        markSquareAsDead(&wm, pos);

        pos.x = 0;
        pos.y = 0;
        test = isDeadEnd(wm, pos, S);
        assert(test == true);
        test = isDeadEnd(wm, pos, E);
        assert(test == false);
}

void isUnknown_test(void)
{
        WorldMap wm;
        XYPos pos;
        bool test;
        /*
         * +-+-+-+
         * | |   |
         * + + +-+
         * |X  . |
         * +-+ + +
         * |  X| |
         * +-+-+-+ */

        wm.xlen = 3;
        wm.ylen = 3;
        initWorldMap(&wm);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);
        addToWorldMap(&wm, pos, S, OPEN);
        addToWorldMap(&wm, pos, W, OPEN);

        pos.x = 0;
        pos.y = 1;
        markSquareAsDead(&wm, pos);

        pos.x = 1;
        pos.y = 2;
        markSquareAsDead(&wm, pos);

        pos.x = 2;
        pos.y = 1;
        test = isUnknown(wm, pos, W);
        assert(test == true);
        addToWorldMap(&wm, pos, N, WALL);
        test = isUnknown(wm, pos, N);
        assert(test == false);
}


void isValidMove_test(void)
{
        WorldMap wm;
        XYPos pos;
        bool test;

        wm.xlen = 3;
        wm.ylen = 3;
        initWorldMap(&wm);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);
        addToWorldMap(&wm, pos, S, OPEN);
        addToWorldMap(&wm, pos, E, OPEN);
        addToWorldMap(&wm, pos, W, OPEN);

        pos.x = 0;
        pos.y = 1;
        markSquareAsDead(&wm, pos);

        pos.x = 1;
        pos.y = 0;
        markSquareAsDead(&wm, pos);

        pos.x = 2;
        pos.y = 1;
        markSquareAsDead(&wm, pos);

        pos.x = 1;
        pos.y = 2;
        markSquareAsDead(&wm, pos);

        pos.x = 1;
        pos.y = 1;
        test = isValidMove(wm, pos, N, true);
        assert(test == false);
        test = isValidMove(wm, pos, W, true);
        assert(test == false);
        test = isValidMove(wm, pos, S, true);
        assert(test == false);
        test = isValidMove(wm, pos, E, true);
        assert(test == false);

        return;
}

void getNextMove_test(void)
{
        WorldMap wm;
        XYPos pos;
        XYPos oldpos;
        Direction test;
        /*
         * +-+-+-+
         * | |   |
         * + + +-+
         * |X  . |
         * +-+ + +
         * |  X| |
         * +-+-+-+ */

        wm.xlen = 3;
        wm.ylen = 3;
        initWorldMap(&wm);

        pos.x = 1;
        pos.y = 1;
        addToWorldMap(&wm, pos, N, OPEN);
        addToWorldMap(&wm, pos, S, OPEN);
        addToWorldMap(&wm, pos, W, OPEN);

        pos.x = 0;
        pos.y = 1;
        markSquareAsDead(&wm, pos);

        /*pos.x = 1;*/
        /*pos.y = 0;*/
        /*markSquareAsDead(&wm, pos);*/

        /*pos.x = 2;*/
        /*pos.y = 1;*/
        /*markSquareAsDead(&wm, pos);*/

        pos.x = 1;
        pos.y = 2;
        markSquareAsDead(&wm, pos);

        /* Test algorithm 0 */
        pos.x = 0;
        pos.y = 2;
        oldpos.x = 1;
        oldpos.y = 2;
        test = getNextMove(wm, &pos, N, oldpos, 1, 0, RIGHT_WALL_NAIVE);
        assert(test == NONE);

        pos.x = 0;
        pos.y = 1;
        oldpos.x = 1;
        oldpos.y = 1;
        test = getNextMove(wm, &pos, W, oldpos, 1, 0, RIGHT_WALL_NAIVE);
        assert(test == N);

        pos.x = 1;
        pos.y = 0;
        oldpos.x = 1;
        oldpos.y = 1;
        test = getNextMove(wm, &pos, N, oldpos, 1, 0, RIGHT_WALL_NAIVE);
        assert(test == E);


        /* Test algorithm 1 */
        pos.x = 1;
        pos.y = 1;
        oldpos.x = 1;
        oldpos.y = 2;
        test = getNextMove(wm, &pos, W, oldpos, 1, 0, UNKNOWN_FIRST);
        assert(test == E);

        return;
}



