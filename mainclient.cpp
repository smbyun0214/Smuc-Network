#include "Client.h"

int main(void)
{
	string path = "./env_client.json";
	Client client(path);
	client.Initialize();
	client.SendList();
	// cout << sizeof(DATA_INFO) << endl;
	return 0;
}