/*  queue.c  contains methods useful for queues

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


// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "queue.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

result_t destroyQueue(Queue *q)
{
        free(q->head);
        free(q->tail);
        return RESULT_SUCCESS;
}
//returns the address of the first elemet of the doubly linked list
// returns NULL if something went wrong
XYPos pop(Queue *q)
{
        Node *pointer = q->head->next;
        XYPos temp;

        if(q->head->next == q->tail) {
                temp.x = 99999999;
                temp.y = 99999999;
                return temp;  //returns  when the list is empty
        }
        else {
                temp = pointer->pos; //pos of the node to be popped
                q->head->next = pointer->next;
                pointer->next->prev = q->head; //link back to head
                if (pointer != q->tail)
                        free(pointer);
                return temp;
        }

        temp.x = 99999999;
        temp.y = 99999999;
        return temp;
        //returns when the list is empty;
        //something is wrong if returns here
}

int initializeQueue(Queue *q)
{
        q->head = calloc(1,sizeof(Node));
        q->tail = calloc(1,sizeof(Node));
        q->head->next = q->tail;
        q->head->prev = NULL;
        q->tail->next = NULL;
        q->tail->prev = q->head;
        return 0;
}


//adds a new node to the end of the doubly linked list
//returns 0 on completion
int push(Queue *q, XYPos input_pos)
{
        /* set to 2nd-to-last node */
        Node *pointer = q->tail->prev;

        /*Allocate memory*/
        /*head is a empty node*/
        pointer->next = q->tail->prev = (Node *)calloc(1,sizeof(Node));
        (pointer->next)->prev = pointer;
        pointer = pointer->next;
        pointer->next = q->tail;

        pointer->pos.x = input_pos.x;
        pointer->pos.y = input_pos.y;

        return 0;
}