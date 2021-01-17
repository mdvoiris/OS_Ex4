
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
			return SEND_DISCONNECTED;
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
		/*if (bytes_just_trans_ferred == 0 || WSAGetLastError() == WSAETIMEDOUT)
			return RECEIVE_DISCONNECTED; // recv() returns zero if connection was gracefully disconnected.*/
     	if ((bytes_just_trans_ferred == SOCKET_ERROR) || (bytes_just_trans_ferred == 0))
			return RECEIVE_DISCONNECTED;
		remaining_bytes_to_Receive -= bytes_just_trans_ferred;
		cur_place_ptr += bytes_just_trans_ferred; // <ISP> pointer arithmetic
	}

	return COMM_SUCCESS;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

Comm_status receive_string(char** out_put_str_ptr, SOCKET sd)
{
	/* Recv the the request to the server on socket sd */
	int total_string_size_in_bytes = 0;
	Comm_status recv_res;
	char* str_buffer = NULL;

	/* The request is received in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	recv_res = receive_buffer(
		(char*)(&total_string_size_in_bytes),
		(int)(sizeof(total_string_size_in_bytes)), // 4 bytes
		sd);

	if (recv_res != COMM_SUCCESS) return recv_res;

	str_buffer = (char*)malloc(total_string_size_in_bytes * sizeof(char));

	if (str_buffer == NULL)
		return MALLOC_FAILED;

	recv_res = receive_buffer(
		(char*)(str_buffer),
		(int)(total_string_size_in_bytes),
		sd);

	if (recv_res == COMM_SUCCESS)
	{
		*out_put_str_ptr = str_buffer;
	}
	else
	{
		free(str_buffer);
	}

	return recv_res;
}

Comm_status split(char* message, COMM_ARGUMENTS comm_argument, char** param)
{
	char* token;
	char* next_token;
	char buffer[MAX_LENֹֹֹ_RECEIVE];
	strcpy_s(buffer, MAX_LENֹֹֹ_RECEIVE, message);
	token = strtok_s(buffer, ":", &next_token);
	for (int j = 0; j < comm_argument; j++)
		token = strtok_s(NULL, ";", &next_token);
	*param = (char*)malloc(sizeof(char) * (strlen(token) + 1));
	if (*param == NULL)
	{
		return MALLOC_FAILED;
	}
	if (token[strlen(token) - 1] == '\n')
		token[strlen(token) - 1] = '\0';
	strcpy(*param, token);
	return COMM_SUCCESS;
}