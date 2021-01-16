
#include "service_thread.h"

DWORD WINAPI service_thread(SOCKET* socket)
{
	Status status = INVALID_STATUS_CODE;
	Comm_status comm_status = INVALID_COMM_STATUS;
	const char file_name[] = "GameSession.txt";
	char* recieved = NULL;
	char* move_results = NULL;
	char* client_name = NULL;
	char client_numbers[5] = { 0 };
	char client_guess[5] = { 0 };
	char* opponent_name = NULL;
	char opponent_numbers[5] = { 0 };
	char opponent_guess[5] = { 0 };


	//wait for CLIENT_REQUEST
	comm_status = recieve_sting(recieved, socket);
	if (comm_status) { 
		status = FAILED_TO_RECIEVE_STRING;
		report_error(status, false);
		status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
		report_error(status, false);
		return status;
	}

	//send SERVER_APPROVED
	status = send_to_client(socket, REQUEST, NULL);
	if (status) {
		report_error(status, false);
		status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
		report_error(status, false);
		return status;
	}

	//while client is in
	while (true) {
		//send SERVER_MAIN_MENU
		status = send_to_client(socket, MAIN_MENU, NULL);
		if (status) {
			report_error(status, false);
			status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
			report_error(status, false);
			return status;
		}

		//wait for client choice
		comm_status = recieve_sting(recieved, socket);
		if (comm_status) {
			status = FAILED_TO_RECIEVE_STRING;
			report_error(status, false);
			status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
			report_error(status, false);
			return status;
		}

		//if client chose CLIENT_VERSUS
		if (recieved == "CLIENT_VERSUS\n") {

			//search for an already connected opponent or wait for one
			status = look_for_opponent(file_name, client_name, client_numbers, &opponent_name, &opponent_numbers);
			if (status) {
				report_error(status, false);
				status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
				report_error(status, false);
				return status;
			}

			//if can't find another opponent
			if (opponent_name == NULL) {
				//send SERVER_NO_OPPONENTS
				status = send_to_client(socket, NO_OPPONENT, NULL);
				//back to main menu
				continue;
			}

			//send SERVER_INVITE and SERVER_SETUP_REUEST
			status = send_to_client(socket, SETUP, opponent_name);
			if (status) {
				report_error(status, false);
				status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
				report_error(status, false);
				return status;
			}

			//wait for client response
			comm_status = recieve_sting(recieved, socket);
			if (comm_status) {
				status = FAILED_TO_RECIEVE_STRING;
				report_error(status, false);
				status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
				report_error(status, false);
				return status;
			}

			//get client numbers from recieved message
			//TODO

			//while game is on
			while (true) {
				//send SERVER_PLAYER_MOVE_REQUEST
				status = send_to_client(socket, MOVE, NULL);
				if (status) {
					report_error(status, false);
					status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
					report_error(status, false);
					return status;
				}

				//wait for client response
				comm_status = recieve_sting(recieved, socket);
				if (comm_status) {
					status = FAILED_TO_RECIEVE_STRING;
					report_error(status, false);
					status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
					report_error(status, false);
					return status;
				}

				//get client guess from recieved message
				//TODO

				//wait for opponent
				WaitForSingleObject(opponent_event, INFINITE);


				//calculate move results and format them as a string
				status = get_move_results(file_name, &move_results, client_guess);
				if (status) {
					report_error(status, false);
					status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
					report_error(status, false);
					return status;
				}

				//if opponent quit
				if (move_results == 0) {
					//send SERVER_OPPONENT_QUIT
					status = send_to_client(socket, OPPONENT_QUIT, NULL);
					//back to main menu
					break;
				}

				//send SERVER_GAME_RESULTS
				status = send_to_client(socket, RESULT, move_results);
				if (status) {
					report_error(status, false);
					status = (closesocket(socket) == SOCKET_ERROR) ? FAILED_TO_CLOSE_SOCKET : SUCCESS;
					report_error(status, false);
					return status;
				}

				//if game done by a win
				if (move_results == "win") {
					//send SERVER_WIN
					status = send_to_client(socket, VERDICT, move_results);
					//back to main menu
					break;
				}
				//if game done by a draw
				else if (move_results = "draw") {
					//send SERVER_DRAW
					status = send_to_client(socket, VERDICT, NULL);
					//back to main menu
					break;
				}

			}
		}
		else {
			//graceful shutdown
		}

	}


	return SUCCESS;
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
		if (message == NULL) {
			comm_status = send_string("SERVER_DRAW\n", socket);
		}
		else {
			buffer_size = (strlen("SERVER_DRAW:\n") + strlen(message) + 1);
			buffer = (char*)malloc(buffer_size * sizeof(char));
			if (buffer == NULL) {
				return ALLOC_FAILED;
			}
			sprintf_s(buffer, buffer_size, "SERVER_DRAW:%s\n", message);
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

Status get_move_results(const char file_name[], char** move_results, char* client_guess) {
	Status status = INVALID_STATUS_CODE;

	//get file mutex
	if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
		return UNRELEASED_MUTEX;




	ReleaseMutex(file_mutex);

	return SUCCESS;
}

Status look_for_opponent(const char file_name[], char* client_name, char* client_numbers, char** opponent_name, char* opponent_numbers) {
	Status status = INVALID_STATUS_CODE;
	FILE* session_file = NULL;
	int buffer_size = 0;
	DWORD return_value = 0;


	//get file mutex
	if (WaitForSingleObject(file_mutex, DEFAULT_TIMEOUT) != WAIT_OBJECT_0)
		return UNRELEASED_MUTEX;

	//check if file doesn't exist
	if ((fopen_s(&session_file, file_name, "r")) != 0) {

		//create it
		if ((fopen_s(&session_file, file_name, "w")) != 0) {
			return FOPEN_FAIL;
		}

		//write the client name and numbers
		fprintf_s(session_file, "%s;%s;", client_name, client_numbers);

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

			if ((fopen_s(&session_file, file_name, "r")) != 0) {
				return FOPEN_FAIL;
			}

			fseek(session_file, 0, SEEK_END);
			buffer_size = (ftell(session_file) - strlen(client_name) - (2 * 6));
			*opponent_name = (char*)malloc(buffer_size * sizeof(char));
			if (*opponent_name == NULL) {
				return ALLOC_FAILED;
			}
			fseek(session_file, strlen(client_name) + 6, SEEK_SET);
			fgets(*opponent_name, buffer_size, session_file);

			fgetc(session_file); //ignore ';'
			fgets(*opponent_numbers, 4, session_file);

			fclose(session_file);
			ReleaseMutex(file_mutex);
		}
		//no opponent joined
		else if (return_value == WAIT_TIMEOUT) {
			//remove session file
			if (remove(file_name) != 0)
				return FAILED_TO_REMOVE_FILE;
		}
		else {
			//remove session file
			if (remove(file_name) != 0)
				return FAILED_TO_REMOVE_FILE;

			return FAILED_TO_WAIT;
		}
		ResetEvent(opponent_event);
	}
	//if file exists
	else {

		if ((fopen_s(&session_file, file_name, "r+")) != 0) {
			return FOPEN_FAIL;
		}

		fseek(session_file, 0, SEEK_END);
		buffer_size = ftell(session_file) - 6;
		*opponent_name = (char*)malloc(buffer_size * sizeof(char));
		if (*opponent_name == NULL) {
			return ALLOC_FAILED;
		}
		fseek(session_file, 0, SEEK_SET);
		fgets(*opponent_name, buffer_size, session_file);

		fgetc(session_file); //ignore ';'
		fgets(*opponent_numbers, 4, session_file);

		fseek(session_file, 0, SEEK_END);

		//write the current user name
		fprintf_s(session_file, "%s;%s\n", client_name, client_numbers);

		fclose(session_file);

		ReleaseMutex(file_mutex);
		if (!SetEvent(opponent_event)) {
			return FAILED_TO_SET_EVENT;
		}
	}
	
	return SUCCESS;
}

