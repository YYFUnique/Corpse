#include "stdafx.h"
#include "Localtion.h"

CIPLocaltion::CIPLocaltion()
{
	
}

CIPLocaltion::CIPLocaltion(LPCTSTR lpszFilePath)
{
	m_strFilePath = lpszFilePath;
}

CIPLocaltion::~CIPLocaltion()
{

}


CIPLocaltion* CIPLocaltion::Create(IP_LOCALTION IpLocaltion,LPCTSTR lpszFilePath)
{
	CIPLocaltion* pIPLocaltion = NULL;
	switch(IpLocaltion)
	{
	case IpFromDataFile:
		break;
	case IpFromWebFile:
		pIPLocaltion = new CIPFromWeb(lpszFilePath);
		break;
	}
	return pIPLocaltion;
}

CIPFromWeb::CIPFromWeb()
{

}

CIPFromWeb::CIPFromWeb(LPCTSTR lpszFilePath)
{
	m_strFilePath = lpszFilePath;
}

CIPFromWeb::~CIPFromWeb()
{

}

CString CIPFromWeb::GetLocation(LPCTSTR lpszIPAddress /*= NULL*/)
{
	HANDLE hFile = CreateFile(m_strFilePath,GENERIC_ALL,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return _T("");

	CString strLocation;
	char szData[4*1024+1];
	DWORD dwReadLen = 0;
	while (TRUE)
	{
		if (ReadFile(hFile,szData,4*1024,&dwReadLen,NULL) == FALSE)
		{
			strLocation.Empty();
			break;
		}
		szData[dwReadLen]=_T('\0');
		strLocation += szData;

		if (dwReadLen != 4*1024)
			break;
	}
	
	CloseHandle(hFile);
	
	return strLocation;
}
