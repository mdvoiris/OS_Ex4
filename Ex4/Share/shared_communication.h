

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef __SHARED_COMM_H__
#define __SHARED_COMM_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h> 
#pragma comment(lib, "Ws2_32.lib")


//Typedefs:
typedef enum _comm_status {
	INVALID_COMM_STATUS = -1,
	COMM_SUCCESS,
	COMM_FAILED,
	RECEIVE_DISCONNECTED,
	TIMEOUT_RECEIVE
}Comm_status;


//Function handles:
Comm_status send_buffer(const char* Buffer, int BytesToSend, SOCKET sd);

Comm_status send_string(const char* Str, SOCKET sd);

#endif
