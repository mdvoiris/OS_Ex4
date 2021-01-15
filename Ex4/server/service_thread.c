
#include "service_thread.h"

DWORD WINAPI service_thread(SOCKET* socket)
{
	Status status = INVALID_STATUS_CODE;
	char* user_name = NULL;
	char* opponent_name = NULL;
	char* recieved = NULL;
	char* move_results = NULL;
	char user_numbers[5] = { 0 };
	char opponent_guess[5] = { 0 };


	status = get_from_client(REQUEST, &recieved);

	status = send_to_client(REQUEST, NULL);

	while (true) {
		status = send_to_client(MAIN_MENU, NULL);

		status = get_from_client(MAIN_MENU, &recieved);

		if (recieved == "CLIENT_VERSUS") {

			status = look_for_opponent(&opponent_name);

			if (opponent_name != NULL) {
				status = send_to_client(NO_OPPONENT, NULL);
				continue;
			}

			status = send_to_client(SETUP, opponent_name);//INVITE+SETUP_REQ

			status = get_from_client(SETUP, &recieved);

			//user_numbers = recieved;

			while (true) {
				status = send_to_client(MOVE, NULL);

				status = get_from_client(MOVE, &recieved);

				status = get_move_results(&move_results);
				if (move_results == 0) {
					status = send_to_client(OPPONENT_QUIT, NULL);
					break;
				}

				status = send_to_client(RESULT, move_results);

				if (move_results == "win") {
					status = send_to_client(VERDICT, move_results);
					break;
				}
				else if (move_results = "draw") {
					status = send_to_client(VERDICT, NULL);
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


Status send_to_client(Stage stage, char* message) {
	Status status = INVALID_STATUS_CODE;

	switch (stage) {
	case REQUEST: {

		break;
	}
	case MAIN_MENU: {

		break;
	}
	case NO_OPPONENT: {

		break;
	}
	case OPPONENT_QUIT: {

		break;
	}
	case SETUP: {

		break;
	}
	case MOVE: {

		break;
	}
	case RESULT: {

		break;
	}
	case VERDICT: {

		break;
	}
	}




	return SUCCESS;
}


Status get_from_client(Stage stage, char** message) {
	Status status = INVALID_STATUS_CODE;


	switch (stage) {
	case REQUEST: {

		break;
	}
	case MAIN_MENU: {

		break;
	}
	case NO_OPPONENT: {

		break;
	}
	case OPPONENT_QUIT: {

		break;
	}
	case SETUP: {

		break;
	}
	case MOVE: {

		break;
	}
	case RESULT: {

		break;
	}
	case VERDICT: {

		break;
	}
	}

	return SUCCESS;
}

Status get_move_results(char** move_results) {
	Status status = INVALID_STATUS_CODE;


	return SUCCESS;
}

Status look_for_opponent(char** opponent_name) {
	Status status = INVALID_STATUS_CODE;


	return SUCCESS;
}

