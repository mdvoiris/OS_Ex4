//Main Client header

#ifndef __MAIN_CLIENT_H__
#define __MAIN_CLIENT_H__

#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define TIMEOUT_SEND 30000 
#define TIMEOUT_RECEIVE_SHORT 15000
#define TIMEOUT_RECEIVE_LONG 30000
#define DEFAULT_TIMEOUT 15000


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
	CLIENT_INVALID,
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
SOCKET m_socket;







//Function Handles:
//Prints errors according to the received status
void report_error(Status status);
//receives player name, indicator if it's a time for CLIENT REQUEST, the last server message, server port, server address, server address
//and the stausus of the client right now wich is sending. the function updates the next status 
//of the client, sending the message to the server and returns if there was an error or the user wants to quit.
Status send_level(char* player_name, SEND_SERVER* send_server, RECEIVE_SERVER receive_server, CLIENT_ACTION* client_action, int server_port, char* server_address);
//receives SOCKADDR_IN typdef, server port, server address, client status (as the above function) and taking care of reconnecting the user to the server if the user wants that.
//returns if there was an error or the user wants to quit.
Status connect_level(SOCKADDR_IN client_service, int server_port, char* server_address, SEND_SERVER* send_server, CLIENT_ACTION* client_action);
//receives player name, indicator if it's a time for CLIENT REQUEST, the last server message, server port, server address, server address
//and the stausus of the client right now wich is receiving. the function updates the next status 
//of the client, sending the message to the server and returns if there was an error or the user wants to quit.
Status receive_level(RECEIVE_SERVER* receive_serve, CLIENT_ACTION* client_action, int server_port, char* server_address);
//receives the status of the client (as the above function), srver port and server address. checks of the user wants to reconnect
//to the game. returns if there was an error or the user wants to exiit.
Status ask_to_reconnect(CLIENT_ACTION** client_action, int server_port, char* server_address);


#endif // __MAIN_H__