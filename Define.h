#pragma once
#define IOV_LIST_CNT		10
#define BUF_SIZE			1024
#define PORT_SIZE           10


typedef struct{
    int                sock;
    struct sockaddr_in addr;
}   SOCK_INFO;


typedef struct 
{
    struct sockaddr_in addr;
    char        buf[BUF_SIZE];
}   CLNT_DATA_INFO;