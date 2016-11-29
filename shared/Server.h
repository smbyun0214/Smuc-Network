#include "Header.h"
#include "Define.h"

class Server
{
    private:
        SOCK_INFO m_sockInfo;


        struct iovec            m_recv_list[IOV_LIST_CNT];
        CLNT_DATA_INFO          m_recv_buf[IOV_LIST_CNT];

    public:
        Server(char *port);
        ~Server();

    public:
        void InitSock(SOCK_INFO& sockInfo, char *port);
        SOCK_INFO& AcceptSocket(SOCK_INFO& sockInfo, char* port);


        void SetIovBuffer();

    public:
        void ReceiveList();

    
    public:
        SOCK_INFO& GetSockInfo();

};