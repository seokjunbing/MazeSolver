/*  log.h   logging datatype and functions

    Copyright 2015

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    ======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef INC_LOG_H
#define INC_LOG_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"

// ---------------- Constants
#define MSG_LEN 250
#define FIFO_NAME "logpipe.tmp"
#define LVL_FATAL 0
#define LVL_ERR   1
#define LVL_WARN  2
#define LVL_INFO  3
#define LVL_DBG   4
#define LVL_VERB  5
#define LVL_TRACE 6


// ---------------- Structures/Types
typedef struct logMessage {
        int level;
        char msg[MSG_LEN];
} LogMessage;

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/* doLog
 *      Loops, printing log messages, until all writers disconnect
 *
 * @logfile: file to output to
 * @level: current level of maze
**/
void doLog(FILE *logfile, int level);

#define LOG(msgobj, logpointer, loglevel, formatstring, ...) do { \
                memset(&(msgobj), 0, sizeof(msgobj)); \
                sprintf((msgobj).msg, (formatstring), ##__VA_ARGS__); \
                (msgobj).level = (loglevel); \
                if (write((logpointer), &(msgobj), sizeof(msgobj)) == -1) { \
                        fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
                        perror("write"); \
                        return -1; \
                }} while(0);

#endif // INC_LOG_H

