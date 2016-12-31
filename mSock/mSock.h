#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

class mSock
{
protected:
	int sock;
    struct sockaddr_in sock_addr;

public:
	mSock();
	~mSock();

public:
	void Initialize(char ip[], char port[]);
	void Connect();
	void Accept(int& accept_sock, struct sockaddr_in& accept_sock_addr);
	void Release();
	int GetSock()
	{
		return sock;
	}
};