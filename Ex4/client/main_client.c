//Main module
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "main_client.h"

Status main(int argc, char* argv[])
{
	HANDLE hThread;
	int server_port;
	THREAD_ARGS thread_args;
	int iResult;
	DWORD event_result;
	unsigned long server_address;
	WSADATA wsaData; //Create a WSADATA object called wsaData.
//The WSADATA structure contains information about the Windows Sockets implementation.

	if (argc != ARG_COUNT) {
		printf("Error - got %d arguments, expecting %d", argc, ARG_COUNT);
		return WRONG_ARGUMENT_COUNT;
	}
	server_port = atoi(argv[SERVER_PORT]);
	// if we want to give the player another chance need to add here while(1) loop
	if (strlen(argv[2]) != SERVER_PORT_LENTH)
	{
		printf("Port server number is illegal try again");
		return ILLEGAL_PORT;
	}

	strcpy_s(thread_args.player_name, MAX_PLAYER_NAME, argv[PLAYER_NAME]);
	server_address = inet_addr(argv[SERVER_ADDRESS]);
	if (server_address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			argv[SERVER_ADDRESS]);
		return ILLEGAL_SERVER;
	}
	// Initialize Winsock.

	//Call WSAStartup and check for errors.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("Error at WSAStartup()\n");
		return WSASTARTUP_FAILED;
	}
	    

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return FAILED_CREATE_SOCKET;
	}
	/*
	 The parameters passed to the socket function can be changed for different implementations.
	 Error detection is a key part of successful networking code.
	 If the socket call fails, it returns INVALID_SOCKET.
	 The if statement in the previous code is used to catch any errors that may have occurred while creating
	 the socket. WSAGetLastError returns an error number associated with the last error that occurred.
	 */


	 //For a client to communicate on a network, it must connect to a server.
	 // Connect to a server.

	 //Create a sockaddr_in object clientService and set  values.
	thread_args.clientService.sin_family = AF_INET;
	thread_args.clientService.sin_addr.s_addr =server_address; //Setting the IP address to connect to
	thread_args.clientService.sin_port = htons(server_port); //Setting the port to connect to.


	/*
		AF_INET is the Internet address family.
	*/
	close_event = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("close_event")  // object name
	);

	open_event = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("open_event")  // object name
	);

	if (open_event == NULL || close_event == NULL)
	{
		printf("fail to create an Event %d", GetLastError());
		return FAILED_EVENT;
	}


	hThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)communication_thread,
			&thread_args,
			0,
			NULL
		);
	while (1)//main thread as a timer for the communication thread
	{

		event_result = WaitForSingleObject(
			open_event, // event handle
			time_out);    // time wait is dependent on the scenario in communication threa




			// An error occurred
		if (event_result != WAIT_OBJECT_0)
		{
			printf("Wait error (%d)\n", GetLastError());
			return 0;
		}


		event_result = WaitForSingleObject(
			close_event, // event handle
			time_out);    // time wait is dependent on the scenario in communication threa


		//if()
	}
}
