//Main module
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "main_client.h"
Status receive_level(RECEIVE_SERVER receive_server, CLIENT_ACTION* client_action, int server_port, long server_address)
{
	char* AcceptedStr = NULL;
	Comm_status recv_res;
	char send_str[MAX_LEN………_RECEIVE];
	recv_res = receive_string(&AcceptedStr, m_socket);
	if (recv_res == INVALID_COMM_STATUS)
		return INVALID_STATUS_CODE;
	if (recv_res == COMM_FAILED)
	{
		return FAILED_RECEIVE;
	}
	else if (recv_res == RECEIVE_DISCONNECTED)
	{
		printf("Failed connecting to server on <ip>:<port>.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
		closesocket(m_socket);
		gets_s(send_str, sizeof(send_str));
		if (!strcmp(send_str, "2"))
		{
			return USER_EXIT;
		}
		else
		{
			client_action = CONNECT;
			return SUCCESS;
		}

	}
	else
	{
		printf("%s\n", AcceptedStr);
	}

	free(AcceptedStr);
}


Status connect_level(SOCKADDR_IN client_service, int server_port, long server_address, SEND_SERVER* send_server, CLIENT_ACTION* client_action)
{
	char send_str[1];
	while (1)
	{
		if (connect(m_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
		{
			printf("Failed connecting to server on %ld:%d.\nChoose what to do next:\n1. Try to reconnect\2. Exit", server_address, server_port);
			gets_s(send_str, sizeof(send_str)); //Reading a string from the keyboard
			if (STRINGS_ARE_EQUAL(send_str, "1"))
				continue;
			else if (STRINGS_ARE_EQUAL(send_str, "2"))
			{
				return USER_EXIT;
			}
		}
		else
			break;
	}
	*send_server = CLIENT_REQUEST;
	*client_action = SEND;
	return SUCCESS;
}


Status send_level(char* player_name, SEND_SERVER* send_server, RECEIVE_SERVER receive_server, CLIENT_ACTION* client_action, int server_port, long server_address)
{
	char send_str[MAX_LEN………_SEND];
	if (*send_server == CLIENT_REQUEST)
	{
		*client_action = RECEIVE;
		return send_string(player_name, m_socket);
	}
	if (receive_server == SERVER_MAIN_MENU)
	{
		printf("Choose what to do next:\n1. Play against another client\n2. Quit\n");
		gets_s(send_str, sizeof(send_str));
		*client_action = RECEIVE;
		if (!strcmp(send_str, "2"))
		{
			send_string("CLIENT……_DISCONNECT\n", m_socket);
			return USER_QUIT;
		}
		*send_server == CLIENT_VERSUS;
		return send_string("CLIENT……_VERSUS\n", m_socket);
	}
	if (receive_server == SERVER_INVITE)// moove to receive block
	{
		printf("Game is on!\n");
		*client_action = RECEIVE;
		return SUCCESS;
	}
	if (receive_server == SERVER_DENIED)
	{
		closesocket(m_socket);
		printf("Server on %ld : %d denied the connection request.\nChoose what to do next :\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
		gets_s(send_str, sizeof(send_str));
		if (!strcmp(send_str,"2"))
		{
			return USER_EXIT;
		}
		else
		{
			client_action = CONNECT;
			return SUCCESS;
		}
	}
	if (receive_server == SERVER_SETUP_REQUEST)
	{
		printf("Choose your 4 digits:");
		gets_s(send_str, sizeof(send_str));
		*client_action = RECEIVE;
		return send_string(send_str, m_socket);
	}
	if (receive_server == SERVER_SETUP_REQUEST)
	{
		printf("Choose your 4 digits:");
		gets_s(send_str, sizeof(send_str));
		*client_action = RECEIVE;
		return send_string(sprintf_s(send_str, MAX_LEN………_SEND, "CLIENT_SETUP:%s\n", send_str), m_socket);
	}
	if (receive_server == SERVER_PLAYER_MOVE_REQUEST)
	{
		printf("Choose your guess:");
		gets_s(send_str, sizeof(send_str));
		*client_action = RECEIVE;
		return send_string(sprintf_s(send_str, MAX_LEN………_SEND, "CLIENT_PLAYER_MOVE:%s\n", send_str), m_socket);
	}
}











Status main(int argc, char* argv[])
{
	char player_name[20];
	CLIENT_ACTION client_action = SEND;
	Status status = SUCCESS;
	SOCKADDR_IN client_service;
	SEND_SERVER send_server = CLIENT_REQUEST;
	int iResult;
	DWORD event_result;
	int server_port;
	long server_address;
	RECEIVE_SERVER receive_server;
	char send_str[1];
	WSADATA wsaData; //Create a WSADATA object called wsaData.
//The WSADATA structure contains information about the Windows Sockets implementation.

	if (argc != ARG_COUNT) {
		printf("Error - got %d arguments, expecting %d", argc, ARG_COUNT);
		return WRONG_ARGUMENT_COUNT;
	}
	server_port = atoi(argv[SERVER_PORT]);
	// if we want to give the player another chance need to add here while(1) loop
	if (strlen(argv[2]) != SERVER_PORT_LENGTH)
	{
		printf("Port server number is illegal try again");
		return ILLEGAL_PORT;
	}

	strcpy_s(player_name, MAX_PLAYER_NAME, argv[PLAYER_NAME]);
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

	client_service.sin_family = AF_INET;
	client_service.sin_addr.s_addr = server_port; //Setting the IP address to connect to
	client_service.sin_port = htons(server_port); //Setting the port to connect to.

	while (1)
	{
		if (connect(m_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
		{
			printf("Failed connecting to server on %ld:%d.\nChoose what to do next:\n1. Try to reconnect\2. Exit", server_address, server_port);
			gets_s(send_str, sizeof(send_str)); //Reading a string from the keyboard
			if (STRINGS_ARE_EQUAL(send_str, "1"))
				continue;
			else if (STRINGS_ARE_EQUAL(send_str, "2"))
			{
				report_error(FAILED_CONNECT);
			}
		}
		else
			break;
	}

	while (status!=USER_QUIT && status!=USER_EXIT)
	{
		//connect block
		if (client_action == CONNECT)
		{
			status = connect_level(client_service, server_port, server_address, &send_server, &client_action);
			if (status != SUCCESS && status != USER_QUIT && status != USER_EXIT)
				report_error(status);
			continue;
		}
		//send block
		if (client_action == SEND)
		{
			setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, TIMEOUT_SEND, sizeof(int));
			status = send_level(player_name, &send_server, receive_server, &client_action, server_port, server_address);
			if (status != SUCCESS)
				report_error(status);
			continue;
		}
		//recv block
		if (client_action == RECEIVE)
		{
			if (send_server == CLIENT_VERSUS)
				setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, TIMEOUT_RECEIVE_LONG, sizeof(int));
			else
				setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, TIMEOUT_RECEIVE_SHORT, sizeof(int));
			char* AcceptedStr = NULL;
			status = receive_level(&receive_server, &client_action, server_port, server_address);
			if (status != SUCCESS)
				report_error(status);
			continue;
		}
	}
	status = SUCCESS;

	closesocket(m_socket);
	WSACleanup();
	return SUCCESS;
}

void report_error(Status status) {
	closesocket(m_socket);
	WSACleanup();
	switch (status) {
	case INVALID_STATUS_CODE: break;//there allready was a print for this case
	case FAILED_SEND:         printf("Error - Failed at send_function %ld", WSAGetLastError()); break;
	}
	exit(status);
}
