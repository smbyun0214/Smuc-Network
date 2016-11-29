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
        void _SendList(char *path, bool flag);
        


    public:
        SOCK_INFO& GetSockInfo();
};