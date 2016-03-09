#pragma once
#include <Winsock2.h>

typedef CDuiList<WSAPROTOCOL_INFOW,const WSAPROTOCOL_INFOW&> CWsaProtocolInfoList;
class CProvider : public CBase
{
public:
	CProvider();
	~CProvider();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
protected:
	BOOL EnumProtocols(CWsaProtocolInfoList& WscProtocolInfo);
	/*void ShowWindow();*/
	//static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};