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


//Typedefs:
typedef struct _client_args {
	SOCKET socket;
	HANDLE file_mutex;
	HANDLE opponent_event;
	HANDLE opponent_disconnect_event;
}Client_args;


//Function Handles:
Status start_socket();

Status start_exit_thread();

DWORD WINAPI monitor_exit(HANDLE main_thread_h);

Status admit_clients();

Status dismiss_client(AcceptSocket);

void clients_cleanup(Client_args client_args);

//Prints errors according to the recieved status
void report_error(Status status, bool terminate);

#endif // __MAIN_SERVER_H__
