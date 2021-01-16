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


//Function Handles:
DWORD WINAPI service_thread(SOCKET* socket);

Status send_to_client(SOCKET socket, Stage stage, char* message);

Status get_move_results(const char file_name[], char** move_results, char* client_guess);

Status look_for_opponent(const char file_name[], char* client_name, char* client_numbers, char** opponent_name, char* opponent_numbers);

#endif
