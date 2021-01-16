
#include "shared_communication.h"

Comm_status send_buffer(const char* buffer, int bytes_to_send, SOCKET sd)
{
	const char* cur_place_ptr = buffer;
	int bytes_trans_ferred;
	int remaining_bytes_to_send = bytes_to_send;

	while (remaining_bytes_to_send > 0)
	{
		/* send does not guarantee that the entire message is sent */
		bytes_trans_ferred = send(sd, cur_place_ptr, remaining_bytes_to_send, 0);
		if (bytes_trans_ferred == SOCKET_ERROR)
		{
			return COMM_FAILED;
		}

		remaining_bytes_to_send -= bytes_trans_ferred;
		cur_place_ptr += bytes_trans_ferred; // <ISP> pointer arithmetic
	}

	return COMM_SUCCESS;
}

Comm_status send_string(const char* str, SOCKET sd)
{
	/* Send the the request to the server on socket sd */
	int total_string_size_in_bytes;
	Comm_status send_res;

	/* The request is sent in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	total_string_size_in_bytes = (int)(strlen(str) + 1); // terminating zero also sent	

	send_res = send_buffer(
		(const char*)(&total_string_size_in_bytes),
		(int)(sizeof(total_string_size_in_bytes)), // sizeof(int) 
		sd);

	if (send_res != COMM_SUCCESS) return send_res;

	send_res = send_buffer(
		(const char*)(str),
		(int)(total_string_size_in_bytes),
		sd);

	return send_res;
}

Comm_status receive_buffer(char* out_put_buffer, int bytes_to_receive, SOCKET sd)
{
	char* cur_place_ptr = out_put_buffer;
	int bytes_just_trans_ferred;
	int remaining_bytes_to_Receive = bytes_to_receive;

	while (remaining_bytes_to_Receive > 0)
	{
		/* send does not guarantee that the entire message is sent */
		bytes_just_trans_ferred = recv(sd, cur_place_ptr, remaining_bytes_to_Receive, 0);
		if (bytes_just_trans_ferred == 0 || WSAGetLastError() == WSAETIMEDOUT)
			return RECEIVE_DISCONNECTED; // recv() returns zero if connection was gracefully disconnected.
     	else if (bytes_just_trans_ferred == SOCKET_ERROR)
			return COMM_FAILED;
		remaining_bytes_to_Receive -= bytes_just_trans_ferred;
		cur_place_ptr += bytes_just_trans_ferred; // <ISP> pointer arithmetic
	}

	return COMM_SUCCESS;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

Comm_status receive_string(char** out_put_str_ptr, SOCKET sd)
{
	/* Recv the the request to the server on socket sd */
	int total_string_size_in_bytes;
	Comm_status RecvRes;
	char* str_buffer = NULL;

	if ((out_put_str_ptr == NULL) || (*out_put_str_ptr != NULL))
	{
		printf("The first input to ReceiveString() must be "
			"a pointer to a char pointer that is initialized to NULL. For example:\n"
			"\tchar* Buffer = NULL;\n"
			"\tReceiveString( &Buffer, ___ )\n");
		return INVALID_COMM_STATUS;
	}

	/* The request is received in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	RecvRes = ReceiveBuffer(
		(char*)(&total_string_size_in_bytes),
		(int)(sizeof(total_string_size_in_bytes)), // 4 bytes
		sd);

	if (RecvRes != COMM_SUCCESS) return RecvRes;

	str_buffer = (char*)malloc(total_string_size_in_bytes * sizeof(char));

	if (str_buffer == NULL)
		return MALLOC_FAILED;

	RecvRes = ReceiveBuffer(
		(char*)(str_buffer),
		(int)(total_string_size_in_bytes),
		sd);

	if (RecvRes == COMM_SUCCESS)
	{
		*out_put_str_ptr = str_buffer;
	}
	else
	{
		free(str_buffer);
	}

	return RecvRes;
}

void split(char str[], int i, char** str_cpy)
{
	char* token = NULL;
	char buffer[35];
	strcpy(buffer, str);
	token = strtok(buffer, ":");
	for (int j = 0; j < i; j++)
		token = strtok(NULL, ";");
	*str_cpy = token;
}