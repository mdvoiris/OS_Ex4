//Hard coded data header

#ifndef __HARD_CODED_DATA_H__
#define __HARD_CODED_DATA_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS


//Includes:
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


//Defines:
#define SERVER_ADDRESS_STR "127.0.0.1"
#define DEFAULT_TIMEOUT 15000


//Typedefs:
//All of the main functions return a status enum for debugging
typedef enum _status {
    INVALID_STATUS_CODE = -1,
    SUCCESS = 0,
    WRONG_ARGUMENT_COUNT,
    INVALID_PORT_NUMBER,
    WSASTARTUP_FAILED,
    FAILED_TO_CREATE_SOCKET,
    FAILED_TO_CONVERT_IP_ADDRESS,
    FAILED_TO_BIND_SOCKET,
    FAILED_TO_LISTEN_ON_SOCKET,
    FAILED_TO_CLOSE_SOCKET,
    FAILED_TO_ACCEPT_SOCKET,
    FAILED_TO_CREATE_EVENT,
    FAILED_TO_SET_EVENT,
    FAILED_TO_CREATE_THREAD,
    FAILED_TO_WAIT,
    QUEUE_USER_APC_FAILED,
    FOPEN_FAIL
} Status;

#endif