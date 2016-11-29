#include "Client.h"

Client::Client()
{

}

Client::~Client()
{
    close(m_sockInfo.sock);
}

void Client::InitSock(SOCK_INFO& sockInfo, char *ip, char *port)
{
    sockInfo.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockInfo.sock == -1)
        exit_message("socket() error!");

    SetAddr(sockInfo.addr, ip, port);
}

void Client::SetAddr(struct sockaddr_in& addr, char* ip, char* port)
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    if(ip == NULL)
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        addr.sin_addr.s_addr = inet_addr(ip);

    addr.sin_port = htons(atoi(port));
}


void Client::InitializeClient(char *ip, char *port, char* folder)
{
    InitSock(m_sockInfo, ip, port);
    SetSharedFolder(folder);
    SetIovBuffer();
}

void Client::InitializeServer(char* port)
{
    SetAddr(m_mySockInfo.addr, NULL, port);
}


void Client::AskSocket(SOCK_INFO& sockInfo, char *ip, char *port)
{
    if(connect(sockInfo.sock, (struct sockaddr*) &sockInfo.addr, sizeof(sockInfo.addr)) == -1)
        exit_message("connect() error!");
    else
        puts("connected... ");
}

void Client::SetSharedFolder(char *folder)
{
    strcpy(m_shared, folder);
}

void Client::SetIovBuffer()
{
    for(int i = 0; i < IOV_LIST_CNT; i++)
    {
        m_send_list[i].iov_base	    = &m_send_buf[i];
        m_send_list[i].iov_len 		= sizeof(m_send_buf);
    }
}

void Client::SetPort(char *port)
{
    strcpy(m_port, port);
}


void Client::_SendList(char *path, bool flag = false)
{
    static int iCnt = 0;

    if(flag == true)
    {
        if(iCnt != 0)
        {
            db
            writev(m_sockInfo.sock, m_send_list, iCnt + 1);
            send(m_sockInfo.sock, NULL, 0, MSG_OOB);
        }
    
        return;
    }

    m_send_buf[iCnt].addr = m_mySockInfo.addr;
    strcpy(m_send_buf[iCnt].buf, path);
    
    iCnt++;

    if(iCnt == 10)   
    {
        writev(m_sockInfo.sock, m_send_list, IOV_LIST_CNT);
        memset(m_send_buf, 0, IOV_LIST_CNT);
        iCnt = 0;
    
        return;
    }
}


void Client::ExploreDirectory(char *path)
{
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir(path)) == NULL)
        return;

    while((ent = readdir(dir)) != NULL)
    {
        if(!strcmp(".", ent->d_name)
        || !strcmp("..", ent->d_name)
        || !strcmp(".DS_Store", ent->d_name)
        || !strcmp(".vscode", ent->d_name))
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
            sprintf(file, "%s/%s", path, ent->d_name);
            _SendList(file);
        }
    }

    return;
}

void Client::SendList()
{
    ExploreDirectory(m_shared);
    _SendList(NULL, true);
}








SOCK_INFO& Client::GetSockInfo()
{
    return m_sockInfo;
}
