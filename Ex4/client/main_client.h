//Main Client header

#ifndef __MAIN_CLIENT_H__
#define __MAIN_CLIENT_H__

#define BRUTAL_TERMINATION_CODE 0x55
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define TIMEOUT_SEND 30000 
#define TIMEOUT_RECEIVE_SHORT 15000
#define TIMEOUT_RECEIVE_LONG 30000


//Includes:
#include "shared_communication.h"
#include "HardCodedData.h"

//Typedefs:
typedef enum _client_action {
	CONNECT,
	SEND,
	RECEIVE,
	DISCONNECT
} CLIENT_ACTION;

typedef enum _send_server {
	CLIENT_REQUEST,
	CLIENT_VERSUS,
	CLIENT_SETUP,
	CLIENT_PLAYER_MOVE,
	CLIENT_DISCONNECT
} SEND_SERVER;

typedef enum _receive_server {
	SERVER_MAIN_MENU,
	SERVER_APPROVED,
	SERVER_DENIED,
	SERVER_INVITE,
	SERVER_SETUP_REQUEST,
	SERVER_PLAYER_MOVE_REQUEST,
	SERVER_GAME_RESULTS,
	SERVER_WIN,
	SERVER_DRAW,
	SERVER_NO_OPPONENTS,
	SERVER_OPPONENT_QUIT
} RECEIVE_SERVER;



//Global Variables:
HANDLE open_event;
HANDLE close_event;
HANDLE user_event;
SOCKET m_socket;
bool new_communication_thread = true;
int time_out;






//Function Handles:
//Prints errors according to the recieved status
Status main(int argc, char* argv[]);
void report_error(Status status);
Status send_level(char* player_name, SEND_SERVER* send_server, RECEIVE_SERVER receive_server, CLIENT_ACTION* client_action, int server_port, long server_address);
Status connect_level(SOCKADDR_IN client_service, int server_port, long server_address, SEND_SERVER* send_server, CLIENT_ACTION* client_action);
Status receive_level(RECEIVE_SERVER receive_serve, CLIENT_ACTION* client_action);
//Status send_string(const char* Str, SOCKET sd);
//Status SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd);

#endif // __MAIN_H__