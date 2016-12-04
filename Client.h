#include "Header.h"
#include "Define.h"

class Client
{
    private:
        SOCK_INFO                   m_sockInfo;
        SOCK_INFO                   m_mySockInfo;
        char                        m_port[PORT_SIZE];

        char                        m_shared[BUF_SIZE];

        struct iovec                m_send_list[IOV_LIST_CNT];
        CLNT_DATA_INFO              m_send_buf[IOV_LIST_CNT];

        struct iovec                m_recv_list[IOV_LIST_CNT];
        CLNT_DATA_INFO              m_recv_buf[IOV_LIST_CNT];

        pthread_t                   m_thId;
        pthread_mutex_t             m_mutx;

        list<SOCK_INFO*>            m_listSockInfo;
        list<SOCK_INFO*>::iterator  m_iter;

    public:
        Client();
        ~Client();

    public:
        void InitSock(SOCK_INFO& sockInfo, char *ip, char *port);
        void SetAddr(struct sockaddr_in& addr, char* ip, char* port);
        void SetPort(char *port);

        void InitializeClient(char *ip, char *port, char* folder);
        void InitializeServer(char *port);


    public:
        void AskSocket(SOCK_INFO& sockInfo);
        SOCK_INFO* AcceptSocket(SOCK_INFO& sockInfo);

    public:
        void SetSharedFolder(char* folder);
        void SetIovBuffer();

        
    public:
        void SendList();
        void ExploreDirectory(char *path);
        void _SendList(char *path, time_t modTime, bool flag);

        void ReceiveList();

        void RunSendList();

    public:
        void ReceivePath();



    public:
        SOCK_INFO& GetSockInfo()
        {
            return m_sockInfo;
        }

        SOCK_INFO& GetMySockInfo()
        {
            return m_mySockInfo;
        }

        CLNT_DATA_INFO* GetRecvBuf()
        {
            return m_recv_buf;
        }



        list<SOCK_INFO*>& GetListSockInfo()
        {
            return m_listSockInfo;
        }
        
        pthread_mutex_t& GetMutex()
        {
            return m_mutx;
        }
};