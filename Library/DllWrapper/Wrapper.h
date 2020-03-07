#pragma once

class DLL_WRAPPER_API Wrapper : public Base
{
public:
	Wrapper();
	virtual ~Wrapper();
public:
	BOOL InitializeLibrary(LPCTSTR lpszFileName, BOOL bRetry = FALSE, DWORD* pdwWin32ErrorCode = NULL);
	BOOL UnInitializeLibrary(DWORD* pdwWin32ErrorCode = NULL);
	BOOL IsInitialized() const;
protected:
	virtual BOOL GetFunctionsAddress(BOOL bDefaultValue = TRUE, DWORD* pdwWin32ErrorCode = NULL) { return FALSE; };
protected:
	BOOL m_bIsInitialized;
};

#define DECLARE_TMCOMMON_LIBRARY_WRAPPER(cls)	\
	cls(){this->GetFunctionsAddress(false);}	\
	virtual ~cls(){this->UnInitializeLibrary(NULL);}	\
protected:	\
	virtual bool GetFunctionsAddress(bool _bDefaultValue = true, DWORD *_pdwWin32ErrorCode = NULL);

#define IMPL_TMCOMMON_LIBRARY_WRAPPER(cls)	\
	bool cls::GetFunctionsAddress(bool _bDefaultValue, DWORD *_pdwWin32ErrorCode)	\
{	\
	bool bSuccess = true;	\
	DWORD dwWin32ErrorCode = ERROR_SUCCESS;

#define IMPL_INLINE_TMCOMMON_LIBRARY_WRAPPER(cls)	\
	inline bool cls::GetFunctionsAddress(bool _bDefaultValue, DWORD *_pdwWin32ErrorCode)	\
{	\
	bool bSuccess = true;	\
	DWORD dwWin32ErrorCode = ERROR_SUCCESS;

#define PROC_TMCOMMON_LIBRARY_WRAPPER(FarProc, lpFuncName)	\
	if(false == _bDefaultValue){	\
	FarProc = NULL;	\
	}else if(false == this->GetProcAddress((FARPROC*)&FarProc, lpFuncName, &dwWin32ErrorCode)){	\
	bSuccess = false;	\
	if(_pdwWin32ErrorCode){*_pdwWin32ErrorCode = dwWin32ErrorCode;}	\
	}

#define END_TMCOMMON_LIBRARY_WRAPPER()	\
	this->m_bIsInitialized = _bDefaultValue ? bSuccess : false;	\
	return(bSuccess);	\
}