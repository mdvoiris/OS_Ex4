//Service thread header

#ifndef __SERVICE_THREAD_H__
#define __SERVICE_THREAD_H__


//Includes:
#include "shared_communication.h"
#include "HardCodedData.h"
#include "main_server.h"


//Typedefs:
//enum to distinguish stages in the communication
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

//struct to hold game relevant information on players
typedef struct _player {
	char* name;
	char* numbers;
	char* guess;
}Player;

//struct to hold file relevant parameters and handles to synchronize file communication between threads
//all execpt cur_file_pos are shared between threads
typedef struct _file_params {
	const char* file_name;
	HANDLE file_mutex;
	bool *file_exists_p;
	long int cur_file_pos;
}File_params;


//Function Handles:
//main service thread function
//simulates the game stages and executes calls the execution functions
//in case client quits it notifies the opponent with event and clears memory
DWORD WINAPI service_thread(LPVOID lpParam);

//send to the client the message for the relevant stage in the game/communication
Status send_to_client(SOCKET socket, Stage stage, char* message);

//uses session file to communicate client and opponent numbers
//utilizes events and file mutex to coordinate
Status share_numbers(File_params* file_params, HANDLE opponent_event, Stage stage, Player *client, Player *opponent);

//build move_results string for each move preformed move
Status calculate_move_results(char** move_results, int buffer_size, Player* client, Player* opponent);

//check for opponent to play with
//uses session file and exchanges user names
Status look_for_opponent(File_params* file_params, HANDLE opponent_event, Player* client, Player* opponent);

//writes verdict if win or draw and update match_verdict string
Status get_verdict(char** match_verdict, Player* client, Player* opponent);

//frees all allocated memory and deletes session file
void free_match_memory(File_params* file_params, Player* client, Player* opponent, char** move_results, char** match_verdict);

#endif
