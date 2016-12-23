#include <my_global.h>
#include <mysql.h>

#define SQL_SIZE 1024

class mMySQL
{
private:
	MYSQL*      mysql;
    MYSQL*      connection;
    MYSQL_RES*  result;

    int 		state;
    char		sql[SQL_SIZE];

public:
	void Initialize(char host[], char user[], char password[]);
	void CreateDatabase(char database[]);
	void CreateTable(char table[], char attr[][SQL_SIZE], int attr_len);
	void InsertRow(char table[], char row[][SQL_SIZE], int row_len);
	MYSQL_RES* SelectRow(char table[], char where[][SQL_SIZE], int where_len);
	void DeleteRow(char table[], char where[][SQL_SIZE], int where_len);
	void ShowResult();
};