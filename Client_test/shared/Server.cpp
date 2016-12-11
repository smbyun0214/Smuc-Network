#include "Server.h"

Server::Server()
{
    pthread_mutex_init(&m_mutx, NULL);
}

Server::~Server()
{
    pthread_mutex_destroy(&m_mutx);
    close(m_sockInfo.sock);
}







void Server::InitSock(char *port)
{
    CreateSocket(m_sockInfo, port);
}


void Server::CreateSocket(SOCK_INFO& sockInfo, char *port)
{
    sockInfo.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   
    if(sockInfo.sock == -1)
        exit_message("socket() error!");

    memset(&sockInfo.addr, 0, sizeof(sockInfo.addr));
    sockInfo.addr.sin_family = AF_INET;
    sockInfo.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockInfo.addr.sin_port = htons(atoi(port));
    
    if(bind(sockInfo.sock, (struct sockaddr*) &sockInfo.addr, sizeof(sockInfo.addr)) == -1)
        exit_message("bind() error!");

	if(listen(sockInfo.sock, 5) == -1)
		exit_message("listen() error");
}


SOCK_INFO& Server::AcceptSocket(SOCK_INFO& sockInfo)
{
    SOCK_INFO* clnt_sockInfo = new SOCK_INFO();
    socklen_t clnt_addr_sz = sizeof(clnt_sockInfo->addr);
    clnt_sockInfo->sock = accept(m_sockInfo.sock, (struct sockaddr*) &clnt_sockInfo->addr, &clnt_addr_sz);
    if(clnt_sockInfo->sock == -1)
        exit_message("accept() error!");

    return *clnt_sockInfo;
}


void Server::SetIovBuffer()
{
    for(int i = 0; i < IOV_LIST_CNT; i++)
    {
        m_recv_list[i].iov_base	    = &m_recv_buf[i];
        m_recv_list[i].iov_len 		= sizeof(m_recv_buf);

        m_send_list[i].iov_base	    = &m_send_buf[i];
        m_send_list[i].iov_len 		= sizeof(m_send_buf);
    }
}








void Server::ReceiveList(SOCK_INFO& sockInfo)
{
    int iRead;
    CLNT_DATA_INFO dataInfo;

    char* path;
    char* clntIp;
    char* clntPort;

    time_t modTime;
    struct tm localTime;
    char timeBuf[80];

    while((iRead = readv(sockInfo.sock, m_recv_list, 1)) != 0)
    {
        dataInfo = m_recv_buf[0];
        if(strlen(dataInfo.buf) == 0)
            break;
        

        // INSERT MYSQL
        /*
        path = dataInfo.buf;
        clntIp = inet_ntoa(sockInfo.addr.sin_addr);
        clntPort = dataInfo.port;

        modTime = dataInfo.modTime;
        localtime_r(&modTime, &localTime);
        strftime(timeBuf, 80, "%Y-%m-%d %H:%M:%S", &localTime);

        InsertRow(path, timeBuf, clntIp, clntPort);
        */
    }

    memset(m_recv_buf, 0, sizeof(m_recv_buf));
}


void Server::SendList(SOCK_INFO& sockInfo)
{
    bool sendCheck = false;
    static int iCnt = 0;

    memset(m_send_buf, 0, sizeof(m_send_buf));

    MYSQL_ROW row;
    int num_fields = mysql_num_fields(m_mysqlResult);
    while (row = mysql_fetch_row(m_mysqlResult)) 
    {
        printf("\t%s %s %s \n", row[2], row[3], row[0]);

        strcpy(m_send_buf[iCnt].buf, row[0]);
        strcpy(m_send_buf[iCnt].ip, row[2]);
        strcpy(m_send_buf[iCnt].port, row[3]);

        iCnt++;
        if(iCnt == 10)
        {
            writev(sockInfo.sock, m_send_list, iCnt);
            iCnt = 0;

            memset(m_send_buf, 0, sizeof(m_send_buf));
            sendCheck = true;
            continue;
        }

        sendCheck = false;
    }

    if(iCnt != 0 && !sendCheck)
    {
        writev(sockInfo.sock, m_send_list, iCnt);
        send(m_sockInfo.sock, NULL, 0, MSG_OOB);
    }
}







void Server::Initialize()
{
    SetIovBuffer();
}


void* th_Handle_Client(void* arg);
void Server::RunReceive()
{
    while(1)
    {
        SOCK_INFO& clntInfo = AcceptSocket(m_sockInfo);

        pthread_mutex_lock(&m_mutx);
        m_listSockInfo.push_back(&clntInfo);

        pthread_create(&m_thId, NULL, th_Handle_Client, (void*) this);
        pthread_detach(m_thId);
        printf("Connected client IP: %s \n", inet_ntoa(clntInfo.addr.sin_addr));
    }
}

void* th_Handle_Client(void* arg)
{    
    Server* server = (Server*) arg;

    list<SOCK_INFO*>& listSockInfo = server->GetListSockInfo();
    SOCK_INFO sockInfo = *listSockInfo.back();


    server->ReceiveList(sockInfo);
    
    ////////////////////////////////////////////
    // NEED ALGORITHM....
    // ABOUT DIFF
    server->SelectRow(NULL, NULL, NULL);
    ////////////////////////////////////////////

    server->SendList(sockInfo);
    pthread_mutex_t& mutex = server->GetMutex();
    pthread_mutex_unlock(&mutex);

    list<SOCK_INFO*>::iterator iter;
    for(iter = listSockInfo.begin(); iter != listSockInfo.end(); ++iter)
    {
        if((*iter)->sock == sockInfo.sock)
            {
                listSockInfo.erase(iter++);
                break;
            }
    }

    close(sockInfo.sock);

    printf("Disconnected client IP: %s \n", inet_ntoa(sockInfo.addr.sin_addr));
    return NULL;
}









void Server::InitMySQL(char* host, char* user, char* passwd, char* db, char* table)
{
    mysql_init(&m_mysql);
    m_mysqlConnection = mysql_real_connect(&m_mysql, host, user, passwd, db, 0, NULL, 0);
    if(m_mysqlConnection == NULL)
        exit_message("mysql_real_connect() error!");
    else
        puts("mysql connected...");

    CreateTable(table);
}


void Server::CreateTable(char* table)
{
    memset(m_sql, 0, BUF_SIZE);
    sprintf(m_sql, "CREATE TABLE IF NOT EXISTS %s(path VARCHAR(1024), date DATETIME, ip VARCHAR(20), port VARCHAR(10), PRIMARY KEY(path)) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci", table);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : create table");
}


void Server::InsertRow(char* path, char* date, char* ip, char* port)
{
    memset(m_sql, 0, BUF_SIZE);
    sprintf(m_sql, "REPLACE INTO lists VALUES( '%s', '%s', '%s', '%s')", path, date, ip, port);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : insert values");
}


void Server::SelectRow(char* path, char* date, char* ip)
{
    memset(m_sql, 0, BUF_SIZE);

    char sql[BUF_SIZE];
    char tmp[BUF_SIZE];
    memset(sql, 0, BUF_SIZE);
    memset(tmp, 0, BUF_SIZE);

    if(!path && !date && !ip)
        strcpy(m_sql, "SELECT * from lists");
    else
    {
        if(path)
        {
            sprintf(sql, "%spath = '%s' AND ", tmp, path);
            strcpy(tmp, sql);
        }
        if(date)
        {
            sprintf(sql, "%sdate = '%s' AND ", tmp, date);
            strcpy(tmp, sql);
        }
        if(ip)
        {
            sprintf(sql, "%sip = '%s' AND", tmp, ip);
            strcpy(tmp, sql);
        }
        
        int len = strlen(tmp);
        memset(sql, 0, BUF_SIZE);
        strncpy(sql, tmp, len - 4);
        strcpy(tmp, sql);

        sprintf(sql, "WHERE %s", tmp);
        sprintf(m_sql, "SELECT * from lists %s", sql);
    }

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : insert values");

    m_mysqlResult = mysql_store_result(m_mysqlConnection);
    if (m_mysqlResult == NULL)
        exit_message("mysql_store_result() error!");
}
