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
	struct sockaddr_storage client1_address;
	socklen_t client1_len = sizeof(client1_address);
	SOCKET socket_client1 = accept(socket_listen, (struct sockaddr*)&client1_address, &client1_len);
	if(!ISVALIDSOCKET(socket_client1))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	char address1_buffer[100];
	getnameinfo((struct sockaddr*)&client1_address, client1_len, address1_buffer, sizeof(address1_buffer), 0, 0, NI_NUMERICHOST);
	printf("Connected to %s.\n", address1_buffer);
	
	//Second client.	
	printf("Waiting for connection...\n");
	struct sockaddr_storage client2_address;
	socklen_t client2_len = sizeof(client2_address);
	SOCKET socket_client2 = accept(socket_listen, (struct sockaddr*)&client2_address, &client2_len);
	if(!ISVALIDSOCKET(socket_client2))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	char address2_buffer[100];
	getnameinfo((struct sockaddr*)&client2_address, client2_len, address2_buffer, sizeof(address2_buffer), 0, 0, NI_NUMERICHOST);
	printf("Connected to %s.\n", address2_buffer);

	while(1)
	{
		char message[1024];

		recv(socket_client1, message, 1024, 0);

		if(message == "leave")
		{
			break;
		}

		send(socket_client2, message, 1024, 0);
		
		recv(socket_client2, message, 1024, 0);
		
		if(message == "leave")
		{
			break;
		}

		send(socket_client1, message, 1024, 0);
	}

	printf("Closing connection...\n");
	CLOSESOCKET(socket_client1);
	CLOSESOCKET(socket_client2);

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

	#if defined(_WIN32)
		WSACleanup();
	#endif
	
	return EXIT_SUCCESS;
}

