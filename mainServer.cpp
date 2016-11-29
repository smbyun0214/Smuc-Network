#include "Server.h"

int main(void)
{
    char port[] = "9190";
    
    char host[] = "localhost";
    char user[] = "root";
    char passwd[] = "best";
    char db[]     = "FileList";
    char table[]  = "lists";

    Server* server = new Server();
    
    server->InitSock(port);
    server->InitMySQL(host, user, passwd, db, table);
    server->Initialize();
    
    server->RunReceive();

    // server->SelectRow();
    // server->SendList(clntInfo);
    

    delete server;
    return 0;
}