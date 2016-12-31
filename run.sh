mysql_libs=$(mysql_config --libs)
mysql_include=$(mysql_config --include)

FOLDER=("." "mMySQL" "mSock" "json-develop/src")

cpp=()
for folder in ${FOLDER[@]}
do

	if [ $folder == "backup" ]; then
		continue
	fi

	for file in $folder/*
	do
		if [[ "${file%.*}" == *"main"* ]]; then
			continue
		fi

		if [ "${file##*.}" == "cpp" ]; then
			cpp+=($file)
		fi
	done

done

header=(${FOLDER[@]:1:${#FOLDER[@]}})
header=("${header[@]/#/-I./}")

# -std=c++11
g++ -o client mainclient.cpp ${cpp[@]} ${header[@]} $mysql_libs $mysql_include -std=c++11 -D_REENTRANT -lpthread
g++ -o server mainserver.cpp ${cpp[@]} ${header[@]} $mysql_libs $mysql_include -std=c++11 -D_REENTRANT -lpthread
echo "-------------------------------------------"
# ./test