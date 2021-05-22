#pragma once

class CBuddyMsg
{
public:
	CBuddyMsg();
	~CBuddyMsg();

public:
	static BOOL IsType(LPCTSTR lpszType);
	void ReSet();

	BOOL Parse(LPCTSTR lpszJsonValue);
	BOOL IsShakeMsg();
protected:
	CONTENT_TYPE		m_nMsgType;
	UINT						m_nMsgId;
	UINT						m_nMsgId2;
	UINT						m_nFromUin;
	UINT						m_nToUin;
	UINT						m_nReplyIp;
	UINT64					m_nTime;
	CString						m_strNickName;
	CBuddyContent		m_arrContent;
};