//Hard coded data header

#ifndef __HARD_CODED_DATA_H__
#define __HARD_CODED_DATA_H__

//Defines:
#define NUM_OF_LINES 100
#define MAX_THREADS MAXIMUM_WAIT_OBJECTS
#define MAX_WAIT_IN_MILISECONDS 10000
#define SERVER_PORT_LENTH 4
#define MAX_PLAYER_NAME 20


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
 /*   INVALID_NUM_OF_THREADS,
    INVALID_FLAG,
    INPUT_FOPEN_FAIL,
    ALLOC_FAILED,
    REALLOC_FAILED,
    FAILED_TO_CREATE_INPUT_FILE,
    FAILED_TO_CREATE_OUTPUT_FILE,
    FAILED_TO_SET_FILE_PTR,
    FAILED_TO_SET_EOF,
    FAILED_TO_CREARTE_THREAD,
    //FAILED_TO_READ_FILE,
    //FAILED_TO_WRITE_FILE,*/
    WSASTARTUP_FAILED,
    FAILED_CREATE_SOCKET,
    ILLEGAL_PORT,
    ILLEGAL_SERVER,
    FAILED_EVENT
} Status;


#endif // __HARD_CODED_DATA_H__