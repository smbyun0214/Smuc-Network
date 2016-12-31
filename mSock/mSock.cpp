#include "mSock.h"

mSock::mSock()
{

}

mSock::~mSock()
{
	Release();
}


void mSock::Initialize(char ip[], char port[])
{
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sock == -1)
	{
		perror("socket");
		exit(1);
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;

	if(ip == NULL)	// server
	{
		sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		sock_addr.sin_port = htons(atoi(port));

		if(bind(sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1)
		{
			perror("bind");
			exit(1);
		}

		if(listen(sock, 5) == -1)
		{
			perror("listen");
			exit(1);
		}
	}
	else	// client
	{
		sock_addr.sin_addr.s_addr = inet_addr(ip);
		sock_addr.sin_port = htons(atoi(port));
	}

}


void mSock::Connect()
{
	if(connect(sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1)
	{
		perror("connect");
		exit(1);
	}
	else
		puts("connected...");
}


void mSock::Accept(int& accept_sock, struct sockaddr_in& accept_sock_addr)
{
	socklen_t addr_sz = sizeof(accept_sock_addr);
	accept_sock = accept(sock, (struct sockaddr*) &accept_sock_addr, &addr_sz);

	if(accept_sock == -1)
	{
		perror("accept");
		exit(1);
	}
}


void mSock::Release()
{
	close(sock);
}