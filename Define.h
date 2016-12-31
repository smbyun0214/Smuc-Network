#pragma once

#include "Header.h"

inline char* StringToCharPointer(std::string& str)
{
    char* pChar = new char[str.size() + 1];
    copy(str.begin(), str.end(), pChar);
    pChar[str.size()] = '\0';

    return pChar;
}

inline void CharPointerToDelete(char* pChar)
{
    delete[] pChar;
}

inline void exit_message(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#define IOV_LIST_CNT		4
#define IP_SIZE             20
#define PORT_SIZE           10
#define BUF_SIZE			1024
#define TIME_SIZE           30

typedef struct data_info
{
    char        ip[IP_SIZE];
    char        port[PORT_SIZE];
    char        buf[BUF_SIZE];
    char        modTime[TIME_SIZE];

    data_info operator=(data_info& ref)
    {
        strcpy(ip, ref.ip);
        strcpy(port, ref.port);
    	strcpy(buf, ref.buf);
        strcpy(modTime, ref.modTime);

        return *this;
    }
    
} DATA_INFO;


// typedef struct sock_info
// {
//     int sock;  
//     struct sockaddr_in addr;
//     void* object;
// } SOCK_INFO;

