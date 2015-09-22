/* 	queue.h implements a queue 

    Copyright 2015

    License (if any)

    Project name: Maze
    Component name: Queue

    This file contains ...

    Primary Author: N/A
    Date Created: 03/11/2015

    ======================================================================*/

#ifndef INC_QUEUE_H
#define INC_QUEUE_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables


// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "common.h"


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types


typedef struct Node {
    XYPos pos;                           // the data for a given page
    struct Node *next;                   // pointer to next node
    struct Node *prev;                   // pointer to previous node
} Node;

typedef struct Queue {
    Node *head;                          // "beginning" of the queue
    Node *tail;                          // "end" of the queue
} Queue;



// ---------------- Prototypes/Macros
/** initializeQueue
*       Initialize new Queue
*       @q- queue to initialize
**/
int initializeQueue(Queue *q);

/** push
*       Add item to queue
*       @q- Queue to add to
*       @input_pos- XYPos to add 
**/
int push(Queue *q, XYPos input_pos);

/** pop
*       Pop off item from queue
*       @q- Queue to pop
**/
XYPos pop(Queue *q);

/**destroyQueue
*      Destroys queue from memory. Returns result_t for success/failure check
*      @q- queue to destroy
**/
result_t destroyQueue(Queue *q);

#endif // INC_QUEUE_H

