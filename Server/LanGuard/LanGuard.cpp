#include "StdAfx.h"
#include "TCPServer.h"
#include "ADELLock.h"

CTCPServer g_TCPSrv; 

void RecvCB(int client_id, const char* buf, int buf_size) 
{ 
	//处理接受消息 
	char* tmp = new char[buf_size*2]; 
	memcpy(tmp, buf, buf_size*2); 
	g_TCPSrv.Send(client_id, (const char*)tmp, buf_size*2); 

	char* tmp2 = new char[buf_size]; 
	memcpy(tmp2, buf, buf_size); 
	g_TCPSrv.Send(client_id, (const char*)tmp2, buf_size); 
	delete[] tmp; 
	tmp = 0; 
	delete[] tmp2; 
	tmp2 = 0; 
	return; 
} 

void NewConnectCB(int nClientId)
{ 
	//设置新连接接收数据回调函数 
	g_TCPSrv.SetRecvCB(nClientId, (ServerRecvcb)RecvCB); 
} 

int main() 
{ 
#if _DEBUG
	CADELLock* pADELLock = new CADELLock();
	//int n= pADELLock->Init();
	LONG lCardNo;
	int n=pADELLock->ReadId(&lCardNo);
	pADELLock->Release();
#else
	g_TCPSrv.SetConnectExtra((NewConnect)NewConnectCB); 
	g_TCPSrv.Start(_T("0.0.0.0"), 6111); 
#endif
	return 0; 
}