//Main module
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "main_client.h"

Status ask_to_reconnect(CLIENT_ACTION** client_action,int server_port, char* server_address)
{
	char send_str[2];
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
	Comm_status recv_res;
	char send_str[USER_ANSWER_LEN];

	printf("waiting for server approved\n"); //REMOVE
	recv_res = receive_string(&accepted_str, m_socket);
	printf("recieved string:\t %s\n", accepted_str); //REMOVE
	if (recv_res == INVALID_COMM_STATUS)
		return INVALID_STATUS_CODE;
	/*if (recv_res == COMM_FAILED)
	{
		return FAILED_RECEIVE;
		free(accepted_str);
	}*/
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
	if (split(accepted_str, MASSAGE_TYPE, &param) != SUCCESS)
		return ALLOCTION_FAILED;
	if (strcmp(param,"SERVER_GAME_RESULTS") == 0)
	{
		if(SUCCESS == split(accepted_str, PARAM_1, &param))
		    printf("Bulls: %s\n", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if(SUCCESS == split(accepted_str, PARAM_2, &param))
		   printf("Cows: %s\n", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if(SUCCESS == split(accepted_str, PARAM_3, &param))
		   printf("%s played:", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if(SUCCESS == split(accepted_str, PARAM_4, &param))
		   printf("%s\n", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		free(accepted_str);
		return SUCCESS;
	}
	else if ((strcmp(param, "SERVER_WIN")) == 0)
	{
		if (SUCCESS == split(accepted_str, PARAM_1, &param))
		   printf("%s won!\n", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		if(SUCCESS == split(accepted_str, PARAM_2, &param))
		   printf("opponents number was %s\n", param);
		else
		{
			free(accepted_str);
			return ALLOCTION_FAILED;
		}
		free(accepted_str);
		return SUCCESS;
	}
	else if ((strcmp(param, "SERVER_DRAW")) == 0)
	{
		printf("Itís a tie\n");
		free(accepted_str);
		return SUCCESS;
	}
	else if ((0 == strcmp(param, "SERVER_OPPONENT_QUIT")))
	{
		printf("Opponent quit.\n");
		free(accepted_str);
		return SUCCESS;
	}
	else if ((0 == strcmp(param, "SERVER_MAIN_MENU")))
	{
		*client_action = SEND;
		*receive_server = SERVER_MAIN_MENU;
	}
	else if ((0 == strcmp(param, "SERVER_INVITE")))
	{
		printf("Game is on!\n");
		*client_action = RECEIVE;
		return SUCCESS;
	}
	else if ((0 == strcmp(param, "SERVER_DENIED")))
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
	else if ((0 == strcmp(param, "SERVER_SETUP_REQUEST")))
	{
		*client_action = SEND;
		*receive_server = SERVER_SETUP_REQUEST;
	}
	else if ((0 == strcmp(param, "SERVER_PLAYER_MOVE_REQUEST")))
	{
		*client_action = SEND;
		*receive_server = SERVER_PLAYER_MOVE_REQUEST;
	}
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
		printf("sending client request\n"); //REMOVE
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
	char player_name[MAX_PLAYER_NAME];
	int set_socket_status;
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
	client_service.sin_addr.s_addr = server_address; //Setting the IP address to connect to
	client_service.sin_port = htons(server_port); //Setting the port to connect to.

	while (1)
	{
		printf("trying to connect\n"); //REMOVE
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
			printf("eneterd send\n"); //REMOVE
			timeout = TIMEOUT_SEND;
			if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
				return SET_SOCKET_FAILED;
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
				set_socket_status = setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int));
				if (set_socket_status == SOCKET_ERROR)
					return SET_SOCKET_FAILED;
			}
			else
			{
				timeout = TIMEOUT_RECEIVE_SHORT;
				set_socket_status = setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int));
				if (set_socket_status == SOCKET_ERROR)
					return SET_SOCKET_FAILED;
			}
			char* AcceptedStr = NULL;
			status = receive_level(&receive_server, &client_action, server_port, argv[SERVER_ADDRESS]);
			if (status != SUCCESS && status != USER_EXIT)
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
	switch (status) {
	case INVALID_STATUS_CODE: break;//there allready was a print for this case
	case FAILED_SEND:          printf("Error - Failed at send_function %ld", WSAGetLastError()); break;
	case FAILED_CREATE_SOCKET: printf("Error - Failed at socket_function %ld", WSAGetLastError()); exit(status);
	case FAILED_RECEIVE:       printf("Error - Failed at recv_function %ld", WSAGetLastError()); break;
	case FAILED_CLOSE_SOCKET:  printf("Error - Failed at close_socket_function %ld", WSAGetLastError()); WSACleanup();exit(status);
	case FAILED_CONNECT:       printf("Error - Failed at connect_function %ld", WSAGetLastError()); break;
	case ALLOCTION_FAILED:     printf("Error - Failed at malloc_function"); break;
	case SET_SOCKET_FAILED:    printf("Error - Failed at set_socket_function %ld", WSAGetLastError()); break;
	}
	closesocket(m_socket);
	WSACleanup();
	exit(status);
}
