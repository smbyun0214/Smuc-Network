#include "Client.h"

int main(void)
{
    char ip[] = "127.0.0.1";
    // char ip[] = "203.237.179.19";
    char port[] = "9199";
    char folder[] = "shared";

    
    char inPort[] = "9193";

    Client *client1 = new Client();
    SOCK_INFO& sockInfo = client1->GetSockInfo();
    
    // client->InitializeClient(ip, port, folder);
    client1->InitializeServer(inPort);


    // repeat a day
    // client->RunSendList();
    client1->ReceivePath();






    return 0;
}