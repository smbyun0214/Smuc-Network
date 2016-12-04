#include "Client.h"

int main(void)
{
    // char ip[] = "127.0.0.1";
    char ip[] = "203.237.179.19";
    char port[] = "8000";
    char folder[] = "shared";


    char inPort[] = "9191";

    Client *client = new Client();
    SOCK_INFO& sockInfo = client->GetSockInfo();
    
    client->InitializeClient(ip, port, folder);
    client->InitializeServer(inPort);

    // SendLIst to Server
    client->AskSocket(sockInfo);
    client->SendList();

    // Receive List and Ask Client
    // client->ReceiveList();


    return 0;
}