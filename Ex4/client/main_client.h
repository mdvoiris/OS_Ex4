//Main Client header

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef __MAIN_CLIENT_H__
#define __MAIN_CLIENT_H__

//Includes:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "HardCodedData.h"
#include "thread.h"
#include <stdbool.h> 
#pragma comment(lib, "Ws2_32.lib")

//Typedefs:

typedef struct _thread_args {
    char player_name[MAX_PLAYER_NAME];
    SOCKADDR_IN clientService;
}THREAD_ARGS;



//Global Variables:
//Syncronization barrier to sync threading
HANDLE open_event;
HANDLE close_event;
SOCKET m_socket;
int time_out;






//Function Handles:
//Prints errors according to the recieved status
Status main(int argc, char* argv[]);

#endif // __MAIN_H__