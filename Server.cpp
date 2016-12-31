#include "Server.h"

Server::Server(string& path)
{
    env_path = path;
}

Server::~Server()
{
    pthread_mutex_destroy(&m_mutx);
}

void Server::LoadEnvironment()
{
    ifstream file;
    file.open(env_path);
    file >> env;
    file.close();
}

void Server::Initialize()
{
    LoadEnvironment();
    InitializeDatabase();
    InitializeServer();

    pthread_mutex_init(&m_mutx, NULL);
}

void Server::InitializeDatabase()
{
    string strHost = env["mysql"]["host"];
    string strUser = env["mysql"]["user"];
    string strPassword = env["mysql"]["password"];
    string strDB = env["mysql"]["database"];
    string strLists = env["mysql"]["table_lists"];
    string strUsers = env["mysql"]["table_users"];

    char* host = StringToCharPointer(strHost);
    char* user = StringToCharPointer(strUser);
    char* password = StringToCharPointer(strPassword);

    char* db = StringToCharPointer(strDB);
    char* lists = StringToCharPointer(strLists);
    char* users = StringToCharPointer(strUsers);

    char attr[][SQL_SIZE] = {
        "ip VARCHAR(20)",
        "port VARCHAR(10)",
        "path VARCHAR(1024)",
        "date DATETIME",
        "temp TINYINT(2)",
        "modified TINYINT(1) NOT NULL DEFAULT FALSE",
        "deleted TINYINT(1) NOT NULL DEFAULT FALSE",
        "PRIMARY KEY(path)"
    };

    m_mysql.Initialize(host, user, password);
    m_mysql.CreateDatabase(db);
    m_mysql.CreateTable(lists, attr, sizeof(attr)/SQL_SIZE);
    // m_mysql.CreateTable(users, attr, sizeof(attr)/SQL_SIZE);

    CharPointerToDelete(host);
    CharPointerToDelete(user);
    CharPointerToDelete(password);
    CharPointerToDelete(db);
    CharPointerToDelete(lists);
    CharPointerToDelete(users);
}



void Server::InitializeServer()
{
    string strPort = env["server"]["port"];

    char* port = StringToCharPointer(strPort);

    m_server.Initialize(NULL, port);

    CharPointerToDelete(port);
}

void Server::SetIov(struct iovec (&iov)[IOV_LIST_CNT], DATA_INFO& data)
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

void Server::ReceiveList(int& sock, struct sockaddr_in& addr)
{
    bool init;
    int iRead;
    DATA_INFO dataInfo;

    string strLists = env["mysql"]["table_lists"];
    char* lists = StringToCharPointer(strLists);

    char flag[1];
    char row[7][SQL_SIZE];

    init = false;

    memset(&m_send_info, 0, sizeof(m_send_info));
    SetIov(m_recv_list, m_recv_info);

    while((iRead = readv(sock, m_recv_list, IOV_LIST_CNT)) != 0)
    {
        if(strlen(m_recv_info.buf) == 0)
            continue;

        if(init == false)
        {
            UpdateSetInit(m_recv_info.ip, m_recv_info.port);
            init = true;
        }

        sprintf(row[0], "\"%s\"", inet_ntoa(addr.sin_addr));
        sprintf(row[1], "\"%s\"", m_recv_info.port);
        sprintf(row[2], "\"%s\"", m_recv_info.buf);
        sprintf(row[3], "\"%s\"", m_recv_info.modTime);
        sprintf(row[4], "%d", MODIFY);
        sprintf(row[5], "%s", "FALSE");
        sprintf(row[6], "%s", "FALSE");

        m_mysql.ReplaceRow(lists, row, sizeof(row)/SQL_SIZE);

        cout << "\t" << inet_ntoa(addr.sin_addr) << "[" << m_recv_info.port << "]" << " >> " << m_recv_info.buf << endl;
    }

    UpdateSetModified();
    UpdateSetDeleted();

    CharPointerToDelete(lists);
}

void th_receive(void* arg, int* sock, struct sockaddr_in* addr)
{
    Server* server = (Server*) arg;
    server->ReceiveList(*sock, *addr);
    pthread_mutex_unlock(server->GetPMutex());
}

void Server::RunReceive()
{
    int client_sock;  
    struct sockaddr_in client_addr;

    while(1)
    {
        m_server.Accept(client_sock, client_addr);

        pthread_mutex_lock(&m_mutx);
        thread th(&th_receive, this, &client_sock, &client_addr);
        th.detach();
    }
}

void Server::UpdateSetInit(char (&ip)[IP_SIZE], char (&port)[PORT_SIZE])
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET temp = %d WHERE ip LIKE '%s' AND port LIKE '%s'", DELETE, ip, port);

    int state = mysql_query(m_mysql.GetConnection(), m_sql);
    if(state != 0)
    {
        printf("%s \n", m_sql);
        exit_message("mysql_query error() : init rows temp");
    }
}

void Server::UpdateSetModified()
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET modified = TRUE WHERE temp = %d", MODIFY);

    int state = mysql_query(m_mysql.GetConnection(), m_sql);
    if(state != 0)
        exit_message("mysql_query error() : set delete column");
}

void Server::UpdateSetDeleted()
{
    memset(m_sql, 0, SQL_SIZE);
    sprintf(m_sql, "UPDATE lists SET deleted = TRUE WHERE temp = %d", DELETE);

    int state = mysql_query(m_mysql.GetConnection(), m_sql);
    if(state != 0)
        exit_message("mysql_query error() : set delete column");

}