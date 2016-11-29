#!/bin/bash

files=("Client.cpp" "Server.cpp" "Common.cpp")
for file in "${files[@]}";
do
    cmd=$cmd" "$file
done

g++ -o client mainClient.cpp $cmd -I.-lstdc++ -D_REENTRANT -lpthread
g++ -o server mainServer.cpp $cmd -I.-lstdc++ -D_REENTRANT -lpthread

echo "G++ COMPILED"
# ./server