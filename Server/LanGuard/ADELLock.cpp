#include "StdAfx.h"
#include "ADELLock.h"
#include "ADELErrorInfo.h"

CADELLock::CADELLock()
	: m_pfnInit(NULL), m_pfnEndSession(NULL), m_pfnChangeUser(NULL)
	, m_pfnNewKey(NULL), m_pfnDupKey(NULL), m_pfnReadCard(NULL)
	, m_pfnEraseCard(NULL), m_pfnReadId(NULL), m_pfnCheckOut(NULL)
	, m_pfnLostCard(NULL), m_pfnPopCard(NULL), m_pfnSetPort(NULL)
	, m_pfnCheckSC(NULL), m_pfnReadIC(NULL), m_pfnWriteIC(NULL)
	, m_pfnWriteMagCard(NULL), m_pfnReadMagCard(NULL), m_pfnReaderBeep(NULL)
	, m_pfnGetBuffInfo(NULL), m_pfnGetMagBuffInfo(NULL), m_pfnGetCardInfo(NULL)
{
	m_hModule = LoadLibrary(_T("MainDll.dll"));
	if (m_hModule != NULL)
	{
		m_pfnInit						= (FN_INIT)GetProcAddress(m_hModule, "Init");
		m_pfnNewKey				= (FN_NewKey)GetProcAddress(m_hModule, "NewKey");
		m_pfnEndSession		= (FN_EndSession)GetProcAddress(m_hModule, "EndSession");
		m_pfnChangeUser		= (FN_ChangeUser)GetProcAddress(m_hModule, "ChangeUser");
		m_pfnDupKey				= (FN_DupKey)GetProcAddress(m_hModule, "DupKey");
		//m_pfnAddKey				= (FN_AddKey)GetProcAddress(m_hModule, "AddKey");
		m_pfnReadCard			= (FN_ReadCard)GetProcAddress(m_hModule, "ReadCard");
		m_pfnEraseCard			= (FN_EraseCard)GetProcAddress(m_hModule, "EraseCard");
		m_pfnReadId				= (FN_ReadId)GetProcAddress(m_hModule, "ReadCardId");
		m_pfnCheckOut			=	(FN_CheckOut)GetProcAddress(m_hModule, "CheckOut");
		m_pfnLostCard			=	(FN_LostCard)GetProcAddress(m_hModule, "LostCard");
		m_pfnPopCard			= (FN_PopCard)GetProcAddress(m_hModule, "PopCard");
		m_pfnSetPort				=	(FN_SetPort)GetProcAddress(m_hModule, "SetPort");
		m_pfnCheckSC			=	(FN_CheckSC)GetProcAddress(m_hModule, "CheckSC");
		m_pfnReadIC				=	(FN_ReadIC)GetProcAddress(m_hModule, "ReadIC");
		m_pfnWriteIC				=	(FN_WriteIC)GetProcAddress(m_hModule, "WriteIC");
		m_pfnWriteMagCard	=	(FN_WriteMagCard)GetProcAddress(m_hModule, "WriteMagCard");
		m_pfnReadMagCard	=	(FN_ReadMagCard)GetProcAddress(m_hModule, "ReadMagCard");
		m_pfnReaderBeep		=	(FN_ReaderBeep)GetProcAddress(m_hModule, "Reader_Beep");
		m_pfnGetBuffInfo		=	(FN_GetBuffInfo)GetProcAddress(m_hModule, "GetBuffInfo");
		m_pfnGetMagBuffInfo=	(FN_GetMagBuffInfo)GetProcAddress(m_hModule, "GetMagBuffInfo");
		m_pfnGetCardInfo		=	(FN_GetCardInfo)GetProcAddress(m_hModule, "GetCardInfo");
		//m_pfnReadCardData	= (FN_ReadCardData)GetProcAddress(m_hModule, "ReadCardData");
		//m_pfnWriteCardData	= (FN_WriteCardData)GetProcAddress(m_hModule, "WriteCardData");
	}
}

CADELLock::~CADELLock()
{
	if (m_hModule != NULL)
		FreeLibrary(m_hModule);
}

int CADELLock::Init(int nSoftware, LPCTSTR lpszSQLAddr, LPCTSTR lpszAdmin, int nPort, int nEncoder /*= 0*/, int nTMEncoder /*= 5*/)
{
	if (m_pfnInit == NULL)
		return INVALID_PTR_VALUE;

	CStringA strSQLAddr(lpszSQLAddr);
	CStringA strUserName(lpszAdmin);
	/*char* szSQLAddr = strSQLAddr.();*/
	char szSQLAddr[MAX_PATH], szUserName[MAX_PATH];
	strcpy_s(szSQLAddr, _countof(szSQLAddr), strSQLAddr);
	strcpy_s(szUserName, _countof(szUserName), strUserName);
	return m_pfnInit(nSoftware, szSQLAddr, szUserName, nPort, nEncoder, nTMEncoder);
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

int CADELLock::NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, 
					  LPCTSTR lpszGuestName, LPCTSTR lpszGuestId, LPCTSTR lpszLift, 
					  BOOL bCovered, int nBreakfast, LONG* pCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2)
{
	if (m_pfnNewKey == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoomNum(lpszRoomNum), strGate(lpszGate), strValidTime(lpszValidTime);
	CStringA strGuestName(lpszGuestName), strGuestId(lpszGuestId), strElevator(lpszLift);
	CStringA strTrack1(lpszTrack1), strTrack2(lpszTrack2);

	char szRoomNum[20], szGate[50], szValidTime[30], szGuestName[50], szGuestId[50], szElevator[MAX_PATH];
	char szTrack1[MAX_PATH], szTrack2[MAX_PATH];
	strcpy_s(szRoomNum, _countof(szRoomNum), strRoomNum);
	strcpy_s(szGate, _countof(szGate), strGate);
	strcpy_s(szValidTime, _countof(szValidTime), strValidTime);
	strcpy_s(szGuestName, _countof(szGuestName), strGuestName);
	strcpy_s(szGuestId, _countof(szGuestId), strGuestId);
	strcpy_s(szElevator, _countof(szElevator), strElevator);
	strcpy_s(szTrack1, _countof(szTrack1) ,strTrack1);
	strcpy_s(szTrack2, _countof(szTrack2), strTrack2);

	return m_pfnNewKey(szRoomNum, szGate, szValidTime, szGuestName, szGuestId, 
										szElevator, bCovered, nBreakfast, pCardNo, szTrack1, szTrack2);
}

int CADELLock::DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, 
					  LPCTSTR lpszGuestName, LPCTSTR lpszGuestId, LPCTSTR lpszLift, 
					  BOOL bCovered, int nBreakfast, LONG* pCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2)
{
	if (m_pfnNewKey == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoomNum(lpszRoomNum), strGate(lpszGate), strValidTime(lpszValidTime);
	CStringA strGuestName(lpszGuestName), strGuestId(lpszGuestId), strElevator(lpszLift);
	CStringA strTrack1(lpszTrack1), strTrack2(lpszTrack2);

	char szRoomNum[20], szGate[50], szValidTime[30], szGuestName[50], szGuestId[50], szElevator[MAX_PATH];
	char szTrack1[MAX_PATH], szTrack2[MAX_PATH];
	strcpy_s(szRoomNum, _countof(szRoomNum), strRoomNum);
	strcpy_s(szGate, _countof(szGate), strGate);
	strcpy_s(szValidTime, _countof(szValidTime), strValidTime);
	strcpy_s(szGuestName, _countof(szGuestName), strGuestName);
	strcpy_s(szGuestId, _countof(szGuestId), strGuestId);
	strcpy_s(szElevator, _countof(szElevator), strElevator);
	strcpy_s(szTrack1, _countof(szTrack1) ,strTrack1);
	strcpy_s(szTrack2, _countof(szTrack2), strTrack2);

	return m_pfnDupKey(szRoomNum, szGate, szValidTime, szGuestName, szGuestId, 
										szElevator, bCovered, nBreakfast, pCardNo, szTrack1, szTrack2);
}

//int CADELLock::AddKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
//									   LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
//									   LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger)
//{
//	if (m_pfnDupKey == NULL)
//		return INVALID_PTR_VALUE;
//
//	CStringA strRoomNum(lpszRoomNum), strGate(lpszGate), strValidTime(lpszValidTime);
//	CStringA strGuestName(lpszGuestName), strGuestId(lpszGuestId), strElevator(lpszElevator);
//
//	char szRoomNum[20], szGate[50], szValidTime[30], szGuestName[50], szGuestId[50], szElevator[MAX_PATH];
//	strcpy_s(szRoomNum, _countof(szRoomNum), strRoomNum);
//	strcpy_s(szGate, _countof(szGate), strGate);
//	strcpy_s(szValidTime, _countof(szValidTime), strValidTime);
//	strcpy_s(szGuestName, _countof(szGuestName), strGuestName);
//	strcpy_s(szGuestId, _countof(szGuestId), strGuestId);
//	strcpy_s(szElevator, _countof(szElevator), strElevator);
//
//	return m_pfnAddKey(szRoomNum, szGate, szValidTime, szGuestName, szGuestId,
//										bCovered, nOpenWay, pCardNo, nBreakfast, NULL, NULL, szElevator, bFinger);
//}

//int CADELLock::ReadCardData(int nCardType, int nStart, int nLen, CString& strData)
//{
//	if (m_pfnReadCardData == NULL)
//		return INVALID_PTR_VALUE;
//
//	char szBuff[MAX_PATH];
//	ZeroMemory(szBuff, _countof(szBuff));
//
//	int nRet = m_pfnReadCardData(nCardType, nStart, nLen, szBuff);
//	strData = szBuff;
//
//	return nRet;
//}
//
//int CADELLock::WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData)
//{
//	if (m_pfnWriteCardData == NULL)
//		return INVALID_PTR_VALUE;
//
//	CStringA strData(lpszData);
//	char szData[MAX_PATH];
//	strcpy_s(szData, _countof(szData), strData);
//
//	return m_pfnWriteCardData(nCardType, nStart, nLen, szData);
//}

int CADELLock::CheckOut(LPCTSTR lpszRoom, LONG lCardno)
{
	if (m_pfnCheckOut == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoom(lpszRoom);

	char szRoom[20];
	strcpy_s(szRoom, _countof(szRoom), strRoom);

	return m_pfnCheckOut(szRoom, lCardno);
}

int CADELLock::LostCard(LPCTSTR lpszRoom, LONG lCardno)
{
	if (m_pfnLostCard == NULL)
		return INVALID_PTR_VALUE;

	CStringA strRoom(lpszRoom);

	char szRoom[20];
	strcpy_s(szRoom, _countof(szRoom), strRoom);

	return m_pfnLostCard(szRoom, lCardno);
}

int CADELLock::PopCard(void)
{
	if (m_pfnPopCard == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnPopCard();
}

int CADELLock::SetPort(int nSoftware, int nPort, int nEncoder, int nTMEncoder)
{
	if (m_pfnSetPort == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnSetPort(nSoftware, nPort, nEncoder, nTMEncoder);
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

	int nRet = m_pfnReadCard(szRoom, szGate, szVaildTime, szGuestName, szGuestId, szElevator, NULL, NULL, &lCardNo, &nState, &nBreakfast);
	pCardData->strRoom = szRoom;
	pCardData->strGate = szGate;
	pCardData->strVaildTime = szVaildTime;
	pCardData->strGuestName = szGuestName;
	pCardData->strGuestId = szGuestId;
	pCardData->strElevator = szElevator;
	pCardData->lCardNo = lCardNo;
	pCardData->CardState = (CARD_STATE)nState;
	//pCardData->OpenWay = (OPEN_WAY)nOpenWay;
	pCardData->nBreakfast = nBreakfast;

	return nRet;
}

int CADELLock::EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2)
{
	if (m_pfnEraseCard == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnEraseCard(lCardNo, NULL, NULL);
}

int CADELLock::ReadCardId(LONG* lpCardNo)
{
	if (m_pfnReadId == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnReadId(lpCardNo);
}

int CADELLock::CheckSC(LPCTSTR lpszSC)
{
	if (m_pfnCheckSC == NULL)
		return INVALID_PTR_VALUE;

	CStringA strCardSC(lpszSC);
	char szCardSC[MAX_PATH];
	strcpy_s(szCardSC, _countof(szCardSC), strCardSC);

	return m_pfnCheckSC(szCardSC);
}

int CADELLock::ReadIC(UINT nStart, UINT nLen, LPCTSTR lpszData)
{
	if (m_pfnReadIC == NULL)
		return INVALID_PTR_VALUE;

	CStringA strData(lpszData);
	char szData[MAX_PATH];
	strcpy_s(szData, _countof(szData), strData);

	return m_pfnReadIC(nStart, nLen, szData);
}

int CADELLock::WriteIC(UINT nStart, UINT nLen, LPCTSTR lpszData)
{
	if (m_pfnWriteIC == NULL)
		return INVALID_PTR_VALUE;

	CStringA strData(lpszData);
	char szData[MAX_PATH];
	strcpy_s(szData, _countof(szData), strData);

	return m_pfnWriteIC(nStart, nLen, szData);
}

int CADELLock::WriteMagCard(LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszTrack3)
{
	if (m_pfnWriteMagCard == NULL)
		return INVALID_PTR_VALUE;

	CStringA strTrack1(lpszTrack1), strTrack2(lpszTrack2), strTrack3(lpszTrack3);
	char szTrack1[MAX_PATH], szTrack2[MAX_PATH], szTrack3[MAX_PATH];
	strcpy_s(szTrack1, _countof(szTrack1), strTrack1);
	strcpy_s(szTrack2, _countof(szTrack2), strTrack2);
	strcpy_s(szTrack3, _countof(szTrack3), strTrack3);

	return m_pfnWriteMagCard(szTrack1, szTrack2, szTrack3);
}
int CADELLock::ReadMagCard(LPTSTR lpszTrack1, LPTSTR lpszTrack2, LPTSTR lpszTrack3)
{
	if (m_pfnWriteMagCard == NULL)
		return INVALID_PTR_VALUE;

	CStringA strTrack1(lpszTrack1), strTrack2(lpszTrack2), strTrack3(lpszTrack3);
	char szTrack1[MAX_PATH], szTrack2[MAX_PATH], szTrack3[MAX_PATH];
	strcpy_s(szTrack1, _countof(szTrack1), strTrack1);
	strcpy_s(szTrack2, _countof(szTrack2), strTrack2);
	strcpy_s(szTrack3, _countof(szTrack3), strTrack3);

	//return m_pfnWriteMagCard(lpszTrack1, lpszTrack2, lpszTrack3);
	return 0;
}

int CADELLock::Reader_Beep(int Sound)
{
	if (m_pfnReaderBeep == NULL)
		return INVALID_PTR_VALUE;

	return m_pfnReaderBeep(Sound);
}

int CADELLock::GetBuffInfo(BUFF_INFO* pBuffInfo)
{
	if (m_pfnGetBuffInfo == NULL)
		return INVALID_PTR_VALUE;

	char szData[MAX_PATH], szRoomNo[50], szUserName[50], szSTime[50];
	int nLen = _countof(szData);

	int nRet = m_pfnGetBuffInfo(szData, nLen, &pBuffInfo->CardType, &pBuffInfo->CardState, 
													&pBuffInfo->lCardNo, szRoomNo, szUserName, szSTime);

	pBuffInfo->strCardData = szData;
	pBuffInfo->strRoomNum = szRoomNo;
	pBuffInfo->strGName = szUserName;
	pBuffInfo->strValidTime = szSTime;

	return nRet;
}

int CADELLock::GetMagBuffInfo(BUFF_INFO* pBuffInfo)//LPTSTR lpBuff, int *nCardType, int *nCardState, int *nCardNo, LPTSTR szRoomNo, LPTSTR szUserName,LPTSTR szSTime)
{
	if (m_pfnGetMagBuffInfo == NULL)
		return INVALID_PTR_VALUE;

	char szData[MAX_PATH], szRoomNo[50], szUserName[50], szSTime[50];
	int nLen = _countof(szData);

	int nRet = m_pfnGetMagBuffInfo(szData, &pBuffInfo->CardType, &pBuffInfo->CardState, 
		&pBuffInfo->lCardNo, szRoomNo, szUserName, szSTime);

	pBuffInfo->strCardData = szData;
	pBuffInfo->strRoomNum = szRoomNo;
	pBuffInfo->strGName = szUserName;
	pBuffInfo->strValidTime = szSTime;

	return nRet;
}

int CADELLock::GetCardInfo(BUFF_INFO* pBuffInfo)
{
	if (m_pfnGetCardInfo == NULL)
		return INVALID_PTR_VALUE;

	char szRoomNo[50], szUserName[50], szSTime[50];

	int nRet = m_pfnGetCardInfo(pBuffInfo->lCardNo, pBuffInfo->CardType, &pBuffInfo->CardState, szRoomNo, szUserName, szSTime);

	pBuffInfo->strRoomNum = szRoomNo;
	pBuffInfo->strGName = szUserName;
	pBuffInfo->strValidTime = szSTime;

	return nRet;
}

void CADELLock::Release()
{
	delete this;
}