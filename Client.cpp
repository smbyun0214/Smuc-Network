#include "Client.h"

Client::Client()
{
    pthread_mutex_init(&m_mutx, NULL);
}

Client::~Client()
{
    close(m_sockInfo.sock);
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

void Client::InitSock(SOCK_INFO& sockInfo, char *ip, char *port)
{
    sockInfo.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockInfo.sock == -1)
        exit_message("socket() error!");

    SetAddr(sockInfo.addr, ip, port);
}


void Client::InitializeClient(char *ip, char *port, char* folder)
{
    InitSock(m_sockInfo, ip, port);
    SetSharedFolder(folder);
    SetIovBuffer();
}

void Client::InitializeServer(char* port)
{
    SetPort(port);
    SetAddr(m_mySockInfo.addr, NULL, port);
}


void Client::AskSocket(SOCK_INFO& sockInfo)
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
        
        m_recv_list[i].iov_base	    = &m_recv_buf[i];
        m_recv_list[i].iov_len 		= sizeof(m_recv_buf);
    }

    memset(m_send_buf, 0, sizeof(m_send_buf));
}

void Client::SetPort(char *port)
{
    strcpy(m_port, port);
}


void Client::_SendList(char *path, time_t modTime, bool flag = false)
{
    static int iCnt = 0;

    if(flag == true)
    {
        if(iCnt != 0)
        {
            ddb
            writev(m_sockInfo.sock, m_send_list, iCnt + 1);
            send(m_sockInfo.sock, NULL, 0, MSG_OOB);
        }
    
        return;
    }

    strcpy(m_send_buf[iCnt].buf, path);
    m_send_buf[iCnt].modTime = modTime;
    strcpy(m_send_buf[iCnt].port, m_port);
    
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

            struct stat state;
            if(stat(file, &state) == -1)
                exit_message("stat() error!");

            _SendList(file, state.st_mtime);
        }
    }

    return;
}

void Client::SendList()
{
    ExploreDirectory(m_shared);
    _SendList(NULL, NULL, true);
}


void Client::ReceiveList()
{
    int iRead;
    CLNT_DATA_INFO dataInfo;

    char* path;
    char* ip;
    char* port;

    while((iRead = readv(m_sockInfo.sock, m_recv_list, 1) != 0))
    {
        dataInfo = m_recv_buf[0];
        if(strlen(dataInfo.buf) == 0)
            break;
        
        pthread_mutex_lock(&m_mutx);
        path = dataInfo.buf;
        ip = dataInfo.ip;
        port = dataInfo.port;



        pthread_create(&m_thId, NULL, thTransClient, (void*) &dataInfo);
        pthread_detach(m_thId);
        pthread_mutex_unlock(&m_mutx);

        printf("%s %s %s \n", path, ip, port);
    }
}


void* Clinet::thTransClient(void *arg)
{
    char buf[BUF_SIZE];
    CLNT_DATA_INFO dataInfo = *((CLNT_DATA_INFO*)arg);

    SOCK_INFO* sockInfo = new SOCK_INFO();
    InitSock(*sockInfo, dataInfo.ip, dataInfo.port);
    AskSocket(*sockInfo);

    read(sockInfo.sock, dataInfo.buf, BUF_SIZE);
    /////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////
/////////

}


void* th_Handle_Server(void *arg);
void Client::RunReceiveServer()
{
    while(1)
    {

    }
}





SOCK_INFO& Client::GetSockInfo()
{
    return m_sockInfo;
}
