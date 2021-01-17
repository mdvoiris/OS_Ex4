//Hard coded data header

#ifndef __HARD_CODED_DATA_H__
#define __HARD_CODED_DATA_H__

//Defines:
#define MAX_LEN………_SEND 40
#define MAX_LEN………_RECEIVE 55
#define SERVER_PORT_LENGTH 4
#define MAX_PLAYER_NAME 20
#define USER_ANSWER_LEN 2


//Typedefs:
//Enum to organize the input argv
typedef enum _arg {
    SERVER_ADDRESS = 1,
    SERVER_PORT,
    PLAYER_NAME,
    ARG_COUNT
} Arg;
//All of the main functions return a status enum for debugging
typedef enum _status {
    INVALID_STATUS_CODE = -1,
    SUCCESS = 0,
    WRONG_ARGUMENT_COUNT,
    SENDING_DISCONNECTED,
    WSASTARTUP_FAILED,
    FAILED_CREATE_SOCKET,
    ILLEGAL_PORT,
    ILLEGAL_SERVER,
    USER_QUIT,
    USER_EXIT,
    FAILED_CLOSE_SOCKET,
    ALLOCTION_FAILED,
    SET_SOCKET_FAILED
} Status;



#endif // __HARD_CODED_DATA_H__