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
	char* numbers;
	char* guess;
}Player;

typedef struct _file_params {
	const char* file_name;
	HANDLE file_mutex;
	bool *file_exists_p;
	long int cur_file_pos;
}File_params;


//Function Handles:
DWORD WINAPI service_thread(LPVOID lpParam);

Status send_to_client(SOCKET socket, Stage stage, char* message);

Status share_numbers(File_params* file_params, HANDLE opponent_event, Stage stage, Player *client, Player *opponent);

Status calculate_move_results(char** move_results, int buffer_size, Player* client, Player* opponent);

Status look_for_opponent(File_params* file_params, HANDLE opponent_event, Player* client, Player* opponent);

Status get_verdict(char** match_verdict, Player* client, Player* opponent);

void free_match_memory(File_params* file_params, Player* client, Player* opponent, char** move_results, char** match_verdict);

#endif
