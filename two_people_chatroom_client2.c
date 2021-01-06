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


#if defined(_WIN32)
#include <conio.h>
#endif

int main(int argc, char* argv[])
{	
	#if defined(_WIN32)
		WSADATA d;
		if(WSAStartup(MAKEWORD(2, 2), &d))
		{
			fprintf(stderr, "Failed to initialize.\n");
			return EXIT_FAILURE;
		}
	#endif

	if(argc < 3)
	{
		fprintf(stderr, "Enter IP and port.\n");
		return EXIT_FAILURE;
	}

	printf("Configuring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *peer_address;
	if(getaddrinfo(argv[1], argv[2], &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
	}
	
	printf("Creating a socket...\n");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);

	if(!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}

	printf("Connecting...\n");
	if(connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{	
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return EXIT_FAILURE;
	}
	freeaddrinfo(peer_address);

	while(1)
	{
		char message[1024];

		recv(socket_peer, message, 1024, 0);

		printf("%s.\n", message);

		scanf("%s", message);

		send(socket_peer, message, 1024, 0);
	}
	
	printf("Closing connection...\n");
	CLOSESOCKET(socket_peer);

	#if defined(_WIN32)
		WSACleanup();
	#endif
	
	return EXIT_SUCCESS;
}

