#include "mMySQL.h"

void mMySQL::Initialize(char host[], char user[], char password[])
{
	mysql = mysql_init(NULL);
	connection = mysql_real_connect(mysql, host, user, password, NULL, 0, NULL, 0);

	if(connection == NULL)
	{
		perror("mysql_real_connect");
		exit(1);
	}
	else
		puts("mysql connected...");
}

void mMySQL::CreateDatabase(char database[])
{
	memset(sql, 0, SQL_SIZE);
	sprintf(sql, "CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARACTER SET=utf8", database);

	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[create database]");
		exit(1);
	}

	memset(sql, 0, SQL_SIZE);
	sprintf(sql, "USE %s", database);

	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[use database]");
		exit(1);
	}
}

void mMySQL::CreateTable(char table[], char attr[][SQL_SIZE], int attr_len)
{
	int i;
	char tmpSql[SQL_SIZE];
	char attrSql[SQL_SIZE];

	memset(sql, 0, SQL_SIZE);
	memset(tmpSql, 0, SQL_SIZE);
	memset(attrSql, 0, SQL_SIZE);

	for(i = 0; i < attr_len; i++)
	{
		strcat(tmpSql, attr[i]);
		strcat(tmpSql, ", ");
	}

	strncpy(attrSql, tmpSql, strlen(tmpSql) - 2);
	sprintf(sql, "CREATE TABLE IF NOT EXISTS %s(%s) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci", table, attrSql);

	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[create table]");
		exit(1);
	}
}

void mMySQL::InsertRow(char table[], char row[][SQL_SIZE], int row_len)
{
	int i;
	char tmpSql[SQL_SIZE];
	char rowSql[SQL_SIZE];

	memset(sql, 0, SQL_SIZE);
	memset(tmpSql, 0, SQL_SIZE);
	memset(rowSql, 0, SQL_SIZE);

	for(i = 0; i < row_len; i++)
	{
		strcat(tmpSql, row[i]);
		strcat(tmpSql, ", ");
	}

	strncpy(rowSql, tmpSql, strlen(tmpSql) - 2);
	sprintf(sql, "INSERT INTO %s VALUES(%s)", table, rowSql);
	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[insert row]");
		exit(1);
	}
}

MYSQL_RES* mMySQL::SelectRow(char table[], char where[][SQL_SIZE], int where_len)
{
	int i;
	char tmpSql[SQL_SIZE];
	char whereSql[SQL_SIZE];

	memset(sql, 0, SQL_SIZE);
	memset(tmpSql, 0, SQL_SIZE);
	memset(whereSql, 0, SQL_SIZE);

	for(i = 0; i < where_len; i++)
	{
		strcat(tmpSql, where[i]);
		strcat(tmpSql, " AND ");
	}

	strncpy(whereSql, tmpSql, strlen(tmpSql) - 5);
	sprintf(sql, "SELECT * FROM %s WHERE %s", table, whereSql);

	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[select row]");
		exit(1);
	}

    result = mysql_store_result(connection);
    if (result == NULL)
    {
    	perror("mysql_store_result[select row]");
    	exit(1);
    }
}

void mMySQL::DeleteRow(char table[], char where[][SQL_SIZE], int where_len)
{
	int i;
	char tmpSql[SQL_SIZE];
	char whereSql[SQL_SIZE];

	memset(sql, 0, SQL_SIZE);
	memset(tmpSql, 0, SQL_SIZE);
	memset(whereSql, 0, SQL_SIZE);

	for(i = 0; i < where_len; i++)
	{
		strcat(tmpSql, where[i]);
		strcat(tmpSql, " AND ");
	}

	strncpy(whereSql, tmpSql, strlen(tmpSql) - 5);
	sprintf(sql, "DELETE FROM %s WHERE %s", table, whereSql);

	state = mysql_query(connection, sql);
	if(state != 0)
	{
		perror("mysql_query[delete row]");
		exit(1);
	}
}

void mMySQL::ShowResult()
{
	MYSQL_ROW row;
    int num_fields = mysql_num_fields(result);
    while ((row = mysql_fetch_row(result)) != 0) 
    {
    	for(int i = 0; i < num_fields; i++)
    		printf("%s ", row[i]);
    	printf("\n");
    }
    mysql_free_result(result);
}