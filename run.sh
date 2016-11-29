#!/bin/bash

files=("Client.cpp" "Server.cpp" "Common.cpp")
for file in "${files[@]}";
do
    cmd=$cmd" "$file
done

g++ -o client mainClient.cpp $cmd -I.-lstdc++ -D_REENTRANT -lpthread -I/usr/local/Cellar/mysql/5.7.16/include/mysql -L/usr/local/Cellar/mysql/5.7.16/lib -lmysqlclient -lssl -lcrypto
g++ -o server mainServer.cpp $cmd -I.-lstdc++ -D_REENTRANT -lpthread -I/usr/local/Cellar/mysql/5.7.16/include/mysql -L/usr/local/Cellar/mysql/5.7.16/lib -lmysqlclient -lssl -lcrypto

echo "G++ COMPILED"
# ./server