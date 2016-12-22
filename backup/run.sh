#!/bin/bash

files=("Client.cpp" "Server.cpp" "Common.cpp")
for file in "${files[@]}";
do
    cmd=$cmd" "$file
done
libs=$(mysql_config --libs)
include=$(mysql_config --include)

# g++ -o client mainClient.cpp $cmd -I. -D_REENTRANT -lpthread -I/usr/local/Cellar/mysql/5.7.16/include/mysql -L/usr/local/Cellar/mysql/5.7.16/lib -lmysqlclient -lssl -lcrypto
# g++ -o server mainServer.cpp $cmd -I. -D_REENTRANT -lpthread -I/usr/local/Cellar/mysql/5.7.16/include/mysql -L/usr/local/Cellar/mysql/5.7.16/lib -lmysqlclient -lssl -lcrypto
# -lstdc++ 

g++ -o client mainClient.cpp $cmd -I. -D_REENTRANT -lpthread $libs $include
g++ -o Client_test/client_tmp mainClient1.cpp $cmd -I. -D_REENTRANT -lpthread $libs $include
g++ -o server mainServer.cpp $cmd -I. -D_REENTRANT -lpthread $libs $include
echo "G++ COMPILED"
#./server 9199
