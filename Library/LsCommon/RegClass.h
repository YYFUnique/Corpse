#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif
/********************************************************************
*　 文件名：RegOperator.h
*　 文件描述：对注册表的常见操作进行封装
*　 创建人： RainLeaf, 2009年4月10日
*　 版本号：1.0
*　 修改记录：
*********************************************************************/

class LS_COMMON_API CRegClass
{
private:
	HKEY m_hKeyRoot;
	HKEY m_hKey;													/* 保存当前操作根键的句柄						*/
public:
	CRegClass(HKEY hKey = HKEY_LOCAL_MACHINE);					/* 构造函数，默认参数为：HKEY_LOCAL_MACHINE		*/
	~CRegClass();
public:
	void SetHKEY(HKEY hKeyRoot);									/* 设置当前操作的根键							*/
	BOOL OpenKey(LPCTSTR lpSubKey);								/* 读写的形式打开注册表							*/
	BOOL OpenKey(LPCTSTR lpSubKey,DWORD dwExFlag);
	BOOL IsRegOpen();
	void Close();													/* 关闭键句柄									*/

	BOOL CreateKey( LPCTSTR lpSubKey );								/* 创建的形式打开注册表							*/
	BOOL CreateKey( LPCTSTR lpSubKey ,DWORD dwExFlag);
	BOOL DeleteKey(LPCTSTR lpSubKey );				/* 删除相应的子键（子键为空）					*/
	BOOL DeleteValue(LPCTSTR lpKeyName);						/* 删除子键处的相应的键值						*/	
	BOOL SaveKey(LPCTSTR lpFileName);								/* 把当前键值保存到指定文件						*/
	BOOL RestoreKey(LPCTSTR lpFileName);							/* 从指定注册表文件中恢复						*/

	BOOL ReadKey(LPCTSTR lpKeyName,CString& strKeyValue );				/* 读出REG_SZ类型的值							*/
	BOOL ReadKey(LPCTSTR lpKeyName,DWORD& dwVal);				/* 读出DWORD类型的值							*/
	BOOL ReadKey(LPCTSTR lpKeyName,LPBYTE lpByte,DWORD& dwSize);
	BOOL WriteKey(LPCTSTR lpSubKey,LPCTSTR lpVal );					/* 写入REG_SZ类型值								*/
	BOOL WriteKey(LPCTSTR lpSubKey,DWORD dwVal );					/* 写入DWORD类型值								*/
};
