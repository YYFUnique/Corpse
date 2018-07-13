#pragma once

#include "libMain.h"

class CADELLock
{
public:
	CADELLock();
	void Release();
public:
	int Init(LPCTSTR lpszSQLAddr, int nPort);
	int EndSession();
	int ChangeUser(LPCTSTR lpszAdmin);
	int NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int ReadCard(LPTSTR lpsz);
	int EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, int* pFinger);
	int ReadCardId(LONG* lpCardNo);
	int ReadCardData(int nCardType, int nStart, int nLen, CString& strData);
	int WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData);
protected:
	~CADELLock();
private:
	HMODULE			m_hModule;
	FN_INIT				m_pfnInit;							// 初始化。
	FN_EndSession	m_pfnEndSession;				//	结束工作期。
	FN_ChangeUser	m_pfnChangeUser;				//	更改操作员。
	FN_NewKey			m_pfnNewKey;					//	发行新客人卡。
	FN_DupKey			m_pfnDupKey;						//	“复制”客人卡
	FN_ReadCard		m_pfnReadCard;					//	读卡。
	FN_EraseCard		m_pfnEraseCard;					//	注销卡。
	FN_ReadId			m_pfnReadId;						// 读取卡片的ID。
	FN_ReadCardData	m_pfnReadCardData;		// 读取卡片中数据。
	FN_WriteCardData	m_pfnWriteCardData;		// 卡片写入数据。
};