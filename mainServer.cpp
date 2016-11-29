#include "Server.h"

int main(void)
{
    char port[] = "9190";
    
    char host[] = "localhost";
    char user[] = "root";
    char passwd[] = "best";
    char db[]     = "FileList";
    char table[]  = "lists";

    Server* server = new Server(port);

    SOCK_INFO& sockInfo = server->GetSockInfo();
     
    server->InitSock(sockInfo, port);
    server->InitMySQL(host, user, passwd, db);
    server->CreateTable(table);
    
    SOCK_INFO& clntInfo = server->AcceptSocket(sockInfo, port);
    server->SetIovBuffer();
    server->ReceiveList(clntInfo);

    server->SelectRow();
    server->SendList(clntInfo);
    

    return 0;
}