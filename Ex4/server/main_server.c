
#include "main_server.h"


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



    CloseHandle(exit_thread_h);

    if (WSACleanup() == SOCKET_ERROR)
        printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());

    return SUCCESS;
}


Status start_socket() {
    Status status = INVALID_STATUS_CODE;
    unsigned long Address;
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
    Address = inet_addr(SERVER_ADDRESS_STR);
    if (Address == INADDR_NONE)
    {
        return FAILED_TO_CONVERT_IP_ADDRESS;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = Address;
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
    int client_count = 0;
    int index = 0;


    //Create exit event to signal threads to exit
    exit_event = CreateEvent(NULL, TRUE, FALSE, TEXT("exit_event"));
    if (exit_event == NULL) {
        return FAILED_TO_CREATE_EVENT;
    }

    //Start exit monitoring thread
    status = start_exit_thread();
    if (status) return status;

    //Client admitting loop
    while (true)
    {
        AcceptSocket = accept(MainSocket, NULL, NULL);
        if (AcceptSocket == INVALID_SOCKET)
        {
            //if exit called' return success
            if (WaitForSingleObject(exit_event, 0) == WAIT_OBJECT_0) {
                clients_cleanup();
                return SUCCESS;
            }
            //otherwise unexpeced error
            else {
                clients_cleanup();
                return FAILED_TO_ACCEPT_SOCKET;
            }
        }

        if (client_count >= NUM_OF_SLOTS) {
            status = dismiss_client(AcceptSocket);
            if (status) {
                clients_cleanup();
                return status;
            }
        }

        index = (client_thread_h[0] == NULL) ? 0 : 1;

        sockets_h[index] = AcceptSocket;

        client_thread_h[index] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            service_thread,         // thread function name
            &(sockets_h[index]),    // argument to thread function 
            0,                      // use default creation flags 
            NULL);

        if (client_thread_h[index] == NULL){
            clients_cleanup();
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
        GetCurrentThread(),     // argument to thread function 
        0,                      // use default creation flags 
        NULL);

    if (exit_thread_h == NULL) {
        return FAILED_TO_CREATE_THREAD;
    }

    return SUCCESS;
}

DWORD WINAPI monitor_exit(HANDLE main_thread_h) {
    Status status = INVALID_STATUS_CODE;
    char buffer[6] = { 0 }; //enough for "exit"

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


Status dismiss_client(AcceptSocket) {
    Status status = INVALID_STATUS_CODE;

    //TODO

    return SUCCESS;
}


void clients_cleanup() {
    Status status = INVALID_STATUS_CODE;
    DWORD return_value = 0;
    int num_of_threads = 0;


    //set exit event to the client threads
    if (!SetEvent(exit_event)) {
        report_error(FAILED_TO_SET_EVENT, false);
    }

    //wait for open client thread to exit
    if (client_thread_h[0] != NULL && client_thread_h[1] != NULL) {
        return_value = WaitForMultipleObjects(NUM_OF_SLOTS, client_thread_h, TRUE, DEFAULT_TIMEOUT);
    }
    else if (client_thread_h[0] != NULL) {
        return_value = WaitForSingleObject(client_thread_h[0], DEFAULT_TIMEOUT);
    }
    else if (client_thread_h[1] != NULL) {
        return_value = WaitForSingleObject(client_thread_h[1], DEFAULT_TIMEOUT);
    }

    if (return_value == WAIT_FAILED)
        report_error(FAILED_TO_WAIT, false);


    for (int i = 0; i < NUM_OF_SLOTS; i++) {
        if (client_thread_h[i] != NULL) {
            GetExitCodeThread(client_thread_h[i], &return_value);

            if (return_value == STILL_ACTIVE)
                TerminateThread(client_thread_h[i], -1);

            if (sockets_h[i] != INVALID_SOCKET) {
                status = (closesocket(sockets_h[i]) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
                report_error(status, false);
            }
            CloseHandle(client_thread_h[i]);
            client_thread_h[i] = NULL;
        }
    }

    CloseHandle(exit_event);
}


void report_error(Status status, bool terminate) {

    switch (status) 
    {
    case WRONG_ARGUMENT_COUNT:          printf("Error - Program called with worng argument count"); break;
    case INVALID_PORT_NUMBER:           printf("Error - Program called with invalid port number"); break;
    case WSASTARTUP_FAILED:             printf("Error - WSAStartup failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CREATE_SOCKET:       printf("Error - socket failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CONVERT_IP_ADDRESS:  printf("Error - Failed to convert \"%s\" into an ip address", SERVER_ADDRESS_STR); break;
    case FAILED_TO_BIND_SOCKET:         printf("Error - bind failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_LISTEN_ON_SOCKET:    printf("Error - listen failed with %ld", WSAGetLastError()); break;
    case FAILED_TO_CLOSE_SOCKET:        printf("Error - Failed to close socket with %ld", WSAGetLastError()); break;
    case FAILED_TO_ACCEPT_SOCKET:       printf("Error - Failed to accept socket with %ld", WSAGetLastError()); break;
    case FAILED_TO_CREATE_EVENT:        printf("Error - Failed to create event with %ld", WSAGetLastError()); break;
    case FAILED_TO_WAIT:                printf("Error - Failed at a wait function with %d", GetLastError()); break;
    case QUEUE_USER_APC_FAILED:         printf("Error - Failed at QueueUserAPC with %d", GetLastError()); break;
    case FOPEN_FAIL:                    printf("Error - Failed Opening the input file"); break;
    //case ALLOC_FAILED:                  printf("Error - Allocation failed"); break;
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