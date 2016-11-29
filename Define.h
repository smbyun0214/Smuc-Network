#pragma once
#define IOV_LIST_CNT		10
#define BUF_SIZE			1024
#define IP_SIZE             20
#define PORT_SIZE           10


typedef struct{
    int                sock;
    struct sockaddr_in addr;
}   SOCK_INFO;


typedef struct 
{
    char        buf[BUF_SIZE];
    time_t      modTime;
    char        ip[IP_SIZE];
    char        port[PORT_SIZE];
}   CLNT_DATA_INFO;
