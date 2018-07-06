#pragma once

// 好友消息记录
typedef struct tagBUDDY_MSG_LOG
{
	UINT		uID;
	UINT		uUTalkNum;
	CString		strNickName;
	UINT64	nTime;
	BOOL		bSendFlag;
	CString		strContent;
}BUDDY_MSG_LOG;

// 群消息记录
typedef struct tagGROUP_MSG_LOG
{
	UINT		nID;
	UINT		nGroupNum;
	UINT		nUTalkNum;
	CString		strNickName;
	UINT64	nTime;
	CString		strContent;
}GROUP_MSG_LOG;

class CMessageLog
{
public:
	CMessageLog();
	~CMessageLog();

public:
	// 获取消息记录文件名称
	CString GetMsgLogFileName();
	// 设置消息记录文件名称
	void SetMsgLogFileName(LPCTSTR lpszFileName);

	// 判断消息记录文件是否有效
	BOOL IsMsgLogFileValid();
	// 创建消息记录文件
	BOOL CreateMsgLogFile();
};