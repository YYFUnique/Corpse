#pragma once

#include "libMain.h"

typedef enum tagADEL_LOCK_CMD
{
	ADEL_LOCK_CMD_DEFAULT			=	101,						
	ADEL_LOCK_CMD_INIT					=	101,
	ADEL_LOCK_CMD_END					=	102,
	ADEL_LOCK_CMD_CHANGE			=	103,
	ADEL_LOCK_CMD_NEW					=	104,
	ADEL_LOCK_CMD_DUP					=	105,
	ADEL_LOCK_CMD_READCARD		=	106,
	ADEL_LOCK_CMD_ERASE				=	107,
	ADEL_LOCK_CMD_READDATA		=	108,
	ADEL_LOCK_CMD_WRITEDATA		=	109,
	ADEL_LOCK_CMD_READID				=	110,
}ADEL_LOCK_CMD;

typedef enum tagCARD_STATE
{
	CARD_STATE_NORMAL		= 1,	// 正常使用
	CARD_STATE_LOGOUT		=	3,	// 正常注销
	CARD_STATE_LOST				=	4,	// 遗失注销
	CARD_STATE_BROKEN		=	5,	// 损毁注销
	CARD_STATE_AUTO			=	6,	//	自动注销
}CARD_STATE;

typedef enum tagCARD_TYPE
{
	CARD_TYPE_NONE				= 0,	//未定义
	CARD_TYPE_PRESIDENT		= 1,	// 总裁
	CARD_TYPE_VILLA				=	2,	// 别墅
	CARD_TYPE_MANAGE			= 3,	// 管理卡
	CARD_TYPE_CONTROL		= 4,	//总控卡
	CARD_TYPE_ROOM_TIME	= 5,	// 时钟卡
	CARD_TYPE_LEADER			=	6, // 邻班卡
	CARD_TYPE_FLOOR				=	7,	// 楼层卡
	CARD_TYPE_CLEAR				=	8, // 清洁卡
	CARD_TYPE_CUSTOM			=	9, // 客人卡
	CARD_TYPE_FINISH				=10, // 终止卡
	CARD_TYPE_MEETING			=11, // 会议卡
	CARD_TYPE_EMERGENCY	=12, // 应急卡
	CARD_TYPE_CHECK_OUT	=13, // 退房卡
	CARD_TYPE_RESVERED		=14, // 备用
}CARD_TYPE;

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
	//OPEN_WAY OpenWay;							//	开门方式
	int			nBreakfast;								//	早餐信息
}CARD_DATA,*PCARD_DATA;

typedef struct tagBUFF_INFO
{
	CString			strCardData;		// 卡上数据
	int				CardType;		// 卡类型
	int				CardState;		// 卡状态
	int				lCardNo;			// 卡编号
	CString			strRoomNum;	// 卡上对应的房号
	CString			strGName;		//	客人姓名
	CString			strValidTime;	// 卡对应的有效期
}BUFF_INFO, *PBUFF_INFO;

class CADELLock
{
public:
	CADELLock();
	void Release();
public:
	int Init(int nSoftware, LPCTSTR lpszSQLAddr, LPCTSTR lpszAdmin, int nPort, int nEncoder = 0, int nTMEncoder = 5);
	int EndSession();
	int ChangeUser(LPCTSTR lpszAdmin);
	int NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, LPCTSTR lpszLift, BOOL bCovered, int nBreakfast, LONG* pCardNo,
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);

	int DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, LPCTSTR lpszLift, BOOL bCovered, int nBreakfast, LONG* pCardNo,
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);

	int CheckOut(LPCTSTR lpszRoom, LONG lCardno);
	int LostCard(LPCTSTR lpszRoom, LONG lCardno);
	int PopCard(void);
	int SetPort(int nSoftware, int nPort, int nEncoder, int nTMEncoder);
	int ReadCard(CARD_DATA* pCardData);
	int EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);
	int ReadCardId(LONG* lpCardNo);
	int CheckSC(LPCTSTR lpszSC);

	int ReadIC(UINT nStart, UINT nLen, LPCTSTR lpszData);
	int WriteIC(UINT nStart, UINT nLen, LPCTSTR lpszData);

	int WriteMagCard(LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszTrack3);
	int ReadMagCard(LPTSTR lpszTrack1, LPTSTR lpszTrack2, LPTSTR lpszTrack3);

	int Reader_Beep(int Sound);

	int GetBuffInfo(BUFF_INFO* pBuffInfo);
	int GetMagBuffInfo(BUFF_INFO* pBuffInfo);

	int GetCardInfo(BUFF_INFO* pBuffInfo);
	//int ReadCardData(int nCardType, int nStart, int nLen, CString& strData);
	//int WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData);
protected:
	~CADELLock();
private:
	HMODULE			m_hModule;
	FN_INIT				m_pfnInit;							// 初始化。
	FN_EndSession	m_pfnEndSession;				//	结束工作期。
	FN_ChangeUser	m_pfnChangeUser;				//	更改操作员。
	FN_NewKey			m_pfnNewKey;					//	发行新客人卡。
	FN_DupKey			m_pfnDupKey;						//	“复制”客人卡
	//FN_AddKey			m_pfnAddKey;						//	“复制”客人卡
	FN_ReadCard		m_pfnReadCard;					//	读卡。
	FN_EraseCard		m_pfnEraseCard;					//	注销卡。
	FN_ReadId			m_pfnReadId;						// 读取卡片的ID。
	//FN_ReadCardData	m_pfnReadCardData;		// 读取卡片中数据。
	//FN_WriteCardData	m_pfnWriteCardData;		// 卡片写入数据。
	FN_CheckOut		m_pfnCheckOut;					//退房，仅更新数据库，不注销卡
	FN_LostCard		m_pfnLostCard;					// 遗失卡
	FN_PopCard		m_pfnPopCard;					// 弹出卡
	FN_SetPort			m_pfnSetPort;						// 设置端口
	FN_CheckSC		m_pfnCheckSC;					// 校验卡密码

	FN_ReadIC			m_pfnReadIC;						//	读取卡数据
	FN_WriteIC			m_pfnWriteIC;						//	写卡数据

	FN_WriteMagCard	m_pfnWriteMagCard;		//写磁卡轨
	FN_ReadMagCard	m_pfnReadMagCard;		// 读磁卡

	FN_ReaderBeep		m_pfnReaderBeep;				// 蜂鸣器

	FN_GetBuffInfo		m_pfnGetBuffInfo;				
	FN_GetMagBuffInfo m_pfnGetMagBuffInfo;	

	FN_GetCardInfo		m_pfnGetCardInfo;
};