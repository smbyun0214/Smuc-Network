#include <iostream>
#include <string>

#include "Client.h"

using namespace std;

int main(void)
{
	string path = "./env.json";
	Client client(path);
	client.Initialize();
}