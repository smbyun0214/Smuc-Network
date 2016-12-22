#include <stdio.h>
#include "mSock.h"


int main(void)
{
	char ip[] = "127.0.0.1";
	char port[] = "9190";
	// int iSock;
	// struct sockaddr_in addr;
	
	mSock sock;

	// sock.Initialize(NULL, port);
	// sock.Accept(&iSock, &addr);
	
	sock.Initialize(ip, port);
	sock.Connect();

	return 0;	
}