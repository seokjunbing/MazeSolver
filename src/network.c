/*  network.c  functions for dealing with the network

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
#include <string.h> /* memset */
#include <sys/types.h>
#include <sys/socket.h> /* socket */
#include <netdb.h>
#include <netinet/in.h> /* htonl ntohl */
#include <inttypes.h> /* SCNu32 macro */
#include <math.h> /* log10 */


// ---------------- Local includes  e.g., "file.h"

#include "network.h"
#include "amazing.h"
#include "log.h"
// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/*====================================================================*/

int connectToServer(char hostname[], uint32_t port)
{
        int status;
        int s; /* socket to server */
        struct addrinfo hints;
        struct addrinfo *servinfo = NULL; /* results of getaddrinfo */
        char *cport = malloc((int)log10(port) + 2);
        if (cport == NULL) {
                fprintf(stderr, "%s:%d: malloc failed. Exiting...\n", __FILE__, __LINE__);
                return -1;
        }
        sprintf(cport, "%" PRIu32, port);

        /* networking code followed tutorial at
         * http://www.beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo
         */
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC; /* Don't care about IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* TCP stream */
        hints.ai_protocol = 0; /* any */

        status = getaddrinfo(hostname, cport, &hints, &servinfo);
        if (status != 0 || servinfo == NULL)
        {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
                s = -1;
                goto cleanup;
        }
        printf("Verified address format...\n");

        s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
        if (s < 0) {
                fprintf(stderr, "Failed to open socket to server\n");
                perror("socket");
                s = -1;
                goto cleanup;
        }
        printf("Successfully opened socket...\n");

        status = connect(s, servinfo->ai_addr, servinfo->ai_addrlen);
        if (status != 0) {
                printf("Failed to connect to server\n");
                perror("connect");
                s = -1;
                goto cleanup;
        }
        printf("Made connection to server...\n");

cleanup:
        if (servinfo != NULL)
                freeaddrinfo(servinfo);
        free(cport);
        return s;
}

/* Does all the converting from little to big endian for the message */
void sendAMMessage(int s, AM_Message m)
{
        int i;
        uint32_t *message;

        message = (uint32_t *)&m;
        for (i = 0; i < (sizeof(AM_Message)/sizeof(uint32_t)); i++) {
                message[i] = htonl(message[i]);
        }
        send(s, &m, sizeof(AM_Message), 0);
        /* Handle failure gracefully */

        return;
}

/* Does all the converting from big to little endian for the message */
AM_Message receiveAMMessage(int s)
{
        int i;
        AM_Message m;
        uint32_t *message;

        memset(&m, 0, sizeof m);
        recv(s, &m, sizeof(m), 0);
        message = (uint32_t *)&m;
        for (i = 0; i < (sizeof(AM_Message)/sizeof(uint32_t)); i++) {
                message[i] = ntohl(message[i]);
        }

        return m;
}

void printAMError(int fd, uint32_t errcode)
{
        LogMessage lmsg;
        memset(&lmsg, 0, sizeof(lmsg));
        lmsg.level = LVL_ERR;
        if (!IS_AM_ERROR(errcode)) {
                sprintf(lmsg.msg, "Not an error");
        }
        if (errcode & AM_AVATAR_OUT_OF_TURN)
                sprintf(lmsg.msg, "Not this avatar's turn");
        if (errcode & AM_NO_SUCH_AVATAR)
                sprintf(lmsg.msg, "No such avatar");
        if (errcode & AM_TOO_MANY_MOVES)
                sprintf(lmsg.msg, "Too many moves");
        if (errcode & AM_SERVER_DISK_QUOTA)
                sprintf(lmsg.msg, "Server out of disk space");
        if (errcode & AM_SERVER_OUT_OF_MEM)
                sprintf(lmsg.msg, "Server out of memory");
        if (errcode & AM_UNKNOWN_MSG_TYPE)
                sprintf(lmsg.msg, "Unknown message type");
        if (errcode & AM_SERVER_TIMEOUT)
                sprintf(lmsg.msg, "Server timeout");
        if (errcode & AM_UNEXPECTED_MSG_TYPE)
                sprintf(lmsg.msg, "Unexpected message type");
        if (errcode & AM_INIT_FAILED)
                sprintf(lmsg.msg, "Init failed");
        if (write(fd, &lmsg, sizeof(lmsg)) == -1) {
                fprintf(stderr, "Failed to log error message:\n%s\n", lmsg.msg);
                perror("write");
                return;
        }
        return;
}

void printAMInitError(uint32_t errcode)
{
        if (errcode & AM_INIT_ERROR_MASK)
                printf("General init error\n");
        if (errcode & AM_INIT_TOO_MANY_AVATARS)
                printf("Too many avatars\n");
        if (errcode & AM_INIT_BAD_DIFFICULTY)
                printf("Bad difficulty\n");
        printf("0x%X\n", errcode);
}

