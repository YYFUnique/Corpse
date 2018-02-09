#pragma once

#include "Monitor.h"

class CDeamonCom : public IMonitor
{
public:
	CDeamonCom();
	~CDeamonCom();

public:
	// IUnknown�ӿ�
	virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
	virtual ULONG   WINAPI AddRef(void);
	virtual ULONG   WINAPI Release(void);

	//IMonitor�ӿ�
	virtual HRESULT WINAPI AddAppInfo(LPCTSTR lpszFilePath);
protected:
	ULONG m_ulRef;
};