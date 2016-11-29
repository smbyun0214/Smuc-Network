#include "Server.h"

Server::Server(char *port)
{

}

Server::~Server()
{
    close(m_sockInfo.sock);
}

void Server::InitSock(SOCK_INFO& sockInfo, char *port)
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


SOCK_INFO& Server::AcceptSocket(SOCK_INFO& sockInfo, char* port)
{
    SOCK_INFO* clnt_sockInfo = new SOCK_INFO();
    socklen_t clnt_addr_sz = sizeof(clnt_sockInfo->addr);
    clnt_sockInfo->sock = accept(m_sockInfo.sock, (struct sockaddr*) &clnt_sockInfo->addr, &clnt_addr_sz);
    if(clnt_sockInfo->sock == -1)
        exit_message("socket() error!");

    return *clnt_sockInfo;
}


void Server::SetIovBuffer()
{
    for(int i = 0; i < IOV_LIST_CNT; i++)
    {
        m_recv_list[i].iov_base	    = &m_recv_buf[i];
        m_recv_list[i].iov_len 		= sizeof(m_recv_buf);
    }
}



void Server::ReceiveList(SOCK_INFO& sockInfo)
{
    int iRead;
    int len;
    CLNT_DATA_INFO dataInfo;

    
    while((iRead = readv(sockInfo.sock, m_recv_list, 1) != 0))
    {
        dataInfo = m_recv_buf[0];

        printf("%s, %s \n", inet_ntoa(dataInfo.addr.sin_addr), dataInfo.buf);
    }
}













SOCK_INFO& Server::GetSockInfo()
{
    return m_sockInfo;
}