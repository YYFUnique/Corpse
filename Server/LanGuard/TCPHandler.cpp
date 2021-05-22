#include "StdAfx.h"
#include "TCPHandler.h"
#include "TCPServer.h"
#include "TerminalData.h"
#include "ADELErrorInfo.h"

extern CTCPServer g_TCPSrv; 

//服务器回复字段
#define SERVER_SEND_VER			_T("version")
#define SERVER_SEND_CMD			_T("cmd")
#define SERVER_SEND_RESULT		_T("result")
#define SERVER_SEND_DATA			_T("data")

//	Init 初始化函数
#define ADEL_INIT_SOFTWARE		_T("software")
#define ADEL_INIT_SQLADDR		_T("server")
#define ADEL_INIT_USERNAME		_T("username")
#define ADEL_INIT_PORT				_T("port")
#define ADEL_INIT_ENCODER		_T("encoder")
#define ADEL_INIT_TMENCODER	_T("tmencoder")

// 
#define ADEL_CHANGE_USERNAME		_T("username")

//
#define ADEL_ERASE_CARDNO			_T("cardno")
#define ADEL_ERASE_TRACK1				_T("track1")
#define ADEL_ERASE_TRACK2				_T("track2")
#define ADEL_ERASE_FPINDEX				_T("fpindex")

//

#define ADEL_NEW_ROOM							_T("room")
#define ADEL_NEW_GATE								_T("gate")
#define ADEL_NEW_STIME								_T("stime")
#define ADEL_NEW_GNAME							_T("gname")
#define ADEL_NEW_GID									_T("gid")
#define ADEL_NEW_OVERFLAG						_T("overflag")
#define ADEL_NEW_OPENFLAG						_T("openflag")
#define ADEL_NEW_CARDNO						_T("cardno")
#define ADEL_NEW_BREAKFAST					_T("breakfast")
#define ADEL_NEW_LIFT								_T("lift")
#define ADEL_NEW_FINGERFLAG					_T("fingerflag")
#define ADEL_NEW_TRACK1							_T("track1")
#define ADEL_NEW_TRACK2							_T("track2")
//

#define ADEL_SEND_ERASE_FPINDEX				_T("fpindex")
#define ADEL_SEND_READ_CARDID					_T("cardid")

#define ADEL_SEND_READ_ROOM					_T("room")
#define ADEL_SEND_READ_GATE						_T("gate")
#define ADEL_SEND_READ_STIME						_T("stime")
#define ADEL_SEND_READ_GNAME					_T("gname")
#define ADEL_SEND_READ_GID							_T("gid")
#define ADEL_SEND_READ_OVERFLAG				_T("overflag")
#define ADEL_SEND_READ_OPENFLAG				_T("openflag")
#define ADEL_SEND_READ_CARDNO				_T("cardno")
#define ADEL_SEND_READ_STATE						_T("state")
#define ADEL_SEND_READ_BREAKFAST			_T("breakfast")
#define ADEL_SEND_READ_TRACK1					_T("track1")
#define ADEL_SEND_READ_TRACK2					_T("track2")
#define ADEL_SEND_READ_LIFT							_T("lift")

CTCPHandler::CTCPHandler()
{
	m_pADELLock = NULL;
}

CTCPHandler::~CTCPHandler()
{
	if (m_pADELLock != NULL)
	{
		m_pADELLock->Release();
		m_pADELLock = NULL;
	}
}

void CTCPHandler::Init()
{
	if (m_pADELLock == NULL)
		m_pADELLock = new CADELLock();
}

void CTCPHandler::OnClose()
{
	delete this;
}

void CTCPHandler::RecvRoute(int nClientId, LPCSTR lpszData, int nLen)
{
	if (m_pADELLock == NULL)
		return;
	/*g_TCPSrv.Send(nClientId, lpszData, nLen);
	LPCSTR  lpszDatax = "abcdefghijklmn";
	g_TCPSrv.Send(nClientId, lpszDatax, strlen(lpszDatax));*/
	CString strData(lpszData, nLen);
	CTerminalData TerminalData(strData);

	CJsonObject JsonServerData;
	OnRecvData(TerminalData, JsonServerData);
	CStringA strTipInfo(JsonServerData.ToString());
	g_TCPSrv.Send(nClientId, strTipInfo, strTipInfo.GetLength());
}

void CTCPHandler::OnRecvData(const CTerminalData& TerminalData, CJsonObject& JsonData)
{
	int nRet = 0;
	ADEL_LOCK_CMD AdelLockCmd = TerminalData.GetADELCmd();

	CJsonObject JsonSendData;
	switch(AdelLockCmd)
	{
		case ADEL_LOCK_CMD_INIT:
				nRet = OnADELLockInit(TerminalData.GetSubData());
			break;
		case ADEL_LOCK_CMD_END:
				nRet = OnADELLockEnd();
			break;
		case ADEL_LOCK_CMD_CHANGE:
				nRet = OnADELLockChangeUser(TerminalData.GetSubData());
			break;
		case ADEL_LOCK_CMD_NEW:
				nRet = OnADELLockNewKey(TerminalData.GetSubData());
			break;
		case ADEL_LOCK_CMD_ERASE:
				nRet = OnADELLockEraseCard(TerminalData.GetSubData(), JsonSendData);
			break;
		case ADEL_LOCK_CMD_READCARD:
				nRet = OnADELLockReadCard(JsonSendData);
			break;
		case ADEL_LOCK_CMD_READID:
				nRet = OnADELLockReadCardId(JsonSendData);
			break;
		default:
				nRet = ERROR_VERSION;
			break;
	}

	JsonData.SetValue(SERVER_SEND_VER, 1);
	JsonData.SetValue(SERVER_SEND_CMD, AdelLockCmd);
	JsonData.SetValue(SERVER_SEND_RESULT, nRet);
	// 如果要给客户端传输内容才回复以下内容
	if (AdelLockCmd == ADEL_LOCK_CMD_READCARD || 
			AdelLockCmd == ADEL_LOCK_CMD_ERASE || 
			AdelLockCmd == ADEL_LOCK_CMD_READID)
		JsonData.SetValue(SERVER_SEND_DATA, &JsonSendData);
}

int CTCPHandler::OnADELLockInit(LPCTSTR lpszInitData)
{
	CJsonObject JsonInit;
	JsonInit.FromString(lpszInitData);
	CString strSQLAddr, strUserName;
	int nSoftware = 30, nPort = 0, nEncoder = 0, nTMEncoder = 5;

	if (JsonInit.IsMember(ADEL_INIT_SOFTWARE) != FALSE)
		JsonInit.GetValue(ADEL_INIT_SOFTWARE, &nSoftware);
	if (JsonInit.IsMember(ADEL_INIT_SQLADDR) != FALSE)
		strSQLAddr = JsonInit.GetValue(ADEL_INIT_SQLADDR);
	if (JsonInit.IsMember(ADEL_INIT_USERNAME) != FALSE)
		strUserName = JsonInit.GetValue(ADEL_INIT_USERNAME);
	if (JsonInit.IsMember(ADEL_INIT_PORT) != FALSE)
		JsonInit.GetValue(ADEL_INIT_PORT, &nPort);
	if (JsonInit.IsMember(ADEL_INIT_ENCODER) != FALSE)
		JsonInit.GetValue(ADEL_INIT_ENCODER, &nEncoder);
	if (JsonInit.IsMember(ADEL_INIT_TMENCODER) != FALSE)
		JsonInit.GetValue(ADEL_INIT_TMENCODER, &nTMEncoder);

	return m_pADELLock->Init(nSoftware, strSQLAddr, strUserName, nPort, nEncoder, nTMEncoder);
}

int CTCPHandler::OnADELLockEnd()
{
	return m_pADELLock->EndSession();
}

int CTCPHandler::OnADELLockChangeUser(LPCTSTR lpszChangeData)
{
	CJsonObject JsonChange;
	JsonChange.FromString(lpszChangeData);
	CString strUserName;

	if (JsonChange.IsMember(ADEL_CHANGE_USERNAME) != FALSE)
		strUserName = JsonChange.GetValue(ADEL_CHANGE_USERNAME);

	return m_pADELLock->ChangeUser(strUserName);
}

int CTCPHandler::OnADELLockNewKey(LPCTSTR lpszNewData)
{
	CJsonObject JsonNew;
	JsonNew.FromString(lpszNewData);

	CString strRoom, strGate, strSTime, strGName, strGID, strLift, strTrack1, strTrack2;
	int nOverFlag = 0, nBreakfast = 0;
	LONG lCardNo = 0;

	if (JsonNew.IsMember(ADEL_NEW_ROOM) != FALSE)
		strRoom = JsonNew.GetValue(ADEL_NEW_ROOM);
	if (JsonNew.IsMember(ADEL_NEW_GATE) != FALSE)
		strGate = JsonNew.GetValue(ADEL_NEW_GATE);
	if (JsonNew.IsMember(ADEL_NEW_STIME) != FALSE)
		strSTime = JsonNew.GetValue(ADEL_NEW_STIME);
	if (JsonNew.IsMember(ADEL_NEW_GNAME) != FALSE)
		strGName = JsonNew.GetValue(ADEL_NEW_GNAME);
	if (JsonNew.IsMember(ADEL_NEW_GID) != FALSE)
		strGID = JsonNew.GetValue(ADEL_NEW_GID);
	if (JsonNew.IsMember(ADEL_NEW_LIFT) != FALSE)
		strLift = JsonNew.GetValue(ADEL_NEW_LIFT);
	if (JsonNew.IsMember(ADEL_NEW_TRACK1) != FALSE)
		strTrack1 = JsonNew.GetValue(ADEL_NEW_TRACK1);
	if (JsonNew.IsMember(ADEL_NEW_TRACK2) != FALSE)
		strTrack2 = JsonNew.GetValue(ADEL_NEW_TRACK2);
	if (JsonNew.IsMember(ADEL_NEW_OVERFLAG) != FALSE)
		JsonNew.GetValue(ADEL_NEW_OVERFLAG, &nOverFlag);
	if (JsonNew.IsMember(ADEL_NEW_BREAKFAST) != FALSE)
		JsonNew.GetValue(ADEL_NEW_BREAKFAST, &nBreakfast);
	if (JsonNew.IsMember(ADEL_NEW_CARDNO) != FALSE)
		JsonNew.GetValue(ADEL_NEW_CARDNO, (int*)&lCardNo);

	return m_pADELLock->NewKey(strRoom, strGate, strSTime, strGName, strGID, strLift,
														nOverFlag, nBreakfast, &lCardNo, strTrack1, strTrack2);
}

int CTCPHandler::OnADELLockEraseCard(LPCTSTR lpszEraseData, CJsonObject& JsonData)
{
	CJsonObject JsonErase;
	JsonErase.FromString(lpszEraseData);
	CString strTrack1, strTrack2;
	int nCardNo = 0, nfpIndex = 0;

	if (JsonErase.IsMember(ADEL_ERASE_CARDNO) != FALSE)
		JsonErase.GetValue(ADEL_ERASE_CARDNO, &nCardNo);

	if (JsonErase.IsMember(ADEL_ERASE_TRACK1) != FALSE)
		strTrack1 = JsonErase.GetValue(ADEL_ERASE_TRACK1);
	if (JsonErase.IsMember(ADEL_ERASE_TRACK2) != FALSE)
		strTrack2 = JsonErase.GetValue(ADEL_ERASE_TRACK2);

	return m_pADELLock->EraseCard(nCardNo, strTrack1, strTrack2);
}

int CTCPHandler::OnADELLockReadCard(CJsonObject& JsonReadCard)
{
	CARD_DATA CardData;
	int nRet = m_pADELLock->ReadCard(&CardData);

	JsonReadCard.SetValue(ADEL_SEND_READ_ROOM, CardData.strRoom);
	JsonReadCard.SetValue(ADEL_SEND_READ_GATE, CardData.strGate);
	JsonReadCard.SetValue(ADEL_SEND_READ_STIME, CardData.strVaildTime);
	JsonReadCard.SetValue(ADEL_SEND_READ_GNAME, CardData.strGuestName);
	JsonReadCard.SetValue(ADEL_SEND_READ_GID, CardData.strGuestId);
	/*JsonReadCard.SetValue(ADEL_SEND_READ_OVERFLAG, CardData.CardState)*/

	JsonReadCard.SetValue(ADEL_SEND_READ_CARDNO, CardData.lCardNo);
	JsonReadCard.SetValue(ADEL_SEND_READ_LIFT, CardData.strElevator);
	JsonReadCard.SetValue(ADEL_SEND_READ_STATE, CardData.CardState);
	JsonReadCard.SetValue(ADEL_SEND_READ_BREAKFAST, CardData.nBreakfast);
	if (CardData.strResvered1.IsEmpty() == FALSE)
		JsonReadCard.SetValue(ADEL_SEND_READ_TRACK1, CardData.strResvered1);
	if (CardData.strResvered2.IsEmpty() == FALSE)
		JsonReadCard.SetValue(ADEL_SEND_READ_TRACK2, CardData.strResvered2);

	return nRet;
}

int CTCPHandler::OnADELLockReadCardId(CJsonObject& JsonReadCardId)
{
	LONG lCardId = 0;
	int nRet = m_pADELLock->ReadCardId(&lCardId);

	JsonReadCardId.SetValue(ADEL_SEND_READ_CARDID, lCardId);

	return nRet;
}