#include "Header.h"
#include "Define.h"

class Client
{
    private:
        SOCK_INFO   m_sockInfo;

        SOCK_INFO   m_mySockInfo;
        char        m_port[PORT_SIZE];

        char        m_shared[BUF_SIZE];

        struct iovec            m_send_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_send_buf[IOV_LIST_CNT];

        struct iovec            m_recv_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_recv_buf[IOV_LIST_CNT];


        pthread_t               m_thId;
        pthread_mutex_t         m_mutx;

    public:
        Client();
        ~Client();

    public:
        void InitSock(SOCK_INFO& sockInfo, char *ip, char *port);
        void SetAddr(struct sockaddr_in& addr, char* ip, char* port);

        void InitializeClient(char *ip, char *port, char* folder);
        void InitializeServer(char *port);



        void AskSocket(SOCK_INFO& sockInfo, char *ip, char *port);

        void SetSharedFolder(char* folder);
        void SetIovBuffer();
        void SetPort(char *port);

        
    public:
        void SendList();
        void ExploreDirectory(char *path);
        void _SendList(char *path, time_t modTime, bool flag);

        void ReceiveList();
        


    public:
        void* thTransClient(SOCK_INFO* sock, char* path)

    public:
        SOCK_INFO& GetSockInfo();
};