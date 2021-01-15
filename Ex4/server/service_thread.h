//Service thread header

#ifndef __SERVICE_THREAD_H__
#define __SERVICE_THREAD_H__


//Includes:
#include "HardCodedData.h"


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

Status send_to_client(Stage stage, char* message);

Status get_from_client(Stage stage, char** message);

Status get_move_results(char** move_results);

Status look_for_opponent(char** opponent_name);

#endif
