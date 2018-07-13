#pragma once

#include "libMain.h"

typedef enum tagCARD_STATE
{
	CARD_STATE_NORMAL		= 1,	// 正常使用
	CARD_STATE_LOGOUT		=	3,	// 正常注销
	CARD_STATE_LOST				=	4,	// 遗失注销
	CARD_STATE_BROKEN		=	5,	// 损毁注销
	CARD_STATE_AUTO			=	6,	//	自动注销
}CARD_STATE;

typedef enum tagOPEN_WAY
{
	OPEN_WAY_CARD							=	1,	//	仅使用卡开门
	OPEN_WAY_CARD_OR_FIGNER		=	2,	//	使用卡或者指纹开门
	OPEN_WAY_CARD_FIGNER				=	3,	// 仅使用指纹开门
	OPEN_WAY_CARD_AND_FIGNER	= 4,	//	使用卡和指纹开门
}OPEN_WAY;

typedef struct tagCARD_DATA
{
	CString		strRoom;									// 房号信息
	CString		strGate;									//	通道信息
	CString		strVaildTime;							//	有效时间
	CString		strGuestName;						//	客人姓名
	CString		strGuestId;								//	客人id
	CString		strResvered1;							//	保留
	CString		strResvered2;							//	保留
	CString		strElevator;								//	多路电梯
	LONG		lCardNo;									//	卡ID
	CARD_STATE CardState;							//	卡状态
	OPEN_WAY OpenWay;							//	开门方式
	int			nBreakfast;								//	早餐信息
}CARD_DATA,*PCARD_DATA;

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

	int ReadCard(CARD_DATA* pCardData);
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