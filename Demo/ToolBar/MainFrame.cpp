#include "stdafx.h"
#include "Toolbar.h"
#include "define.h"
#include "type.h"
#include "DllCore/Log/LogHelper.h"

#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

typedef enum tagETHERNET_TYPE
{
	ETHERNET_TYPE_IP			= 0x0800,	// type:IP
	ETHERNET_TYPE_ARP		= 0x0806,	//type:ARP
	ETHERNET_TYPE_RARP	= 0x8035,	//type:RARP
}ETHERNET_TYPE;

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{

// 	int sum = 0;
// 	Lua= lua_open();
// 	luaopen_base(L);
// 	luaL_openlibs(L);
// 	luaL_loadfile(L, "add.lua");
// 	lua_pcall(L, 0, LUA_MULTRET, 0);
// 	/*sum = luaAdd(10, 15);*/
// 
// 	return TRUE;

	HANDLE hNdisProto = CreateFile(_T("\\\\.\\NdisProto"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	//QLOG_INFO(_T("CreateFile"));

	//CString strDeviceName(_T("\\DEVICE\\{FAC2AF0A-6EC0-44FD-A8D7-C22EC0643103}"));
	////CString strDeviceName(_T("\\DEVICE\\{77542FED-4372-44B0AA92-EEEAA4BA30B2}"));

	//DWORD dwLen = (strDeviceName.GetLength() + 1) * sizeof(TCHAR);
	//DWORD dwByteReturn = 0;
	//if (DeviceIoControl(hNdisProto, IOCTL_NDISPROT_BIND_NETWORK_ADAPTER, (LPVOID)(LPCWSTR)(LPCTSTR)strDeviceName, dwLen, NULL, 0, &dwByteReturn, NULL) == FALSE)
	//{
	//	return 0;
	//}
	////CloseHandle(hNdisProto);
	//
	//QLOG_INFO(_T("IOCTL_NDISPROT_BIND_NETWORK_ADAPTER SUCCESS"));

	//if (DeviceIoControl(hNdisProto, IOCTL_NDISPROT_OPEN_DEVICE, (LPVOID)(LPCWSTR)(LPCTSTR)strDeviceName, dwLen, NULL, 0, &dwByteReturn, NULL) == FALSE)
	//{
	//	return 0;
	//}

	//QLOG_INFO(_T("IOCTL_NDISPROT_OPEN_DEVICE SUCCESS"));

	////00-0C-29-83-F8-67

	////BYTE sMac[] = {0x00,0x0C,0x29,0x83,0xF8,0x67};
	//BYTE sMac[] = {0x28,0x51,0x32,0x11,0xFA,0x6B};
	//BYTE dMac[] = {0x94,0xDE,0x80,0xCB,0xCD,0x8A};
	////BYTE dMac[] = {0xD4,0x3D,0x7E,0xB2,0x12,0xFB};

	//BYTE PacketBuffer[sizeof(ETHERNET_HEADER) + sizeof(ARP_HEADER)+18];
	//PETHERNET_HEADER pEthernetHeader = (PETHERNET_HEADER)PacketBuffer;
	////memcpy(pEthernetHeader->dhost , dMac , 6);
	////ZeroMemory(pEthernetHeader->dhost, 0xFF);//, 6);
	//memset(pEthernetHeader->dhost, 0xFF, 6);
	//memcpy(pEthernetHeader->shost , sMac , 6);
	//pEthernetHeader->type = htons(ETHERNET_TYPE_ARP);

	//PARP_HEADER pArpHeader = (PARP_HEADER)(PacketBuffer + sizeof(ETHERNET_HEADER));
	//pArpHeader->hrd = htons(1);
	//pArpHeader->eth_type = htons(0x0800);
	//pArpHeader->maclen = 6;														//	MAC地址的长度，为6
	//pArpHeader->iplen = 4;															//	IP地址的长度，为4
	//pArpHeader->opcode = htons(1);											//	操作代码，ARPOP_REQUEST为请求，ARPOP_REPLY为响应
	//memcpy(pArpHeader->smac , sMac, 6);								//	源MAC地址
	//*(DWORD*)(pArpHeader->saddr) = 0xA308A8C0;				//	源IP地址
	////memcpy(pArpHeader->dmac , sMac , 6);								//	目的MAC地址
	//memset(pArpHeader->dmac , 0x00 , 6);								//	目的MAC地址
	//*(DWORD*)(pArpHeader->daddr) = 0xA308A8C0;				//	目的IP地址

	//int n=0;

	//QLOG_INFO(_T("IOCTL_NDISPROT_SEND_RAW_PACKET Begin"));

	//while(TRUE)
	//{
	//	if (DeviceIoControl(hNdisProto , IOCTL_NDISPROT_SEND_RAW_PACKET , PacketBuffer , sizeof(PacketBuffer) , NULL , 0 , &dwByteReturn,0) == FALSE)
	//	{
	//		QLOG_INFO(_T("IOCTL_NDISPROT_SEND_RAW_PACKET X"));
	//		break;
	//	}
	//	Sleep(1000);
	//	if (++n > 200)
	//		break;
	//	QLOG_INFO(_T("DeviceIoControl"));
	//}
	//
	//return 0;
// 
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CToolbar* pToolbar = new CToolbar();
		if (pToolbar == NULL) 
			break;

		pToolbar->Create(NULL, _T("桌面工具栏"), UI_WNDSTYLE_DIALOG,WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,0, 0, 800, 600);
		pToolbar->CenterWindow();
		pToolbar->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}