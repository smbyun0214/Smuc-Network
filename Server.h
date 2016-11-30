#include "Header.h"
#include "Define.h"

class Server
{
    private:
        SOCK_INFO   m_sockInfo;

        list<SOCK_INFO*> m_listSockInfo;
        list<SOCK_INFO*>::iterator m_iter;


        MYSQL       m_mysql;
        MYSQL*      m_mysqlConnection;
        MYSQL_RES*  m_mysqlResult;
        char        m_sql[BUF_SIZE];



        struct iovec            m_recv_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_recv_buf[IOV_LIST_CNT];
        
        struct iovec            m_send_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_send_buf[IOV_LIST_CNT];


        pthread_t               m_thId;
        pthread_mutex_t         m_mutx;

    public:
        Server();
        ~Server();

    public:
        void InitSock(char *port);
        void CreateSocket(SOCK_INFO& sockInfo, char *port);
        SOCK_INFO& AcceptSocket(SOCK_INFO& sockInfo);


        void SetIovBuffer();
    
    public:
        void Initialize();
        void RunReceive();



    public:
        void ReceiveList(SOCK_INFO& sockInfo);
        void SendList(SOCK_INFO& sockInfo);

    
    public:
        SOCK_INFO& GetSockInfo();
        list<SOCK_INFO*>& GetListSockInfo()
        {
            return m_listSockInfo;
        }



    public:
        void InitMySQL(char* host, char* user, char* passwd, char* db, char* table);
        void CreateTable(char* table);
        void InsertRow(char* path, char* date, char* ip, char* port);
        void SelectRow();
        void SelectRow(char* path, char* date, char* ip);

        // void* th_Handle_Client(void* arg);
};