#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <my_global.h>
#include <mysql.h>

#include "Common.h"
#include "Define.h"

using namespace std;




#define __DEBUG__


#ifdef __DEBUG__
#define ddb printf("!!!!!!!!!!!! CONFIRM !!!!!!!!!!!!! \n");




#endif