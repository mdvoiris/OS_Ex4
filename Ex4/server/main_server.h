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
//struct containing arguments rent to client threads
typedef struct _client_args {
	SOCKET socket;
	HANDLE file_mutex;
	HANDLE opponent_event;
	HANDLE opponent_disconnect_event;
	HANDLE exit_event;
	const char* file_name;
	bool* file_exists_p;
}Client_args;


//Function Handles:
//initiates main socket
Status start_socket();

//creates the exit thread which monitors stdin for the word exit
Status start_exit_thread();

//thread function of the exit thread
//waits for stdin input of exit, if happend triggers exit event and closes MainSocket
DWORD WINAPI monitor_exit(LPVOID lpParam);

//client managment function
//creats events and mutex for thread communication
//starts the exit thread and waits in an infinite loop for accepted sockets
//allows only 2 clients in and creates communication threads for them
Status admit_clients();

//if 2 clients already in, gets client requests
//denies them and closes the accepted socket
Status dismiss_client(SOCKET socket);

//sets exit event to signal client threads and waits for them to exit
//terminates if threads didn't exit after reaching timeout
//closes event and mutex handles
void clients_cleanup(Client_args client_args);

//Prints errors according to the recieved status
//terminates all if terminate is true
void report_error(Status status, bool terminate);

#endif // __MAIN_SERVER_H__
