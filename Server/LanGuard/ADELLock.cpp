#include "StdAfx.h"
#include "ADELLock.h"
#include "ADELErrorInfo.h"

CADELLock::CADELLock()
	: m_pfnInit(NULL), m_pfnEndSession(NULL), m_pfnChangeUser(NULL)
	, m_pfnNewKey(NULL), m_pfnDupKey(NULL), m_pfnReadCard(NULL)
	, m_pfnEraseCard(NULL), m_pfnReadId(NULL), m_pfnReadCardData(NULL)
	, m_pfnWriteCardData(NULL)
{
	m_hModule = LoadLibrary(_T("MainDll.dll"));
	if (m_hModule != NULL)
	{
		m_pfnInit						= (FN_INIT)GetProcAddress(m_hModule, "Init");
		m_pfnNewKey				= (FN_NewKey)GetProcAddress(m_hModule, "NewKey");
		m_pfnEndSession		= (FN_EndSession)GetProcAddress(m_hModule, "EndSession");
		m_pfnChangeUser		= (FN_ChangeUser)GetProcAddress(m_hModule, "ChangeUser");
		m_pfnDupKey				= (FN_DupKey)GetProcAddress(m_hModule, "DupKey");
		m_pfnReadCard			= (FN_ReadCard)GetProcAddress(m_hModule, "ReadCard");
		m_pfnEraseCard			= (FN_EraseCard)GetProcAddress(m_hModule, "EraseCard");
		m_pfnReadId				= (FN_ReadId)GetProcAddress(m_hModule, "ReadId");
		m_pfnReadCardData	= (FN_ReadCardData)GetProcAddress(m_hModule, "ReadCardData");
		m_pfnWriteCardData	= (FN_WriteCardData)GetProcAddress(m_hModule, "WriteCardData");
	}
}

CADELLock::~CADELLock()
{
	if (m_hModule != NULL)
		FreeLibrary(m_hModule);
}

int CADELLock::Init(LPCTSTR lpszSQLAddr, int nPort)
{
	if (m_pfnInit == NULL)
		return INVALID_PTR_VALUE;

	CStringA strSQLAddr(lpszSQLAddr);
	/*char* szSQLAddr = strSQLAddr.();*/
	char szSQLAddr[MAX_PATH];
	strcpy_s(szSQLAddr, _countof(szSQLAddr), strSQLAddr);
	return m_pfnInit(30, szSQLAddr, "Admin", nPort, 0, 5);
}

int CADELLock::EndSession()
{
	if (m_pfnEndSession == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnEndSession();
}

int CADELLock::ChangeUser(LPCTSTR lpszAdmin)
{
	if (m_pfnChangeUser == NULL)
		return INVALID_PTR_VALUE;

	CStringA strAdmin(lpszAdmin);
	char szAdmin[100];
	strcpy_s(szAdmin, _countof(szAdmin), strAdmin);
	m_pfnChangeUser(szAdmin);
	return 0;
}

int CADELLock::NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
										LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
										LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger)
{
	if (m_pfnNewKey == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoomNum(lpszRoomNum), strGate(lpszGate), strValidTime(lpszValidTime);
	CStringA strGuestName(lpszGuestName), strGuestId(lpszGuestId), strElevator(lpszElevator);

	char szRoomNum[20], szGate[50], szValidTime[30], szGuestName[50], szGuestId[50], szElevator[MAX_PATH];
	strcpy_s(szRoomNum, _countof(szRoomNum), strRoomNum);
	strcpy_s(szGate, _countof(szGate), strGate);
	strcpy_s(szValidTime, _countof(szValidTime), strValidTime);
	strcpy_s(szGuestName, _countof(szGuestName), strGuestName);
	strcpy_s(szGuestId, _countof(szGuestId), strGuestId);
	strcpy_s(szElevator, _countof(szElevator), strElevator);

	return m_pfnNewKey(szRoomNum, szGate, szValidTime, szGuestName, szGuestId,
										bCovered, nOpenWay, pCardNo, nBreakfast, NULL, NULL, szElevator, bFinger);
}

int CADELLock::DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
									  LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
									  LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger)
{
	if (m_pfnDupKey == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoomNum(lpszRoomNum), strGate(lpszGate), strValidTime(lpszValidTime);
	CStringA strGuestName(lpszGuestName), strGuestId(lpszGuestId), strElevator(lpszElevator);

	char szRoomNum[20], szGate[50], szValidTime[30], szGuestName[50], szGuestId[50], szElevator[MAX_PATH];
	strcpy_s(szRoomNum, _countof(szRoomNum), strRoomNum);
	strcpy_s(szGate, _countof(szGate), strGate);
	strcpy_s(szValidTime, _countof(szValidTime), strValidTime);
	strcpy_s(szGuestName, _countof(szGuestName), strGuestName);
	strcpy_s(szGuestId, _countof(szGuestId), strGuestId);
	strcpy_s(szElevator, _countof(szElevator), strElevator);

	return m_pfnDupKey(szRoomNum, szGate, szValidTime, szGuestName, szGuestId,
										bCovered, nOpenWay, pCardNo, nBreakfast, NULL, NULL, szElevator, bFinger);
}

int CADELLock::ReadCard(CARD_DATA* pCardData)
{
	if (m_pfnReadCard == NULL)
		return INVALID_PTR_VALUE;

	LONG lCardNo = 0;
	int nState = CARD_STATE_NORMAL;
	int nOpenWay = OPEN_WAY_CARD;
	int nBreakfast = 0;
	char szRoom[50], szGate[50], szVaildTime[50], szGuestName[50], szGuestId[50], szElevator[50];
	ZeroMemory(szRoom, _countof(szRoom));
	ZeroMemory(szGate, _countof(szGate));
	ZeroMemory(szVaildTime, _countof(szVaildTime));
	ZeroMemory(szGuestName, _countof(szGuestName));
	ZeroMemory(szGuestId, _countof(szGuestId));
	ZeroMemory(szElevator, _countof(szElevator));

	int nRet = m_pfnReadCard(szRoom, szGate, szVaildTime, szGuestName, szGuestId, NULL, NULL,szElevator, &lCardNo, &nState, &nOpenWay, &nBreakfast);
	pCardData->strRoom = szRoom;
	pCardData->strGate = szGate;
	pCardData->strVaildTime = szVaildTime;
	pCardData->strGuestName = szGuestName;
	pCardData->strGuestId = szGuestId;
	pCardData->strElevator = szElevator;
	pCardData->lCardNo = lCardNo;
	pCardData->CardState = (CARD_STATE)nState;
	pCardData->OpenWay = (OPEN_WAY)nOpenWay;
	pCardData->nBreakfast = nBreakfast;

	return nRet;
}

int CADELLock::EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, int* pFinger)
{
	if (m_pfnEraseCard == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnEraseCard(lCardNo, NULL, NULL, pFinger);
}

int CADELLock::ReadCardId(LONG* lpCardNo)
{
	if (m_pfnReadId == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnReadId(lpCardNo);
}

int CADELLock::ReadCardData(int nCardType, int nStart, int nLen, CString& strData)
{
	if (m_pfnReadCardData == NULL)
		return INVALID_PTR_VALUE;

	char szBuff[MAX_PATH];
	ZeroMemory(szBuff, _countof(szBuff));

	int nRet = m_pfnReadCardData(nCardType, nStart, nLen, szBuff);
	strData = szBuff;

	return nRet;
}

int CADELLock::WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData)
{
	if (m_pfnWriteCardData == NULL)
		return INVALID_PTR_VALUE;

	CStringA strData(lpszData);
	char szData[MAX_PATH];
	strcpy_s(szData, _countof(szData), strData);

	return m_pfnWriteCardData(nCardType, nStart, nLen, szData);
}

void CADELLock::Release()
{
	delete this;
}