#include "Server.h"

int main(void)
{
	string path = "./env_server.json";
	Server server(path);
	server.Initialize();
	server.RunReceive();

	return 0;
}