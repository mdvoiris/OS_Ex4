//Main server header

#ifndef __MAIN_SERVER_H__
#define __MAIN_SERVER_H__


//Includes:
#include "shared_communication.h"
#include "HardCodedData.h"
#include "service_thread.h"


//Defines:
#define NUM_OF_ARGS 2
#define NUM_OF_SLOTS 2



//Global variebles:
HANDLE exit_thread_h = NULL;
HANDLE client_thread_h[NUM_OF_SLOTS] = { NULL };
SOCKET MainSocket = INVALID_SOCKET;
SOCKET sockets_h[NUM_OF_SLOTS] = { INVALID_SOCKET, INVALID_SOCKET };
int server_port = 0;
HANDLE exit_event = NULL;



//Function Handles:

Status start_socket();

Status start_exit_thread();

DWORD WINAPI monitor_exit(HANDLE main_thread_h);

Status admit_clients();

Status dismiss_client(AcceptSocket);

void clients_cleanup();

//Prints errors according to the recieved status
void report_error(Status status, bool terminate);

#endif // __MAIN_SERVER_H__
