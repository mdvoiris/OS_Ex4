//Service thread header

#ifndef __SERVICE_THREAD_H__
#define __SERVICE_THREAD_H__


//Includes:
#include "shared_communication.h"
#include "HardCodedData.h"
#include "main_server.h"


//Typedefs:
typedef enum _stage {
	REQUEST,
	MAIN_MENU,
	NO_OPPONENT,
	OPPONENT_QUIT,
	SETUP,
	MOVE,
	RESULT,
	VERDICT
}Stage;

typedef struct _player {
	char* name;
	char numbers[5]; //4 +'\0'
}Player;

//Global veriables:
const char file_name[] = "GameSession.txt";
Player client = { 0 };
Player opponent = { 0 };


//Function Handles:
DWORD WINAPI service_thread(SOCKET* socket);

Status send_to_client(SOCKET socket, Stage stage, char* message);

Status get_move_results(char** move_results, char* client_guess);

Status look_for_opponent();

#endif
