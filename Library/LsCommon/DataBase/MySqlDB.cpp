#include "stdafx.h"
#include "MySqlDB.h"

#pragma comment(lib,"libmysql.lib")


CMySqlDB::CMySqlDB()
:m_nErrorNum(0)
,m_strErrorInfo(_T(""))
{
	mysql_library_init(0,NULL,NULL);
	mysql_init(&m_ObjSql);
	mysql_options(&m_ObjSql,MYSQL_SET_CHARSET_NAME,_T("GBK"));
}

CMySqlDB::~CMySqlDB()
{

}

BOOL CMySqlDB::ConnectMySQL(LPCTSTR lpszServer, LPCTSTR lpszUserName, LPCTSTR lpszPassword, LPCTSTR lpszDataBase, int nPort)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (mysql_real_connect(&m_ObjSql,lpszServer,lpszUserName,lpszPassword,lpszDataBase,nPort,0,0) != NULL)
			bSuccess = TRUE;
		else
			ErrorIntoMySQL();
	} while (FALSE);

	return bSuccess;
}

BOOL CMySqlDB::CreateDatabase(LPCTSTR lpszDatabaseName)
{
	CString strQuery(_T("CREATE DATABASE IF NOT EXISTS"));
	strQuery += lpszDatabaseName;

	if (mysql_query(&m_ObjSql,strQuery) == 0)
}