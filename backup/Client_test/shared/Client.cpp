#include "Client.h"

Client::Client()
{
    pthread_mutex_init(&m_clnt_mutx, NULL);
    pthread_mutex_init(&m_serv_mutx, NULL);

}

Client::~Client()
{
    pthread_mutex_destroy(&m_clnt_mutx);
    pthread_mutex_destroy(&m_serv_mutx);

    close(m_mySockInfo.sock);
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

void Client::SetPort(char *port)
{
    strcpy(m_port, port);
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
    InitSock(m_mySockInfo, NULL, port);

    if(bind(m_mySockInfo.sock, (struct sockaddr*) &m_mySockInfo.addr, sizeof(m_mySockInfo.addr)) == -1)
        exit_message("bind() error!");

    if(listen(m_mySockInfo.sock, 5) == -1)
        exit_message("listen() error");
}


void Client::AskSocket(SOCK_INFO& sockInfo)
{
    if(connect(sockInfo.sock, (struct sockaddr*) &sockInfo.addr, sizeof(sockInfo.addr)) == -1)
        exit_message("connect() error!");
    else
        puts("connected... ");
}

SOCK_INFO* Client::AcceptSocket(SOCK_INFO& sockInfo)
{
    SOCK_INFO* clnt_sockInfo = new SOCK_INFO();
    socklen_t clnt_addr_sz = sizeof(clnt_sockInfo->addr);

    clnt_sockInfo->sock = accept(m_mySockInfo.sock, (struct sockaddr*) &clnt_sockInfo->addr, &clnt_addr_sz);
    if(clnt_sockInfo->sock == -1)
        exit_message("accept() error!");

    return clnt_sockInfo;
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


void Client::_SendList(char *path, time_t modTime, bool flag = false)
{
    static int iCnt = 0;

    if(flag == true)
    {
        if(iCnt != 0)
        {
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
    _SendList(NULL, 0, true);
    // shutdown(m_sockInfo.sock, SHUT_WR);
}


void* th_Handle_Download(void* arg);
void Client::ReceiveList()
{
    int iRead;
    CLNT_DATA_INFO dataInfo;
    static int i = 0;


    while(1)
    {
        pthread_mutex_lock(&m_clnt_mutx);
        iRead = readv(m_sockInfo.sock, m_recv_list, 1);
        pthread_mutex_unlock(&m_clnt_mutx);
        
        if(iRead == 0)
            break;


        else if(strlen(m_recv_buf[0].buf) == 0)
            continue;
        

        pthread_mutex_lock(&m_clnt_mutx);

        dataInfo = m_recv_buf[0];
        printf("\t %s %s %s \n", dataInfo.ip, dataInfo.port, dataInfo.buf);


        pthread_create(&m_thId, NULL, th_Handle_Download, (void*) this);
        pthread_detach(m_thId);

        i++;
    }

}


void* th_Handle_Download(void* arg)
{    
    Client* client = (Client*) arg;

    SOCK_INFO* sockInfo = new SOCK_INFO();
    CLNT_DATA_INFO dataInfo = client->GetRecvBuf()[0];

    pthread_mutex_t& mutex = client->GetClientMutex();

    client->InitSock(*sockInfo, dataInfo.ip, dataInfo.port);
    client->AskSocket(*sockInfo);

    write(sockInfo->sock, dataInfo.buf, BUF_SIZE);

    pthread_mutex_unlock(&mutex);

    FILE* fp = fopen(dataInfo.buf, "w");
    int fd = fileno(fp);

    int iFileRead;
    char buf[BUF_SIZE];

    while((iFileRead = read(sockInfo->sock, buf, BUF_SIZE)) != 0)
        write(fd, buf, iFileRead);


    fclose(fp);
    close(sockInfo->sock);
    printf("Downloaded : %s %s %s \n", dataInfo.ip, dataInfo.port, dataInfo.buf);

    delete sockInfo;
    return NULL;
}


void Client::RunSendList()
{
    AskSocket(m_sockInfo);
    SendList();
    ReceiveList();
    
    // Need time receive files;
    sleep(100);

}


void* th_Handle_Upload(void* arg);
void Client::ReceivePath()
{
    while(1)
    {
        SOCK_INFO* clntInfo = AcceptSocket(m_mySockInfo);

        pthread_mutex_lock(&m_serv_mutx);
        m_listSockInfo.push_back(clntInfo);

        pthread_create(&m_thId, NULL, th_Handle_Upload, (void*) this);
        pthread_detach(m_thId);
        printf("Connected client IP: %s \n", inet_ntoa(clntInfo->addr.sin_addr));
    }
}


void* th_Handle_Upload(void* arg)
{   
    Client* client = (Client*) arg;

    list<SOCK_INFO*>& listSockInfo = client->GetListSockInfo();
    SOCK_INFO sockInfo = *listSockInfo.back();

    pthread_mutex_t& mutex = client->GetServerMutex();


    // Read FIle and Write to socket.
    char path[BUF_SIZE];
    read(sockInfo.sock, path, BUF_SIZE);
    printf("client >> server: %s \n", path);

    FILE* fp = fopen(path, "r");
    int fd = fileno(fp);

    pthread_mutex_unlock(&mutex);

    int iRead;
    char buf[BUF_SIZE];

    while((iRead = read(fd, buf, BUF_SIZE)) != 0)
        write(sockInfo.sock, buf, iRead);


    list<SOCK_INFO*>::iterator iter;
    for(iter = listSockInfo.begin(); iter != listSockInfo.end(); ++iter)
    {
        if((*iter)->sock == sockInfo.sock)
            break;
    }

    fclose(fp);
    close(sockInfo.sock);

    return NULL;
}
