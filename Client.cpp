#include "Client.h"
#include <fstream>

Client::Client(string& path)
{
    env_path = path;
}

void Client::LoadEnvironment()
{
    ifstream file;
    file.open(env_path);
    file >> env;
}

void Client::Initialize()
{
	LoadEnvironment();
	InitializeDatabase();
	
}

char* StringToCharPointer(string& str)
{
	char* pChar = new char[str.size() + 1];
	copy(str.begin(), str.end(), pChar);
	pChar[str.size()] = '\0';

	return pChar;
}

void CharPointerToDelete(char* pChar)
{
	delete[] pChar;
}

void Client::InitializeDatabase()
{
	string strHost = env["mysql"]["host"];
	string strUser = env["mysql"]["user"];
	string strPassword = env["mysql"]["password"];

	char* host = StringToCharPointer(strHost);
	char* user = StringToCharPointer(strUser);
	char* password = StringToCharPointer(strPassword);

	m_mysql.Initialize(host, user, password);

	CharPointerToDelete(host);
	CharPointerToDelete(user);
	CharPointerToDelete(password);
}
