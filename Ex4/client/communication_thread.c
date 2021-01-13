//Thread module

#include "thread.h"

DWORD WINAPI communication_thread(LPVOID lpParam)
{
	THREAD_ARGS* args = (THREAD_ARGS*)lpParam;
	SetEvent(open_event);

		if (connect(m_socket, (SOCKADDR*)&args->clientService, sizeof(args->clientService)) == SOCKET_ERROR) 
		{
			printf("Failed to connect.\n");
			WSACleanup();
			return;
		}
	

}