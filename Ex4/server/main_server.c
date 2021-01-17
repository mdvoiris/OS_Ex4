
#include "main_server.h"


//Global veriables:
HANDLE exit_thread_h = NULL;
HANDLE client_thread_h[NUM_OF_SLOTS] = { NULL };
SOCKET MainSocket = INVALID_SOCKET;
SOCKET sockets_h[NUM_OF_SLOTS] = { INVALID_SOCKET, INVALID_SOCKET };
int server_port = 0;
HANDLE exit_event = NULL;


Status main(int argc, char* argv[]) 
{
	Status status = INVALID_STATUS_CODE;
    WSADATA wsaData;
    int StartupRes;


    //Check for correct input argument count
    if (argc != NUM_OF_ARGS) {
        report_error(WRONG_ARGUMENT_COUNT, true);
    }

    //Get server port number
    server_port = atoi(argv[1]);
    if (server_port < 1000 || server_port > 9999) {
        report_error(INVALID_PORT_NUMBER, true);
    }

    //Initialize Winsock
    StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (StartupRes != NO_ERROR) {
        report_error(WSASTARTUP_FAILED, true);
    }

    //Start main socket
    status = start_socket();
    if (status) {
        report_error(status, false);
        status = (closesocket(MainSocket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, true);
    }

    //Admit clients
    status = admit_clients();
    if (status) {
        report_error(status, false);
        status = (closesocket(MainSocket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, true);
    }

    CloseHandle(exit_event);
    CloseHandle(exit_thread_h);

    if (WSACleanup() == SOCKET_ERROR)
        printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());

    return SUCCESS;
}


Status start_socket() {
    Status status = INVALID_STATUS_CODE;
    unsigned long address;
    SOCKADDR_IN service;
    int bindRes;
    int ListenRes;


    // Create a socket.    
    MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (MainSocket == INVALID_SOCKET)
    {
        return FAILED_TO_CREATE_SOCKET;
    }

    // Create a sockaddr_in object and set its values.
    //address = inet_addr(SERVER_ADDRESS);
    if (InetPton(AF_INET, TEXT(SERVER_ADDRESS), &address) != 1)
    {
        return FAILED_TO_CONVERT_IP_ADDRESS;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = address;
    service.sin_port = htons(server_port);

    // Bind the socket.
    bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
    if (bindRes == SOCKET_ERROR)
    {
        return FAILED_TO_BIND_SOCKET;
    }

    // Listen on the Socket.
    ListenRes = listen(MainSocket, SOMAXCONN);
    if (ListenRes == SOCKET_ERROR)
    {
        return FAILED_TO_LISTEN_ON_SOCKET;
    }

    return SUCCESS;
}


Status admit_clients() {
    Status status = INVALID_STATUS_CODE;
    SOCKET AcceptSocket = INVALID_SOCKET;
    int index = 0;
    Client_args client_args = { 0 };
    LINGER linger_params = { 0 };
    const char file_name[] = "GameSession.txt";
    bool file_exists = false;


    //Create exit event to signal threads to exit
    exit_event = CreateEvent(NULL, TRUE, FALSE, TEXT("exit_event"));
    if (exit_event == NULL) {
        return FAILED_TO_CREATE_EVENT;
    }

    //Start exit monitoring thread
    status = start_exit_thread();
    if (status) return status;

    //Create opponent event to signal opponent moves
    client_args.opponent_event = CreateEvent(NULL, TRUE, FALSE, TEXT("opponent_event"));
    if (client_args.opponent_event == NULL) {
        return FAILED_TO_CREATE_EVENT;
    }

    //Create opponent disconnect event to signal unexpected oppnent disconnect
    client_args.opponent_disconnect_event = CreateEvent(NULL, TRUE, FALSE, TEXT("opponent_disconnect_event"));
    if (client_args.opponent_disconnect_event == NULL) {
        return FAILED_TO_CREATE_EVENT;
    }

    //Create file mutex for accessing in game session
    client_args.file_mutex = CreateMutex(NULL, FALSE, NULL);
    if (client_args.file_mutex == NULL) {
        return FAILED_TO_CREATE_MUTEX;
    }

    client_args.exit_event = exit_event;
    client_args.file_name = &file_name;
    client_args.file_exists_p = &file_exists;

    //Client admitting loop
    while (true)
    {
        AcceptSocket = accept(MainSocket, NULL, NULL);
        if (AcceptSocket == INVALID_SOCKET)
        {
            //if exit called, return success
            if (WaitForSingleObject(exit_event, 0) == WAIT_OBJECT_0) {
                clients_cleanup(client_args);
                return SUCCESS;
            }
            //otherwise unexpected error
            else {
                clients_cleanup(client_args);
                return FAILED_TO_ACCEPT_SOCKET;
            }
        }

        //configure socket to linger for queued data to be sent before closing socket
        linger_params.l_onoff = 1;
        linger_params.l_linger = (DEFAULT_TIMEOUT/1000); //in seconds
        if (setsockopt(AcceptSocket, SOL_SOCKET, SO_LINGER, (char*)&linger_params, sizeof(int)) == SOCKET_ERROR)
            return FAILED_TO_SET_SOCKET_OPT;
        
        //if not first
        if (client_thread_h[0] != NULL) {
            //if second
            if (client_thread_h[1] == NULL) {
                index = 1;
            }
            //after that
            else {
                //if thread 0 finished index = 0; else index = -1 
                index = (WaitForSingleObject(client_thread_h[0], 0) == WAIT_OBJECT_0) ? 0 : -1 ;
                if (index == -1)
                    //else if thread 1 finished index = 1; else index = -1 
                    index = (WaitForSingleObject(client_thread_h[1], 0) == WAIT_OBJECT_0) ? 1 : -1;
                if (index == -1) {
                    //else dissmiss client, serving 2 clients
                    status = dismiss_client(AcceptSocket);
                    if (status) {
                        clients_cleanup(client_args);
                        return status;
                    }
                    continue;
                }
            }
        }

        sockets_h[index] = AcceptSocket;

        client_args.socket = AcceptSocket;
        ResetEvent(client_args.opponent_event);
        ResetEvent(client_args.opponent_disconnect_event);

        client_thread_h[index] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            service_thread,         // thread function name
            &(client_args),         // argument to thread function 
            0,                      // use default creation flags 
            NULL);

        if (client_thread_h[index] == NULL){
            clients_cleanup(client_args);
            return FAILED_TO_CREATE_THREAD;
        }
    }



    return SUCCESS;
}


Status start_exit_thread() {
    Status status = INVALID_STATUS_CODE;

    exit_thread_h = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        monitor_exit,           // thread function name
        NULL,     // argument to thread function 
        0,                      // use default creation flags 
        NULL);

    if (exit_thread_h == NULL) {
        return FAILED_TO_CREATE_THREAD;
    }

    return SUCCESS;
}

DWORD WINAPI monitor_exit(LPVOID lpParam) {
    Status status = INVALID_STATUS_CODE;
    char buffer[6] = { 0 }; //enough for "exit\n"

    while (true)
    {
        fgets(buffer, 6, stdin);
        if (strcmp(buffer, "exit\n") == 0) {
            //set exit event
            if (!SetEvent(exit_event)) {
                report_error(FAILED_TO_SET_EVENT, false);
            }
            //close MainSocket
            status = (closesocket(MainSocket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
            report_error(status, false);
            return SUCCESS;
        }
    }
}


Status dismiss_client(SOCKET socket) {
    Status status = INVALID_STATUS_CODE;
    Comm_status comm_status = INVALID_COMM_STATUS;
    char* recieved = NULL;

    //wait for CLIENT_REQUEST
    comm_status = receive_string(&recieved, socket);
    if (comm_status) {
        return FAILED_TO_RECIEVE_STRING;
    }
    //send SERVER_DENIED
    comm_status = send_string("SERVER_DENIED\n", socket);
    if (comm_status) {
        return FAILED_TO_SEND_STRING;
    }

    //close socket
    status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
    report_error(status, false);

    return SUCCESS;
}


void clients_cleanup(Client_args client_args) {
    Status status = INVALID_STATUS_CODE;
    DWORD return_value[NUM_OF_SLOTS] = { NULL };
    int num_of_threads = 0;


    //set exit event to the client threads
    if (!SetEvent(exit_event)) {
        report_error(FAILED_TO_SET_EVENT, false);
    }

    //findout what clients are active
    for (int i = 0; i < NUM_OF_SLOTS; i++) {
        if (client_thread_h[i] != NULL) {
            GetExitCodeThread(client_thread_h[i], &(return_value[i]));
        }
    }

    //close active client sockets and wait for threads to exit
    //if both clients active
    if (return_value[0] == STILL_ACTIVE && return_value[1] == STILL_ACTIVE) {
        status = (closesocket(sockets_h[0]) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, false);
        status = (closesocket(sockets_h[1]) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, false);
        if (WAIT_FAILED == WaitForMultipleObjects(NUM_OF_SLOTS, client_thread_h, TRUE, DEFAULT_TIMEOUT))
            report_error(FAILED_TO_WAIT, false);
    }
    //if only client 0 active
    else if (return_value[0] == STILL_ACTIVE) {
        status = (closesocket(sockets_h[0]) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, false);
        if (WAIT_FAILED == WaitForSingleObject(client_thread_h[0], DEFAULT_TIMEOUT))
            report_error(FAILED_TO_WAIT, false);
    }
    //if only client 1 active
    else if (return_value[1] == STILL_ACTIVE) {
        status = (closesocket(sockets_h[1]) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
        report_error(status, false);
        if (WAIT_FAILED == WaitForSingleObject(client_thread_h[1], DEFAULT_TIMEOUT))
            report_error(FAILED_TO_WAIT, false);
    }

    //make sure threads exited and close handles
    for (int i = 0; i < NUM_OF_SLOTS; i++) {
        if (client_thread_h[i] != NULL) {
            GetExitCodeThread(client_thread_h[i], &(return_value[i]));

            if (return_value[i] == STILL_ACTIVE) {
                TerminateThread(client_thread_h[i], -1);
            }

            CloseHandle(client_thread_h[i]);
            client_thread_h[i] = NULL;
        }
    }

    CloseHandle(client_args.file_mutex);
    CloseHandle(client_args.opponent_event);
    CloseHandle(client_args.opponent_disconnect_event);
}


void report_error(Status status, bool terminate) {

    switch (status) 
    {
    case WRONG_ARGUMENT_COUNT:          printf("Error - Program called with worng argument count"); break;
    case INVALID_PORT_NUMBER:           printf("Error - Program called with invalid port number"); break;
    case WSASTARTUP_FAILED:             printf("Error - WSAStartup failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CREATE_SOCKET:       printf("Error - socket failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CONVERT_IP_ADDRESS:  printf("Error - Failed to convert \"%s\" into an ip address", SERVER_ADDRESS); break;
    case FAILED_TO_BIND_SOCKET:         printf("Error - bind failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_LISTEN_ON_SOCKET:    printf("Error - listen failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CLOSE_SOCKET:        printf("Error - Failed to close socket with %ld", WSAGetLastError()); break;
    case FAILED_TO_ACCEPT_SOCKET:       printf("Error - Failed to accept socket with %ld", WSAGetLastError()); break;
    case FAILED_TO_SET_SOCKET_OPT:      printf("Error - Failed to set socket options with %ld", WSAGetLastError()); break;
    case FAILED_TO_CREATE_EVENT:        printf("Error - Failed to create event with %ld", WSAGetLastError()); break;
    case FAILED_TO_SEND_STRING:         printf("Error - Failed to send string with %ld", WSAGetLastError()); break;
    case FAILED_TO_RECIEVE_STRING:      printf("Error - Failed to recieve string with %ld", WSAGetLastError()); break;
    case FAILED_TO_WAIT:                printf("Error - Failed at a wait function with %d", GetLastError()); break;
    case QUEUE_USER_APC_FAILED:         printf("Error - Failed at QueueUserAPC with %d", GetLastError()); break;
    case FAILED_TO_CREATE_MUTEX:        printf("Error - Failed at CreateMutex with %d", GetLastError()); break;
    case UNRELEASED_MUTEX:              printf("Error - Didn't manage to get mutex with %d", GetLastError()); break;
    case FOPEN_FAIL:                    printf("Error - Failed opening a file"); break;
    case FAILED_TO_REMOVE_FILE:         printf("Error - Failed to remove file"); break;
    case ALLOC_FAILED:                  printf("Error - Allocation failed"); break;
    case FAILED_TO_CREATE_THREAD:       printf("Error - Failed at CreateThread()"); break;
    }


    if (terminate) {
        if (exit_thread_h != NULL)
            TerminateThread(exit_thread_h, -1);
        if (WSACleanup() == SOCKET_ERROR)
            printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
        exit(status);
    }
}