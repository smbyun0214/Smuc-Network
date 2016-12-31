#include "Client.h"

Client::Client(string& path)
{
    env_path = path;
}

Client::~Client()
{
	CharPointerToDelete(m_folder);
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
	InitializeClient();
	InitializeServer();
	InitializeFolder();
}

void Client::InitializeClient()
{
	string strIp = env["client"]["ip"];
	string strPort = env["client"]["port"];

	char* ip = StringToCharPointer(strIp);
	char* port = StringToCharPointer(strPort);

	m_client.Initialize(ip, port);

	CharPointerToDelete(ip);
	CharPointerToDelete(port);
}

void Client::InitializeServer()
{
	string strPort = env["server"]["port"];

	char* port = StringToCharPointer(strPort);

	m_server.Initialize(NULL, port);

	CharPointerToDelete(port);
}

void Client::InitializeFolder()
{
	string strPath = env["shared"];
	m_folder = StringToCharPointer(strPath);
}

void Client::SetIov(struct iovec (&iov)[IOV_LIST_CNT], DATA_INFO& data)
{
    for(int i = 0; i < IOV_LIST_CNT; i++)
    {
        switch(i)
        {
            case 0:
            iov[i].iov_base      = data.ip;
            iov[i].iov_len       = sizeof(data.ip);

            break;

            case 1:
            iov[i].iov_base      = data.port;
            iov[i].iov_len       = sizeof(data.port);

            break;

            case 2:
            iov[i].iov_base      = data.buf;
            iov[i].iov_len       = sizeof(data.buf);

            break;

            case 3:
            iov[i].iov_base      = data.modTime;
            iov[i].iov_len       = sizeof(data.modTime);

            break;
        }
    }
}


void Client::_SendList(char *path, time_t modTime, bool flag = false)
{
    memset(&m_send_info, 0, sizeof(m_send_info));

    struct tm localTime;

    if(flag == true)
    {
        send(sock, NULL, 0, MSG_OOB);
        return;
    }

    string strPort = env["server"]["port"];
    strcpy(m_send_info.port, strPort.c_str());

    strcpy(m_send_info.buf, path);

    localtime_r(&modTime, &localTime);
    strftime(m_send_info.modTime, TIME_SIZE, "%Y-%m-%d %H:%M:%S", &localTime);

    SetIov(m_send_list, m_send_info);
    int iWrite = writev(m_client.GetSock(), m_send_list, IOV_LIST_CNT);
}

void Client::ExploreDirectory(char *path)
{
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir(path)) == NULL)
        return;

    while((ent = readdir(dir)) != NULL)
    {
        if(!strncmp(ent->d_name, ".", 1))
            continue;

        if(ent->d_type == DT_DIR)
        {
            char newPath[BUF_SIZE];
            memset(&newPath, 0, BUF_SIZE);

            strcat(newPath, path);
            strcat(newPath, "/");
            strcat(newPath, ent->d_name);
            ExploreDirectory(newPath);
        }
        else
        {    
            char file[BUF_SIZE];
            struct stat state;

            sprintf(file, "%s/%s", path, ent->d_name);

            if(stat(file, &state) == -1)
                exit_message("stat() error!");

            _SendList(file, state.st_mtime);
        }
    }

    closedir(dir);
    return;
}

void Client::SendList()
{
	m_client.Connect();
    ExploreDirectory(m_folder);
    _SendList(NULL, 0, true);
    shutdown(m_client.GetSock(), SHUT_WR);
}