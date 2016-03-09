#ifndef __ERRORINFO__H__
#define __ERRORINFO__H__

#include <atlstr.h>

//错误信息线程本地存储
typedef enum
{
	CUSTOM_ERROR = 1,			//自定义错误
	SYSTEM_ERROR,					//系统错误
	WINSOCK_ERROR,				//WINSOCK错误
	KERNEL_ERROR,					//NTSTATUS类型的错误，暂时不支持，如果需要支持自己实现RtlNtStatusToDosError相关信息
	COM_ERROR,						//COM返回的LRESULT错误，可以使用HRESULT_CODE取得错误号
}LS_ERROR_TYPE,*PLS_ERROR_TYPE;

typedef struct tagWIN_ERROR_INFO
{
	DWORD	dwErrorCode;
	DWORD	 dwErrorType;
	CString		 strErrorInfo;
}WIN_ERROR_INFO,*PWIN_ERROR_INFO;

extern DWORD g_dwLastError;

class CWinErrorMsg
{
public:
	static CWinErrorMsg* Init();
	void Release();

	DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);
	DWORD SetErrorTitle(LPCTSTR lpszTitle,...);
	CString& GetThreadErrorInfoString();
protected:
	CWinErrorMsg();
	~CWinErrorMsg();

	WIN_ERROR_INFO& GetThreadErrorInfo();
private:
	static CWinErrorMsg* m_pLastErrorInfo;
};

#endif //__ERRORINFO__H__