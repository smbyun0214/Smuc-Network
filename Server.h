#include "Header.h"
#include "Define.h"

class Server
{
    private:
        SOCK_INFO   m_sockInfo;

        MYSQL*      m_mysql;
        MYSQL*      m_mysqlConnection;
        MYSQL_RES*  m_mysqlResult;
        char        m_sql[SQL_SIZE];



        struct iovec            m_recv_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_recv_buf[IOV_LIST_CNT];
        
        struct iovec            m_send_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_send_buf[IOV_LIST_CNT];


        pthread_t               m_thId;
        pthread_mutex_t         m_mutx;
        int                     m_thCnt;

        list<SOCK_INFO*> m_listSockInfo;

        list<CLNT_DATA_INFO*>   m_modified_list;
        list<CLNT_DATA_INFO*>   m_deleted_list;


    public:
        Server();
        ~Server();

    public:
        void InitSock(char *port);
        void CreateSocket(SOCK_INFO& sockInfo, char *port);
        SOCK_INFO& AcceptSocket(SOCK_INFO& sockInfo);


        void SetIovBuffer();

    public:
        void ReceiveList(SOCK_INFO& sockInfo);
        void SendList(SOCK_INFO& sockInfo);
    

    public:
        void Initialize();
        void RunReceive();


    
    public:
        SOCK_INFO& GetSockInfo()
        {
            return m_sockInfo;
        }

        list<SOCK_INFO*>& GetListSockInfo()
        {
            return m_listSockInfo;
        }
        
        pthread_mutex_t& GetMutex()
        {
            return m_mutx;
        }
        MYSQL_RES* GetMySQLResult()
        {
            return m_mysqlResult;
        }
        void DecrementThreadCount()
        {
            m_thCnt--;
        }
        int GetThreadCount()
        {
            return m_thCnt;
        }


    public:
        void InitMySQL(char* host, char* user, char* passwd, char* db, char* table);
        void CreateTableUser();
        void CreateTable(char* table);
        
        void InitColumnTemp(char* ip, char* port);
        void InitColumn(char* table);
        
        void InsertRow(char* path, char* date, char* ip, char* port);
        
        void SetModifiedColumn();
        void SetDeletedColumn();
        
        void DeleteRow(char* path, char* ip, char* port);
        void SelectRow(char* path = NULL, char* date = NULL, char* ip = NULL, char* modified = NULL);

    public:
        void SetModifiedList();
        void SetDeletedList();
        void ReleaseDataList(list<CLNT_DATA_INFO*>& list_data);

};
