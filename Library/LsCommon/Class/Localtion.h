#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef enum _IP_LOCALTION
{
	IpFromDataFile = 1,			//使用纯真数据库文件格式
	IpFromWebFile,					//通过使用第三方Web查询接口
}IP_LOCALTION,*PIP_LOCALTION;

/************************************************************************/
// 使用纯真IP库获取对应IP地址的位置信息
// 使用方式：
/************************************************************************/
class LS_COMMON_API CIPLocaltion
{
public:
	CIPLocaltion();
	CIPLocaltion(LPCTSTR lpszFilePath);
	~CIPLocaltion();
	static CIPLocaltion* Create(IP_LOCALTION IpLocaltion,LPCTSTR lpszFilePath);
	virtual CString GetLocation(LPCTSTR lpszIPAddress = NULL) = 0;
protected:
	CString m_strFilePath;
};

class LS_COMMON_API CIPFromWeb : public CIPLocaltion
{
public:
	CIPFromWeb();
	CIPFromWeb(LPCTSTR lpszFilePath);
	~CIPFromWeb();
	virtual CString GetLocation(LPCTSTR lpszIPAddress = NULL);
};