#include "Server.h"

int main(void)
{
    char port[] = "9190";
    Server* server = new Server(port);

    SOCK_INFO& sockInfo = server->GetSockInfo();
     
    server->InitSock(sockInfo, port);
    SOCK_INFO& clntInfo = server->AcceptSocket(sockInfo, port);
    server->SetIovBuffer();
    server->ReceiveList(clntInfo);

    return 0;

}