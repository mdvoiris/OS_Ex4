
#include "service_thread.h"


//Global veriables:
const char file_name[] = "GameSession.txt";
bool file_exists = false;


DWORD WINAPI service_thread(LPVOID lpParam)
{
	Status status = INVALID_STATUS_CODE;
	Comm_status comm_status = INVALID_COMM_STATUS;
	Client_args* args = (Client_args*)lpParam;
	SOCKET socket = args->socket;
	HANDLE file_mutex = args->file_mutex;
	HANDLE opponent_event = args->opponent_event;
	HANDLE opponent_disconnect_event = args->opponent_disconnect_event;
	Player client = { NULL };
	Player opponent = { NULL };
	char* recieved = NULL;
	char* move_results = NULL;
	char* match_verdict = NULL;
	int move_results_size = 0;
	long int cur_file_pos = 0;


	//wait for CLIENT_REQUEST
	printf("waiting for CLIENT_REQUEST\n"); //REMOVE
	comm_status = receive_string(&recieved, socket);
	if (comm_status) { 
		status = FAILED_TO_RECIEVE_STRING;
		goto EXIT;
	}
	printf("recieved \t %s\n", recieved);
	//get client name from recieved message
	split(recieved, PARAM_1, &(client.name));

	//send SERVER_APPROVED
	status = send_to_client(socket, REQUEST, NULL);
	if (status) {
		goto EXIT;
	}

	//while client is in
	while (true) {
		//send SERVER_MAIN_MENU
		status = send_to_client(socket, MAIN_MENU, NULL);
		if (status) {
			goto EXIT;
		}

		//wait for client choice
		printf("waiting for client choise\n");
		comm_status = receive_string(&recieved, socket);
		printf("finished waiting for client choise, recieved \t %s\n", recieved);
		if (comm_status) {
			status = FAILED_TO_RECIEVE_STRING;
			goto EXIT;
		}

		//if client chose CLIENT_VERSUS
		if (strcmp(recieved,"CLIENT_VERSUS\n") == 0) {

			//search for an already connected opponent or wait for one
			printf("looking for opponent\n");
			status = look_for_opponent(file_mutex, &cur_file_pos, opponent_event, &client, &opponent);
			printf("finished looking for opponent\n");
			if (status) {
				//if failed because of opponent disconnect go back to main menu
				if (WaitForSingleObject(opponent_disconnect_event, 0) == WAIT_OBJECT_0) {
					status = send_to_client(socket, OPPONENT_QUIT, NULL);
					if (status) {
						goto EXIT;
					}
					//get file mutex
					if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
						report_error(UNRELEASED_MUTEX, false);
					//remove session file
					if (file_exists) {
						if (remove(file_name) != 0)
							report_error(FAILED_TO_REMOVE_FILE, false);
						file_exists = false;
					}
					ReleaseMutex(file_mutex);
					ResetEvent(opponent_disconnect_event);
					continue;
				}
				goto EXIT;
			}

			//if can't find another opponent
			if (opponent.name == NULL) {
				printf("can't find opponent\n");
				//send SERVER_NO_OPPONENTS
				status = send_to_client(socket, NO_OPPONENT, NULL);
				//back to main menu
				continue;
			}
			printf("found opponent\n");
			//send SERVER_INVITE and SERVER_SETUP_REUEST
			status = send_to_client(socket, SETUP, opponent.name);
			if (status) {
				goto EXIT;
			}

			//wait for client response
			comm_status = receive_string(&recieved, socket);
			if (comm_status) {
				status = FAILED_TO_RECIEVE_STRING;
				goto EXIT;
			}

			//get client numbers from recieved message
			split(recieved, PARAM_1, &(client.numbers));

			//get opponent numbers and share client numbers
			status = share_numbers(file_mutex, &cur_file_pos, opponent_event, SETUP, &client, &opponent);
			if (status) {
				//if failed because of opponent disconnect go back to main menu
				if (WaitForSingleObject(opponent_disconnect_event, 0) == WAIT_OBJECT_0) {
					status = send_to_client(socket, OPPONENT_QUIT, NULL);
					if (status) {
						goto EXIT;
					}
					//get file mutex
					if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
						report_error(UNRELEASED_MUTEX, false);
					//remove session file
					if (file_exists) {
						if (remove(file_name) != 0)
							report_error(FAILED_TO_REMOVE_FILE, false);
						file_exists = false;
					}
					ReleaseMutex(file_mutex);
					ResetEvent(opponent_disconnect_event);
					continue;
				}
				goto EXIT;
			}

			//allocate space for move results message
			move_results_size = 2 + strlen(opponent.name) + NUM_OF_DIGITS + 5; //5 = (3*';' + '/n' + '/0')
			move_results = (char*)malloc(move_results_size * sizeof(char));
			if (move_results == NULL) {
				status = ALLOC_FAILED;
				goto EXIT;
			}

			//while game is on
			while (true) {
				//send SERVER_PLAYER_MOVE_REQUEST
				status = send_to_client(socket, MOVE, NULL);
				if (status) {
					goto EXIT;
				}

				//wait for client response
				comm_status = receive_string(&recieved, socket);
				if (comm_status) {
					status = FAILED_TO_RECIEVE_STRING;
					goto EXIT;
				}

				//get client guess from recieved message
				split(recieved, PARAM_1, &(client.guess));


				//get opponent guess and share client guess
				status = share_numbers(file_mutex, &cur_file_pos, opponent_event, MOVE, &client, &opponent);
				if (status) {
					//if failed because of opponent disconnect go back to main menu
					if (WaitForSingleObject(opponent_disconnect_event, 0) == WAIT_OBJECT_0) {
						status = send_to_client(socket, OPPONENT_QUIT, NULL);
						if (status) {
							goto EXIT;
						}
						//get file mutex
						if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
							report_error(UNRELEASED_MUTEX, false);
						//remove session file
						if (file_exists) {
							if (remove(file_name) != 0)
								report_error(FAILED_TO_REMOVE_FILE, false);
							file_exists = false;
						}
						ReleaseMutex(file_mutex);
						ResetEvent(opponent_disconnect_event);
						break;
					}
					goto EXIT;
				}

				//calculate move results and format them in a string
				status = calculate_move_results(&move_results, move_results_size, &client, &opponent);
				if (status) {
					goto EXIT;
				}

				//send SERVER_GAME_RESULTS
				status = send_to_client(socket, RESULT, move_results);
				if (status) {
					goto EXIT;
				}

				//get move verdict
				status = get_verdict(&match_verdict, &client, &opponent);
				if (status) {
					goto EXIT;
				}

				//if game done
				if (match_verdict != NULL) {
					//send SERVER_DRAW/SERVER_WIN
					status = send_to_client(socket, VERDICT, match_verdict);
					if (status) {
						goto EXIT;
					}
					free(move_results);
					free(match_verdict);
					move_results = NULL;
					match_verdict = NULL;
					//get file mutex
					if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
						report_error(UNRELEASED_MUTEX, false);
					//remove session file
					if (file_exists) {
						if (remove(file_name) != 0)
							report_error(FAILED_TO_REMOVE_FILE, false);
						file_exists = false;
					}
					ReleaseMutex(file_mutex);
					//back to main menu
					break;
				}
			}
		}
		else {
			status = SUCCESS;
			goto EXIT;
		}

	}

	return SUCCESS;


EXIT:
	//signal opponent that client disconnected
	if (!SetEvent(opponent_disconnect_event))
		report_error(FAILED_TO_SET_EVENT, false);
	//get file mutex
	if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
		report_error(UNRELEASED_MUTEX, false);
	//remove session file
	if (file_exists) {
		if (remove(file_name) != 0)
			report_error(FAILED_TO_REMOVE_FILE, false);
		file_exists = false;
	}
	ReleaseMutex(file_mutex);
	if (move_results != NULL)
		free(move_results);
	if (match_verdict != NULL)
		free(match_verdict);
	report_error(status, false);
	status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
	report_error(status, false);
	return status;
}


Status send_to_client(SOCKET socket, Stage stage, char* message) {
	Status status = INVALID_STATUS_CODE;
	Comm_status comm_status = INVALID_COMM_STATUS;
	char* buffer = NULL;
	size_t buffer_size = 0;


	switch (stage) {
	case REQUEST: {
		comm_status = send_string("SERVER_APPROVED\n", socket);
		break;
	}
	case MAIN_MENU: {
		comm_status = send_string("SERVER_MAIN_MENU\n", socket);
		break;
	}
	case NO_OPPONENT: {
		comm_status = send_string("SERVER_NO_OPPONENTS\n", socket);
		break;
	}
	case OPPONENT_QUIT: {
		comm_status = send_string("SERVER_OPPONENT_QUIT\n", socket);
		break;
	}
	case SETUP: {
		buffer_size = (strlen("SERVER_INVITE:\n") + strlen(message) + 1);
		buffer = (char*)malloc(buffer_size * sizeof(char));
		if (buffer == NULL) {
			return ALLOC_FAILED;
		}
		sprintf_s(buffer, buffer_size, "SERVER_INVITE:%s\n", message);
		comm_status = send_string(buffer, socket);
		if (comm_status) {
			free(buffer);
			return FAILED_TO_SEND_STRING;
		}

		comm_status = send_string("SERVER_SETUP_REQUEST\n", socket);
		break;
	}
	case MOVE: {
		comm_status = send_string("SERVER_PLAYER_MOVE_REQUEST\n", socket);
		break;
	}
	case RESULT: {
		buffer_size = (strlen("SERVER_GAME_RESULTS:\n") + strlen(message) + 1);
		buffer = (char*)malloc(buffer_size * sizeof(char));
		if (buffer == NULL) {
			return ALLOC_FAILED;
		}
		sprintf_s(buffer, buffer_size, "SERVER_GAME_RESULTS:%s\n", message);
		comm_status = send_string(buffer, socket);
		break;
	}
	case VERDICT: {
		if (strcmp(message, "SERVER_DRAW\n") == 0) {
			comm_status = send_string(message, socket);
		}
		else {
			buffer_size = (strlen("SERVER_WIN:\n") + strlen(message) + 1);
			buffer = (char*)malloc(buffer_size * sizeof(char));
			if (buffer == NULL) {
				return ALLOC_FAILED;
			}
			sprintf_s(buffer, buffer_size, "SERVER_WIN:%s\n", message);
			comm_status = send_string(buffer, socket);
		}
		break;
	}
	}

	if (buffer != NULL)
		free(buffer);

	if (comm_status) {
		return FAILED_TO_SEND_STRING;
	}


	return SUCCESS;
}

Status share_numbers(HANDLE file_mutex, int *cur_file_pos, HANDLE opponent_event, Stage stage, Player *client, Player *opponent) {
	Status status = INVALID_STATUS_CODE;
	FILE* session_file = NULL;
	DWORD return_value = 0;
	char* write_numbers = NULL;
	char* read_numbers = NULL;


	if (stage == SETUP) {
		opponent->guess = (char*)malloc((NUM_OF_DIGITS + 1) * sizeof(char));
		if (opponent->guess == NULL) {
			return ALLOC_FAILED;
		}
		opponent->numbers = (char*)malloc((NUM_OF_DIGITS + 1) * sizeof(char));
		if (opponent->numbers == NULL) {
			return ALLOC_FAILED;
		}
		write_numbers = client->numbers;
		read_numbers = opponent->numbers;
	}
	else {
		write_numbers = client->guess;
		read_numbers = opponent->guess;
	}

	//get file mutex
	if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
		return UNRELEASED_MUTEX;

	if ((fopen_s(&session_file, file_name, "rb+")) != 0) {
		return FOPEN_FAIL;
	}

	fseek(session_file, 0, SEEK_END);
	//if opponent entered
	if (ftell(session_file) != *cur_file_pos) {

		//get opponent numbers
		fseek(session_file, *cur_file_pos, SEEK_SET);
		fgets(read_numbers, (NUM_OF_DIGITS + 1), session_file);

		//write your numbers
		fseek(session_file, 0, SEEK_END);
		fprintf_s(session_file, "%s\n", write_numbers);
		*cur_file_pos = ftell(session_file);

		fclose(session_file);
		ReleaseMutex(file_mutex);
		if (!SetEvent(opponent_event)) {
			return FAILED_TO_SET_EVENT;
		}
	}
	//opponent didn't enter yet
	else {
		fseek(session_file, *cur_file_pos, SEEK_SET);

		//write your guess
		fprintf_s(session_file, "%s;", write_numbers);
		*cur_file_pos = ftell(session_file);

		fclose(session_file);
		ReleaseMutex(file_mutex);


		//wait for opponent
		return_value = WaitForSingleObject(opponent_event, INFINITE);
		//if opponent entered
		if (return_value == WAIT_OBJECT_0) {

			//get file mutex
			if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
				return UNRELEASED_MUTEX;

			if ((fopen_s(&session_file, file_name, "rb")) != 0) {
				return FOPEN_FAIL;
			}

			fseek(session_file, *cur_file_pos, SEEK_SET);
			fgets(read_numbers, (NUM_OF_DIGITS + 1), session_file);

			fseek(session_file, 0, SEEK_END);
			*cur_file_pos = ftell(session_file);

			fclose(session_file);
			ReleaseMutex(file_mutex);
			ResetEvent(opponent_event);
		}
		else {
			return FAILED_TO_WAIT;
		}
	}

	return SUCCESS;
}

Status look_for_opponent(HANDLE file_mutex, int* cur_file_pos, HANDLE opponent_event, Player* client, Player* opponent) {
	Status status = INVALID_STATUS_CODE;
	FILE* session_file = NULL;
	int buffer_size = 0;
	DWORD return_value = 0;


	//get file mutex
	if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
		return UNRELEASED_MUTEX;

	//check if file doesn't exist
	if (file_exists == false) {

		//create it
		if ((fopen_s(&session_file, file_name, "wb")) != 0) {
			return FOPEN_FAIL;
		}
		file_exists = true;

		//write the client name and numbers
		fprintf_s(session_file, "%s;", client->name);
		*cur_file_pos = ftell(session_file);

		//close file and release
		fclose(session_file);
		ReleaseMutex(file_mutex);

		//wait for opponent
		return_value = WaitForSingleObject(opponent_event, DEFAULT_TIMEOUT);
		//if opponent entered
		if (return_value == WAIT_OBJECT_0) {

			//get file mutex
			if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
				return UNRELEASED_MUTEX;

			if ((fopen_s(&session_file, file_name, "rb")) != 0) {
				return FOPEN_FAIL;
			}

			fseek(session_file, 0, SEEK_END);
			buffer_size = (ftell(session_file) - *cur_file_pos);
			opponent->name = (char*)malloc(buffer_size * sizeof(char));
			if (opponent->name == NULL) {
				return ALLOC_FAILED;
			}
			fseek(session_file, *cur_file_pos, SEEK_SET);
			fgets(opponent->name, buffer_size, session_file);

			fseek(session_file, 0, SEEK_END);
			*cur_file_pos = ftell(session_file);

			fclose(session_file);
			ReleaseMutex(file_mutex);
			ResetEvent(opponent_event);
		}
		//no opponent joined
		else if (return_value == WAIT_TIMEOUT) {
			//get file mutex
			if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
				report_error(UNRELEASED_MUTEX, false);
			//remove session file
			if (file_exists) {
				if (remove(file_name) != 0)
					report_error(FAILED_TO_REMOVE_FILE, false);
				file_exists = false;
			}
			ReleaseMutex(file_mutex);
			ResetEvent(opponent_event);
		}
		else {
			return FAILED_TO_WAIT;
		}
	}
	//if file exists
	else {

		if ((fopen_s(&session_file, file_name, "rb+")) != 0) {
			return FOPEN_FAIL;
		}

		fseek(session_file, 0, SEEK_END);
		buffer_size = ftell(session_file);
		opponent->name = (char*)malloc(buffer_size * sizeof(char));
		if (opponent->name == NULL) {
			return ALLOC_FAILED;
		}
		fseek(session_file, 0, SEEK_SET);
		fgets(opponent->name, buffer_size, session_file);

		fseek(session_file, 0, SEEK_END);

		//write the current user name
		fprintf_s(session_file, "%s\n", client->name);
		*cur_file_pos = ftell(session_file);

		fclose(session_file);

		ReleaseMutex(file_mutex);
		if (!SetEvent(opponent_event)) {
			return FAILED_TO_SET_EVENT;
		}
	}
	
	return SUCCESS;
}

Status calculate_move_results(char** move_results, int buffer_size, Player* client, Player* opponent) {
	Status status = INVALID_STATUS_CODE;
	int bulls = 0;
	int cows = 0;


	for (int i = 0; i < NUM_OF_DIGITS; i++) {
		if (client->guess[i] == opponent->numbers[i]) {
			bulls++;
		}
		else {
			for (int j = 0; j < NUM_OF_DIGITS; j++) {
				if (client->guess[i] == opponent->numbers[j])
					cows++;
			}
		}
	}

	sprintf_s(*move_results, buffer_size, "%d;%d;%s;%s", bulls, cows, opponent->name, opponent->guess);

	return SUCCESS;
}

Status get_verdict(char** match_verdict, Player* client, Player* opponent) {
	Status status = INVALID_STATUS_CODE;
	int buffer_size = 0;

	//if draw
	if ((strcmp(client->guess, opponent->numbers) == 0) && (strcmp(opponent->guess, client->numbers) == 0)) {
		buffer_size = strlen("SERVER_DRAW\n") + 1;
		*match_verdict = (char*)malloc(buffer_size * sizeof(char));
		if (*match_verdict == NULL) {
			return ALLOC_FAILED;
		}
		sprintf_s(*match_verdict, buffer_size, "SERVER_DRAW\n");
	}
	//else if client wins
	else if (strcmp(client->guess, opponent->numbers) == 0) {
		buffer_size = (strlen(client->name) + NUM_OF_DIGITS + 2);
		*match_verdict = (char*)malloc(buffer_size * sizeof(char));
		if (*match_verdict == NULL) {
			return ALLOC_FAILED;
		}
		sprintf_s(*match_verdict, buffer_size, "%s;%s", client->name, opponent->numbers);
	}
	//else if opponent wins
	else if (strcmp(opponent->guess, opponent->numbers) == 0) {
		buffer_size = (strlen(opponent->name) + NUM_OF_DIGITS + 2);
		*match_verdict = (char*)malloc(buffer_size * sizeof(char));
		if (*match_verdict == NULL) {
			return ALLOC_FAILED;
		}
		sprintf_s(*match_verdict, buffer_size, "%s;%s", opponent->name, opponent->numbers);
	}


	return SUCCESS;
}