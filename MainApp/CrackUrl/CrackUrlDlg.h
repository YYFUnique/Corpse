
// CrackUrlDlg.h : 头文件
//

#pragma once
typedef struct _PACKET_ITEM_HTTP_INFO
{
	LPCSTR lpszHttpRequest;//Get或Post
	ULONG dwHttpRequestLen;//主机名长度
	LPCSTR lpszUrlPath;//页面路径
	ULONG dwUrlPathLen;//页面路径
	LPCSTR lpszHostName;//主机名信息(可能为空,且无终结符)
	ULONG dwHostNameLen;//主机名长度
	LPCSTR lpszReferer;//引用信息(可能为空,且无终结符)
	ULONG dwRefererLen;//引用信息长度
	LPCSTR lpszUserAgent;//浏览器程序标识(可能为空,且无终结符)
	ULONG dwUserAgentLen;//浏览器程序标识长度
}PACKET_ITEM_HTTP_INFO , *PPACKET_ITEM_HTTP_INFO;
#include <WinIoCtl.h>
//#include "../../Driver/TestDriver/ControlCode.h"
#define KPH_CTL_CODE(x) CTL_CODE(FILE_DEVICE_NULL, 0x800 + x, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define KPH_WHITE_URL_ACC		KPH_CTL_CODE(0)
// CCrackUrlDlg 对话框
class CCrackUrlDlg : public CDialog
{
// 构造
public:
	CCrackUrlDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CRACKURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	BOOL LsCrackUrl( 
			__in     LPCTSTR lpszUrl,					//需要解析的URL地址
			__in     DWORD dwUrlLength,			//需要解析的URL地址长度
			__inout  PPACKET_ITEM_HTTP_INFO lpPacketItemHttpInfo //返回解析字符串的结构体
	);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPrase();
};
