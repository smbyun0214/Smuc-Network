#pragma once
#define IOV_LIST_CNT		10
#define BUF_SIZE			1024
#define IP_SIZE             20
#define PORT_SIZE           10


struct sock_info{
    int                sock;
    struct sockaddr_in addr;

    sock_info operator=(sock_info& ref)
    {
    	sock = ref.sock;
    	memcpy(&addr, &ref.addr, sizeof(struct sockaddr_in));
       	printf("sock info operator called\n");
    };

};

struct clnt_data_info
{
    char        buf[BUF_SIZE];
    time_t      modTime;
    char        ip[IP_SIZE];
    char        port[PORT_SIZE];

    clnt_data_info operator=(clnt_data_info& ref)
    {
    	strcpy(buf, ref.buf);
    	modTime = ref.modTime;
    	strcpy(ip, ref.ip);
    	strcpy(port, ref.port);
    	printf("data info operator called\n");
    }

};

typedef struct sock_info SOCK_INFO;
typedef struct clnt_data_info CLNT_DATA_INFO;
