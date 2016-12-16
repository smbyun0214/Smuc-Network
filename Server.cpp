#include "Server.h"

Server::Server()
{
    pthread_mutex_init(&m_mutx, NULL);
}

Server::~Server()
{
    pthread_mutex_destroy(&m_mutx);
    close(m_sockInfo.sock);
    mysql_close(m_mysql);
    mysql_library_end();
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
        m_recv_list[i].iov_len 		= sizeof(m_recv_buf[i]);

        m_send_list[i].iov_base	    = &m_send_buf[i];
        m_send_list[i].iov_len 		= sizeof(m_send_buf[i]);
    }
}








void Server::ReceiveList(SOCK_INFO& sockInfo)
{
    int iRead;
    CLNT_DATA_INFO dataInfo;

    char* path;
    char* modTime;
    char* clntIp;
    char* clntPort;

    bool init = false;
    int recvCheck = 0;

    while((iRead = readv(sockInfo.sock, m_recv_list, 1)) != 0)
    {
        dataInfo = m_recv_buf[0];
        if(strlen(dataInfo.buf) == 0)
            continue;
        
        // INSERT MYSQL
        path = dataInfo.buf;
        modTime = dataInfo.modTime;
        clntIp = inet_ntoa(sockInfo.addr.sin_addr);
        clntPort = dataInfo.port;

        if(init == false)
        {
            InitColumnTemp(clntIp, clntPort);
            init = true;
        }

        InsertRow(path, modTime, clntIp, clntPort);
    }
    memset(m_recv_buf, 0, sizeof(m_recv_buf));
    SetModifiedColumn();
    SetDeletedColumn();
}


void Server::SendList(SOCK_INFO& sockInfo)
{
    bool sendCheck = false;
    static int iCnt = 0;

    memset(m_send_buf, 0, sizeof(m_send_buf));

    MYSQL_ROW row;
    int num_fields = mysql_num_fields(m_mysqlResult);
    while ((row = mysql_fetch_row(m_mysqlResult)) != 0) 
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
    m_thCnt = 0;
}


void* th_Handle_Client(void* arg);
void* th_Handle_Client_Send(void* arg);
void Server::RunReceive()
{

    // pthread_create(&m_thId, NULL, th_Handle_Client_Send, (void*) this);
    // pthread_detach(m_thId);

    while(1)
    {
        SOCK_INFO& clntInfo = AcceptSocket(m_sockInfo);

        pthread_mutex_lock(&m_mutx);
        m_listSockInfo.push_back(&clntInfo);

        pthread_create(&m_thId, NULL, th_Handle_Client, (void*) this);
        m_thCnt++;
        pthread_detach(m_thId);
        printf("Connected client IP: %s \n", inet_ntoa(clntInfo.addr.sin_addr));
    }
}


void* th_Handle_Client(void* arg)
{    
    Server* server = (Server*) arg;

    list<SOCK_INFO*>& listSockInfo = server->GetListSockInfo();
    SOCK_INFO* sockInfo = listSockInfo.back();

    server->ReceiveList(*sockInfo);

    pthread_mutex_t& mutex = server->GetMutex();
    pthread_mutex_unlock(&mutex);

    list<SOCK_INFO*>::iterator iter;
    for(iter = listSockInfo.begin(); iter != listSockInfo.end(); ++iter)
    {
        if((*iter)->sock == sockInfo->sock)
            {
                listSockInfo.erase(iter++);
                break;
            }
    }

    server->DecrementThreadCount();
    close(sockInfo->sock);
    delete sockInfo;

    printf("Disconnected client IP: %s \n", inet_ntoa(sockInfo->addr.sin_addr));
    return NULL;
}


void* th_Handle_Client_Send(void* arg)
{
    Server *server = (Server*) arg;
    char table[] = "lists";
    int thCnt;

    pthread_mutex_t& mutex = server->GetMutex();

    while(1)
    {
        thCnt = server->GetThreadCount();

        if(thCnt != 0)
        {
            sleep(120);
            continue;
        }

/*
    
    pthread_mutex_lock(&mutex);
        server->SetModifiedList();


    pthread_mutex_unlock(&mutex);
*/


        // server->ReleaseDataList(m_modified_list);
        // server->ReleaseDataList(m_deleted_list);
        // server->InitColumn(table);
    }
}







void Server::InitMySQL(char* host, char* user, char* passwd, char* db, char* table)
{
    m_mysql = mysql_init(NULL);
    m_mysqlConnection = mysql_real_connect(m_mysql, host, user, passwd, db, 0, NULL, 0);
    if(m_mysqlConnection == NULL)
        exit_message("mysql_real_connect() error!");
    else
        puts("mysql connected...");

    CreateTable(table);
}


void Server::CreateTable(char* table)
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "CREATE TABLE IF NOT EXISTS %s(path VARCHAR(1024), date DATETIME, ip VARCHAR(20), port VARCHAR(10), temp TINYINT(2), modified TINYINT(1) NOT NULL DEFAULT FALSE, deleted TINYINT(1) NOT NULL DEFAULT FALSE, PRIMARY KEY(path)) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci", table);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : create table");
}


void Server::InitColumnTemp(char* ip, char* port)
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET temp = %d WHERE ip LIKE '%s' AND port LIKE '%s'", DELETE, ip, port);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
    {
        printf("%s \n", m_sql);
        exit_message("mysql_query error() : init rows temp");
    }

    sleep(0.1);
}


void Server::InitColumn(char* table)
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE %s SET temp = NULL, modified = FALSE, deleted = FALSE", table);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
    {
        printf("%s \n", m_sql);
        exit_message("mysql_query error() : init column");    
    }
}


void Server::InsertRow(char* path, char* date, char* ip, char* port)
{
    // Check file modified time
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "SELECT IF((SELECT NOT EXISTS(SELECT * FROM lists WHERE path LIKE '%s')), 'ADD', (SELECT IF('%s' > (SELECT date FROM lists WHERE path LIKE '%s'), 'MODIFY', (SELECT IF('%s' = (SELECT date FROM lists WHERE path LIKE '%s'), 'NONE', 'NULL')))))", path, date, path, date, path);

    int state = mysql_real_query(m_mysqlConnection, m_sql, strlen(m_sql));
    if(state != 0)
        exit_message("mysql_query error() : check time");

    m_mysqlResult = mysql_store_result(m_mysqlConnection);
    if (m_mysqlResult == NULL)
    {
        printf("%s \n", m_sql);
        exit_message("mysql_store_result() error! : get time diff");
    }

    MYSQL_ROW row = mysql_fetch_row(m_mysqlResult);
    char* result = row[0];

    mysql_free_result(m_mysqlResult); 
    if(!strcmp(result, "ADD") || !strcmp(result, "MODIFY"))
    {
        memset(m_sql, 0, SQL_SIZE);
        sprintf(m_sql, "REPLACE INTO lists VALUES( '%s', '%s', '%s', '%s', %d, FALSE, FALSE)", path, date, ip, port, MODIFY);

        state = mysql_query(m_mysqlConnection, m_sql);
        if(state != 0)
            exit_message("mysql_query error() : insert ADD");
    }
    else if(!strcmp(result, "NONE"))
    {
        memset(m_sql, 0, SQL_SIZE);
        sprintf(m_sql, "UPDATE lists SET temp = %d WHERE path LIKE '%s'", NONE, path);

        state = mysql_query(m_mysqlConnection, m_sql);
        if(state != 0)
            exit_message("mysql_query error() : insert NONE");
    }
    else if(!strcmp(result, "NULL"))
    {
        // others
    }
}


void Server::SetModifiedColumn()
{
    MODIFIED MOD;
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET modified = TRUE WHERE temp = %d", MODIFY);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : set delete column");
}

void Server::SetDeletedColumn()
{
    MODIFIED MOD;
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET deleted = TRUE WHERE temp = %d", DELETE);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : set delete column");
}

void Server::DeleteRow(char* path, char* ip, char* port)
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "DELETE FROM lists WHERE path LIKE '%s' AND ip LIKE '%s' AND port LIKE '%s'", path, ip, port);

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : delete rows");
}


void Server::SelectRow(char* path, char* date, char* ip, char* modified)
{
    memset(m_sql, 0, SQL_SIZE);

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
            sprintf(sql, "%spath LIKE '%s' AND ", tmp, path);
            strcpy(tmp, sql);
        }
        if(date)
        {
            sprintf(sql, "%sdate LIKE '%s' AND ", tmp, date);
            strcpy(tmp, sql);
        }
        if(ip)
        {
            sprintf(sql, "%sip LIKE '%s' AND", tmp, ip);
            strcpy(tmp, sql);
        }
        if(modified)
        {
            sprintf(sql, "%smodified LIKE '%s' AND", tmp, modified);
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


void Server::SetModifiedList()
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "SELECT path, date, ip, port FROM lists WHERE modified = TRUE and deleted = FALSE");

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : insert values");

    m_mysqlResult = mysql_store_result(m_mysqlConnection);
    if (m_mysqlResult == NULL)
        exit_message("mysql_store_result() error!");

    MYSQL_ROW row;
    int num_fields = mysql_num_fields(m_mysqlResult);
    while ((row = mysql_fetch_row(m_mysqlResult)) != 0) 
    {
        CLNT_DATA_INFO* clntInfo = new CLNT_DATA_INFO();

        memset(clntInfo, 0, sizeof(CLNT_DATA_INFO));
        strcpy(clntInfo->buf, row[0]);
        strcpy(clntInfo->modTime, row[1]);
        strcpy(clntInfo->ip, row[2]);
        strcpy(clntInfo->port, row[3]);

        m_modified_list.push_back(clntInfo);
    }
    mysql_free_result(m_mysqlResult);
}


void Server::SetDeletedList()
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "SELECT path, date, ip, port FROM lists WHERE deleted = TRUE");

    int state = mysql_query(m_mysqlConnection, m_sql);
    if(state != 0)
        exit_message("mysql_query error() : insert values");

    m_mysqlResult = mysql_store_result(m_mysqlConnection);
    if (m_mysqlResult == NULL)
        exit_message("mysql_store_result() error!");

    MYSQL_ROW row;
    int num_fields = mysql_num_fields(m_mysqlResult);
    while ((row = mysql_fetch_row(m_mysqlResult)) != 0) 
    {
        CLNT_DATA_INFO* clntInfo = new CLNT_DATA_INFO();

        memset(clntInfo, 0, sizeof(CLNT_DATA_INFO));
        strcpy(clntInfo->buf, row[0]);
        strcpy(clntInfo->modTime, row[1]);
        strcpy(clntInfo->ip, row[2]);
        strcpy(clntInfo->port, row[3]);

        m_deleted_list.push_back(clntInfo);
    }
    mysql_free_result(m_mysqlResult);
}


void Server::ReleaseDataList(list<CLNT_DATA_INFO*>& list_data)
{
    list<CLNT_DATA_INFO*>::iterator iter;
    for(iter = list_data.begin(); iter != list_data.end(); ++iter)
        delete *iter;
    
    list_data.clear();
}

