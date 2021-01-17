//Main module
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "main_client.h"

Status ask_to_reconnect(CLIENT_ACTION** client_action,int server_port, char* server_address)
{
	char send_str[USER_ANSWER_LEN];
	printf("Failed connecting to server on %s:%d.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
	gets_s(send_str, sizeof(send_str));
	if (strcmp(send_str, "2") == 0)
	{
		return USER_EXIT;
	}
	else
	{
		**client_action = CONNECT;
		return SUCCESS;
	}

}

Status receive_level(RECEIVE_SERVER* receive_server, CLIENT_ACTION* client_action, int server_port, char *server_address)
{
	char* accepted_str = NULL;
	char* param = NULL;
	char* message = NULL;
	Comm_status recv_res;
	char send_str[USER_ANSWER_LEN];


	recv_res = receive_string(&accepted_str, m_socket);
	if (recv_res == RECEIVE_DISCONNECTED)
	{
		free(accepted_str);
		printf("Failed connecting to server on %s:%d.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
		gets_s(send_str, sizeof(send_str));
		if (strcmp(send_str, "2") == 0)
		{
			return USER_EXIT;
		}
		else
		{
			*client_action = CONNECT;
			return SUCCESS;
		}
	}
	if (split(accepted_str, MASSAGE_TYPE, &message) != SUCCESS)
		return ALLOCTION_FAILED;
	if (strcmp(message,"SERVER_GAME_RESULTS") == 0)
	{
		if (SUCCESS == split(accepted_str, PARAM_1, &param))
		{
			printf("Bulls: %s\n", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if (SUCCESS == split(accepted_str, PARAM_2, &param))
		{
			printf("Cows: %s\n", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if (SUCCESS == split(accepted_str, PARAM_3, &param))
		{
			printf("%s played:", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if (SUCCESS == split(accepted_str, PARAM_4, &param))
		{
			printf("%s\n", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
	}
	else if ((strcmp(message, "SERVER_WIN")) == 0)
	{
		if (SUCCESS == split(accepted_str, PARAM_1, &param))
		{
			printf("%s won!\n", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if (SUCCESS == split(accepted_str, PARAM_2, &param))
		{
			printf("opponents number was %s\n", param);
			free(param);
		}
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
	}
	else if ((strcmp(message, "SERVER_DRAW")) == 0)
	{
		printf("It's a tie\n");
	}
	else if ((0 == strcmp(message, "SERVER_OPPONENT_QUIT")))
	{
		printf("Opponent quit.\n");
	}
	else if ((0 == strcmp(message, "SERVER_MAIN_MENU")))
	{
		*client_action = SEND;
		*receive_server = SERVER_MAIN_MENU;
	}
	else if ((0 == strcmp(message, "SERVER_INVITE")))
	{
		printf("Game is on!\n");
		*client_action = RECEIVE;
	}
	else if ((0 == strcmp(message, "SERVER_DENIED")))
	{
		printf("Server on %s:%d denied the connection request.\nChoose what to do next :\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
		gets_s(send_str, sizeof(send_str));
		if (0 == strcmp(send_str, "2"))
		{
			return USER_EXIT;
		}
		else
		{
			*client_action = CONNECT;
		}
	}
	else if ((0 == strcmp(message, "SERVER_SETUP_REQUEST")))
	{
		*client_action = SEND;
		*receive_server = SERVER_SETUP_REQUEST;
	}
	else if ((0 == strcmp(message, "SERVER_PLAYER_MOVE_REQUEST")))
	{
		*client_action = SEND;
		*receive_server = SERVER_PLAYER_MOVE_REQUEST;
	}
	free(message);
	free(accepted_str);
	return SUCCESS;
}


Status connect_level(SOCKADDR_IN client_service, int server_port, char* server_address, SEND_SERVER* send_server, CLIENT_ACTION* client_action)
{
	char send_str[USER_ANSWER_LEN];
	if (closesocket(m_socket) == SOCKET_ERROR)
		return FAILED_CLOSE_SOCKET;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
		return FAILED_CREATE_SOCKET;
	while (1)
	{
		if (connect(m_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
		{
			printf("Failed connecting to server on %s:%d.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", server_address, server_port);
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


Status send_level(char* player_name, SEND_SERVER* send_server, RECEIVE_SERVER receive_server, CLIENT_ACTION* client_action, int server_port, char *server_address)
{
	Comm_status send_res;
	char send_str[MAX_LEN………_SEND];
	char input_str[MAX_LEN………_SEND];


	if (*send_server == CLIENT_REQUEST)
	{
		*client_action = RECEIVE;
		sprintf_s(send_str, MAX_LEN………_SEND, "CLIENT_REQUEST:%s\n", player_name);
		*send_server = CLIENT_INVALID;
		send_res = send_string(send_str, m_socket);
		if (send_res == SEND_DISCONNECTED)
			return ask_to_reconnect(&client_action, server_port, server_address);
		return send_res;
	}
	if (receive_server == SERVER_MAIN_MENU)
	{
		printf("Choose what to do next:\n1. Play against another client\n2. Quit\n");
		gets_s(input_str, sizeof(input_str));
		*client_action = RECEIVE;
		if (strcmp(input_str, "2") == 0)
		{
			send_res = send_string("CLIENT_DISCONNECT\n", m_socket);
			return USER_QUIT;
		}
		send_res = send_string("CLIENT_VERSUS\n", m_socket);
		if (send_res == SEND_DISCONNECTED)
			return ask_to_reconnect(&client_action, server_port, server_address);
		return send_res;
	}
	if (receive_server == SERVER_SETUP_REQUEST)
	{
		printf("Choose your 4 digits:");
		gets_s(input_str, sizeof(input_str));
		*client_action = RECEIVE;
		sprintf_s(send_str, MAX_LEN………_SEND, "CLIENT_SETUP:%s\n", input_str);
		send_res = send_string(send_str, m_socket);
		if (send_res == SEND_DISCONNECTED)
			return ask_to_reconnect(&client_action, server_port, server_address);
		return send_res;
	}
	if (receive_server == SERVER_PLAYER_MOVE_REQUEST)
	{
		printf("Choose your guess:");
		gets_s(input_str, sizeof(input_str));
		*client_action = RECEIVE;
		sprintf_s(send_str, MAX_LEN………_SEND, "CLIENT_PLAYER_MOVE:%s\n", input_str);
		send_res = send_string(send_str, m_socket);
		if (send_res == SEND_DISCONNECTED)
			return ask_to_reconnect(&client_action, server_port, server_address);
		return send_res;
	}
	return SUCCESS;
}





Status main(int argc, char* argv[])
{
	char player_name[MAX_PLAYER_NAME+1];
	CLIENT_ACTION client_action = SEND;
	Status status = SUCCESS;
	SOCKADDR_IN client_service = { 0 };
	SEND_SERVER send_server = CLIENT_REQUEST;
	int iResult;
	int server_port;
	long server_address;
	RECEIVE_SERVER receive_server = INVALID_STATUS_CODE;
	char send_str[USER_ANSWER_LEN];
	int timeout = TIMEOUT_SEND;
	LINGER linger_params = { 0 };
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
	strcpy_s(player_name, MAX_PLAYER_NAME + 1, argv[PLAYER_NAME]);
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
	client_service.sin_addr.s_addr = server_address; //Setting the IP address to connect to
	client_service.sin_port = htons(server_port); //Setting the port to connect to.

	linger_params.l_onoff = 1;
	linger_params.l_linger = (DEFAULT_TIMEOUT / 1000); //in seconds
	if(setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&linger_params, sizeof(int)) == SOCKET_ERROR)
	{
		status = SET_SOCKET_FAILED;
		report_error(status);
	}

	while (1)
	{
		if (connect(m_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
		{
			printf("Failed connecting to server on %s:%d.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", argv[SERVER_ADDRESS], server_port);
			gets_s(send_str, sizeof(send_str)); //Reading a string from the keyboard
			if (STRINGS_ARE_EQUAL(send_str, "1"))
				continue;
			else if (STRINGS_ARE_EQUAL(send_str, "2"))
			{
				return SUCCESS;
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
			status = connect_level(client_service, server_port, argv[SERVER_ADDRESS], &send_server, &client_action);
			if (status != SUCCESS)
				report_error(status);
			continue;
		}
		//send block
		if (client_action == SEND)
		{
			timeout = TIMEOUT_SEND;
			if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
			{
				status = SET_SOCKET_FAILED;
				report_error(status);
			}
			status = send_level(player_name, &send_server, receive_server, &client_action, server_port, argv[SERVER_ADDRESS]);
			if (status != SUCCESS && status != USER_QUIT)
				report_error(status);
			continue;
		}
		//recv block
		if (client_action == RECEIVE)
		{
			if (send_server == CLIENT_VERSUS)
			{
				timeout = TIMEOUT_RECEIVE_LONG;
				if(setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
				{
					status = SET_SOCKET_FAILED;
					report_error(status);
				}
			}
			else
			{
				timeout = TIMEOUT_RECEIVE_SHORT;
				if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
				{
					status = SET_SOCKET_FAILED;
					report_error(status);
				}
			}
			char* AcceptedStr = NULL;
			status = receive_level(&receive_server, &client_action, server_port, argv[SERVER_ADDRESS]);
			if (status != SUCCESS && status != USER_EXIT)
				report_error(status);
			continue;
		}
	}
	status = SUCCESS;

	if (closesocket(m_socket) == SOCKET_ERROR)
	{
		status = FAILED_CLOSE_SOCKET;
		report_error(status);
	}
	WSACleanup();
	return SUCCESS;
}

void report_error(Status status) {
	switch (status) {
	case FAILED_CREATE_SOCKET: printf("Error - Failed at socket_function %ld", WSAGetLastError()); exit(status);
	case FAILED_CLOSE_SOCKET:  printf("Error - Failed at close_socket_function %ld", WSAGetLastError()); WSACleanup();exit(status);
	case ALLOCTION_FAILED:     printf("Error - Failed at malloc_function"); break;
	case SET_SOCKET_FAILED:    printf("Error - Failed at set_socket_function %ld", WSAGetLastError()); break;
	}
	if (closesocket(m_socket) == SOCKET_ERROR)
	{
		printf("Error - Failed at close_socket_function %ld", WSAGetLastError()); WSACleanup(); exit(FAILED_CLOSE_SOCKET);
	}
	WSACleanup();
	exit(status);
}
