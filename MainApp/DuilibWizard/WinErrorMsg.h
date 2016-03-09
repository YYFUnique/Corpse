#ifndef __ERRORINFO__H__
#define __ERRORINFO__H__

#include <atlstr.h>

//������Ϣ�̱߳��ش洢
typedef enum
{
	CUSTOM_ERROR = 1,			//�Զ������
	SYSTEM_ERROR,					//ϵͳ����
	WINSOCK_ERROR,				//WINSOCK����
	KERNEL_ERROR,					//NTSTATUS���͵Ĵ�����ʱ��֧�֣������Ҫ֧���Լ�ʵ��RtlNtStatusToDosError�����Ϣ
	COM_ERROR,						//COM���ص�LRESULT���󣬿���ʹ��HRESULT_CODEȡ�ô����
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