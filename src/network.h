/*  network.h  functions for performing common networking functionality

    Copyright 2015

    License (if any)

    Project name:
    Component name:

    This file contains ...

    Primary Author:
    Date Created:

    ======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef INC_NETWORK_H
#define INC_NETWORK_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <sys/types.h>
#include <sys/socket.h> /* socket */
#include <netdb.h>
#include <netinet/in.h> /* htonl ntohl */
#include <inttypes.h> /* SCNu32 macro */
#include "amazing.h"


// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/** sendAMMessage
*		Convert an AM_Message from little to big endian and send it
*		@s- socket to send to
*		@m- AM_Message to send
**/
void sendAMMessage(int s, AM_Message m);

/** receiveAMMessage
*		Receive an AM_Message from socket
*		@s- socket to get from
**/
AM_Message receiveAMMessage(int s);

/** connectedToServer
*		Connect to given hostname
*		@hostname- name of host
*		@port- port number
**/
int connectToServer(char hostname[], uint32_t port);

/** printAMError
*		Print correct error message based on given
*		@fd- file descriptor to print to
*		@errcode- error code
**/
void printAMError(int fd, uint32_t errcode);

/** printAMInitError
*		Print errors upon initialization
*		@errcode- error code
**/
void printAMInitError(uint32_t errcode);

#endif // INC_NETWORK_H

