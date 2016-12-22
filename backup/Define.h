#pragma once
#define IOV_LIST_CNT		10
#define BUF_SIZE			1024
#define SQL_SIZE            2048
#define IP_SIZE             20
#define PORT_SIZE           10
#define TIME_SIZE           30


struct sock_info{
    int                sock;
    struct sockaddr_in addr;

    sock_info operator=(sock_info& ref)
    {
    	sock = ref.sock;
    	memcpy(&addr, &ref.addr, sizeof(struct sockaddr_in));

        return *this;
    };

};

struct clnt_data_info
{
    char        buf[BUF_SIZE];
    char        modTime[TIME_SIZE];
    char        ip[IP_SIZE];
    char        port[PORT_SIZE];

    clnt_data_info operator=(clnt_data_info& ref)
    {
    	strcpy(buf, ref.buf);
        strcpy(modTime, ref.modTime);
    	strcpy(ip, ref.ip);
    	strcpy(port, ref.port);

        return *this;
    }

};

typedef struct sock_info SOCK_INFO;
typedef struct clnt_data_info CLNT_DATA_INFO;

enum MODIFIED {DELETE = -1, NONE = 0, MODIFY = 1} ;