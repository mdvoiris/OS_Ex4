

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef __SHARED_COMM_H__
#define __SHARED_COMM_H__
#define MAX_LEN………_RECEIVE 55
#pragma warning( disable : 4996 )

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
	MALLOC_FAILED
}Comm_status;

typedef enum _comm_arguments {
	MASSAGE_TYPE,
	PARAM_1,
	PARAM_2,
	PARAM_3,
	PARAM_4
} COMM_ARGUMENTS;

//Function handles:
Comm_status send_buffer(const char* buffer, int bytes_to_send, SOCKET sd);

Comm_status send_string(const char* str, SOCKET sd);

Comm_status receive_string(char** out_put_str_ptr, SOCKET sd);

Comm_status receive_buffer(char* out_put_buffer, int bytes_to_receive, SOCKET sd);

void split(char str[], COMM_ARGUMENTS i, char** str_cpy);

#endif
