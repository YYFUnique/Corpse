#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

/*#include "mysql.h"*/

class LS_COMMON_API CMySqlDB
{
public:
	CMySqlDB();
	~CMySqlDB();

public:
	BOOL ConnectMySQL(LPCTSTR lpszServer,LPCTSTR lpszUserName,LPCTSTR lpszPassword,LPCTSTR lpszDataBase,int nPort);
	BOOL CreateDatabase(LPCTSTR lpszDatabaseName);
	BOOL CreatedbTable(LPCTSTR lpszTableName);

	void ErrorIntoMySQL();
	BOOL WriteDataToDB(LPCTSTR lpszQuery);
	BOOL GetDataFromDB(LPCTSTR lpszQuery,std::vector<std::vector<std::string>>& data);
	void CloseMySQL();

protected:
	MYSQL m_ObjSql;
	MYSQL_RES* m_pRes;
	int		 m_nErrorNum;					//MySQL´íÎóºÅ
	CString m_strErrorInfo;					//´íÎóÄÚÈÝ
};