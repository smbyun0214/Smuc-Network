#include "Server.h"

int main(void)
{
    char port[] = "9199";

    char host[] = "localhost";
    char user[] = "root";
    char passwd[] = "tkfkdgo0!!";
    char db[]     = "FileList";
    char table[]  = "lists";

    Server* server = new Server();
    
    server->InitSock(port);
    server->InitMySQL(host, user, passwd, db, table);
    server->Initialize();
    
    server->RunReceive();
    

    delete server;
    return 0;
}