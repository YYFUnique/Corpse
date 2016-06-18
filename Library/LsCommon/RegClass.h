#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif
/********************************************************************
*�� �ļ�����RegOperator.h
*�� �ļ���������ע���ĳ����������з�װ
*�� �����ˣ� RainLeaf, 2009��4��10��
*�� �汾�ţ�1.0
*�� �޸ļ�¼��
*********************************************************************/

class LS_COMMON_API CRegClass
{
private:
	HKEY m_hKeyRoot;
	HKEY m_hKey;													/* ���浱ǰ���������ľ��						*/
public:
	CRegClass(HKEY hKey = HKEY_LOCAL_MACHINE);					/* ���캯����Ĭ�ϲ���Ϊ��HKEY_LOCAL_MACHINE		*/
	~CRegClass();
public:
	void SetHKEY(HKEY hKeyRoot);									/* ���õ�ǰ�����ĸ���							*/
	BOOL OpenKey(LPCTSTR lpSubKey);								/* ��д����ʽ��ע���							*/
	BOOL OpenKey(LPCTSTR lpSubKey,DWORD dwExFlag);
	BOOL IsRegOpen();
	void Close();													/* �رռ����									*/

	BOOL CreateKey( LPCTSTR lpSubKey );								/* ��������ʽ��ע���							*/
	BOOL CreateKey( LPCTSTR lpSubKey ,DWORD dwExFlag);
	BOOL DeleteKey(LPCTSTR lpSubKey );				/* ɾ����Ӧ���Ӽ����Ӽ�Ϊ�գ�					*/
	BOOL DeleteValue(LPCTSTR lpKeyName);						/* ɾ���Ӽ�������Ӧ�ļ�ֵ						*/	
	BOOL SaveKey(LPCTSTR lpFileName);								/* �ѵ�ǰ��ֵ���浽ָ���ļ�						*/
	BOOL RestoreKey(LPCTSTR lpFileName);							/* ��ָ��ע����ļ��лָ�						*/

	BOOL ReadKey(LPCTSTR lpKeyName,CString& strKeyValue );				/* ����REG_SZ���͵�ֵ							*/
	BOOL ReadKey(LPCTSTR lpKeyName,DWORD& dwVal);				/* ����DWORD���͵�ֵ							*/
	BOOL ReadKey(LPCTSTR lpKeyName,LPBYTE lpByte,DWORD& dwSize);
	BOOL WriteKey(LPCTSTR lpSubKey,LPCTSTR lpVal );					/* д��REG_SZ����ֵ								*/
	BOOL WriteKey(LPCTSTR lpSubKey,DWORD dwVal );					/* д��DWORD����ֵ								*/
};
