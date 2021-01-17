

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
	SEND_DISCONNECTED,
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
/*SendBuffer() is a wrapper for send() function that helps to send the complete message through the network.
*
* Accepts:
*------ -
*buffer - the buffer containing the data to be sent.
* bytes_to_send - the number of bytes from the Buffer to send.
* sd - the socket used for communication.
*
* Returns:
status enum for debuging*/
Comm_status send_buffer(const char* buffer, int bytes_to_send, SOCKET sd);
/*SendString() uses a socket to send a string.
* Str - the string to send.
* sd - the socket used for communication.
* * Returns:
status enum for debuging*/
Comm_status send_string(const char* str, SOCKET sd);
/*receive_string() uses a socket to receive a string, and stores it in dynamic memory.
*
* Accepts:
*------ -
*out_put_str_ptr - a pointer to a char - pointer that is initialized to NULL, as in :
*
* sd - the socket used for communication.
*
* Returns:
status enum for debuging*/
Comm_status receive_string(char** out_put_str_ptr, SOCKET sd);
//receive_buffer() is a wrapper for recv() function that helps to receive the complete message from the network
/*Accepts:
*------ -
*receive_buffer() uses a socket to receive a buffer.
* OutputBuffer - pointer to a buffer into which data will be written
* OutputBufferSize - size in bytes of Output Buffer
* BytesReceivedPtr - output parameter. if function returns TRNS_SUCCEEDED, then this
* will point at an int containing the number of bytes received.
* sd - the socket used for communication.
*
* Returns:
status enum for debuging*/
Comm_status receive_buffer(char* out_put_buffer, int bytes_to_receive, SOCKET sd);

//slit() accepts the message from the ntwork and uptes the parameter from the message in the param pointer.
//comm_argumen- the index of the wanted parameter in the message
//Returns status enum for debuging
Comm_status split(char* message, COMM_ARGUMENTS comm_argument, char** param);

#endif
