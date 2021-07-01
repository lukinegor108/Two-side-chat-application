#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32 WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(cib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_SIZE 1000

void *my_receive(void * arg);
void *my_send(void * arg);

int main()
{	
	#if defined(_WIN32)
		WSADATA d;
		if(WSAStartup(MAKEWORD(2, 2), &d))
		{
			fprintf(stderr, "Failed to initialize.\n");
			return EXIT_FAILURE;
		}
	#endif

	printf("Configuring local address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);
	
	printf("Creating a socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);

	if(!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	printf("Binding socket to local address...\n");
	if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	freeaddrinfo(bind_address);

	printf("Listening...\n");
	if(listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	//First client.
	printf("Waiting for connection...\n");
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
	if(!ISVALIDSOCKET(socket_client))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	char address_buffer[100];
	getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
	printf("Connected to %s.\n", address_buffer);
	


	pthread_t thread_receive, thread_send;

	pthread_create(&thread_receive, NULL, my_receive, (void *)&socket_client);
	pthread_create(&thread_send, NULL, my_send, (void *)&socket_client);

	pthread_join(thread_receive, NULL);
	pthread_join(thread_send, NULL);
	


	printf("Client clossed the connection...");

	printf("Closing connection...\n");
	CLOSESOCKET(socket_client);

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

	#if defined(_WIN32)
		WSACleanup();
	#endif
	
	return EXIT_SUCCESS;
}

//Because of the big message size the last escape sequnce can be lost, or have some delay. Need to be fixed!

void *my_receive(void *arg)
{
	char message[MESSAGE_SIZE];
	SOCKET socket_client = *(SOCKET *)arg;

	while(1)
	{
		recv(socket_client, message, MESSAGE_SIZE, 0);
		printf("\033[1;31mPartner: \033[0m%s", message);
	}
}

void *my_send(void *arg)
{
	char message[MESSAGE_SIZE];
	SOCKET socket_client = *(SOCKET *)arg;

	while(1)
	{
		fgets(message, MESSAGE_SIZE, stdin);
		printf("\033[1A\033[K");
		printf("\033[1;32mMe: \033[0m%s", message);
		send(socket_client, message, MESSAGE_SIZE, 0);
	}
}
