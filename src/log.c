/*  log.c  provides functions to do logging from a separate process

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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>

// ---------------- Local includes  e.g., "file.h"
#include "log.h"

// ---------------- Constant definitions
#define MAX_TIME_STR 25

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/*====================================================================*/

void doLog(FILE *logfile, int level)
{
        int fd;
        int num;
        LogMessage msg;
        time_t curtime;
        char *timestr;
        timestr = calloc(MAX_TIME_STR, 1);
        if (timestr == NULL)
                return;

        if (logfile == NULL)
                return;

        /* make the named pipe */
        if (mkfifo(FIFO_NAME, 0666) == -1) {
                if (errno != EEXIST) {
                        perror("mkfifo");
                        return;
                }
        }

        fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1)
                fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, strerror(errno));

        /* loop and accept messages */
        do {
                num = read(fd, &msg, sizeof(msg));
                if (num == -1)
                        perror("read");
                else {
                        if (msg.level <= level) {
                                time(&curtime);
                                strftime(timestr, MAX_TIME_STR, "%F %T", localtime(&curtime));
                                fprintf(logfile, "[%s]: %s\n", timestr, msg.msg);
                        }
                }
        } while (num > 0);
        close(fd);
        unlink(FIFO_NAME);
        free(timestr);

        return;
}

