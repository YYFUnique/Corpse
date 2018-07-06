#pragma once

// 自定义表情信息
class CCustomFaceInfo
{
public:
	CCustomFaceInfo();
	~CCustomFaceInfo();
public:
protected:
	UINT		m_nFileId;
	CString		m_strKey;
	CString		m_strServer;

	// 发送自定义表情使用参数
	DWORD	m_dwFileSize;
	CString		m_strFileName;
	CString		m_strFilePath;
	//当是文件类型时，该字段代表文件是离线文件还是在线文件
	BOOL		m_bOnline;
};
// 消息内容
class CContent 
{
public:
	CContent();
	~CContent();

public:
protected:
	// 内容类型
	CONTENT_TYPE m_nType;
	// 字体信息
	CFontInfo		m_FontInfo;
	// 文本信息
	CString			m_strText;
	// 系统表情
	int				m_nFaceId;
	// 窗口抖动次数
	int				m_nShakeTime;
	// 自定义表情信息（当发送的是m_nType是文件时，m_CFaceInfo是文件的有关信息）
	CCustomFaceInfo	m_CFaceInfo;
};

typedef std::vector<CContent*> CBuddyContent;