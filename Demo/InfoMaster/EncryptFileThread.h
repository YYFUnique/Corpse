#pragma once

#include "DllCore/Thread/LsThread.h"

class CEncryptFile : public CLsThread
{
public:
	CEncryptFile();
	~CEncryptFile();

	BOOL StopThread(DWORD dwWaitTime , BOOL bTerminateThreadWhenWaitTimeout);
public:
	//默认线程函数
	static UINT workThread(LPVOID lParam);
};