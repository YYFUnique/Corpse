
// CrackUrlDlg.h : ͷ�ļ�
//

#pragma once
typedef struct _PACKET_ITEM_HTTP_INFO
{
	LPCSTR lpszHttpRequest;//Get��Post
	ULONG dwHttpRequestLen;//����������
	LPCSTR lpszUrlPath;//ҳ��·��
	ULONG dwUrlPathLen;//ҳ��·��
	LPCSTR lpszHostName;//��������Ϣ(����Ϊ��,�����ս��)
	ULONG dwHostNameLen;//����������
	LPCSTR lpszReferer;//������Ϣ(����Ϊ��,�����ս��)
	ULONG dwRefererLen;//������Ϣ����
	LPCSTR lpszUserAgent;//����������ʶ(����Ϊ��,�����ս��)
	ULONG dwUserAgentLen;//����������ʶ����
}PACKET_ITEM_HTTP_INFO , *PPACKET_ITEM_HTTP_INFO;
#include <WinIoCtl.h>
//#include "../../Driver/TestDriver/ControlCode.h"
#define KPH_CTL_CODE(x) CTL_CODE(FILE_DEVICE_NULL, 0x800 + x, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define KPH_WHITE_URL_ACC		KPH_CTL_CODE(0)
// CCrackUrlDlg �Ի���
class CCrackUrlDlg : public CDialog
{
// ����
public:
	CCrackUrlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CRACKURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	BOOL LsCrackUrl( 
			__in     LPCTSTR lpszUrl,					//��Ҫ������URL��ַ
			__in     DWORD dwUrlLength,			//��Ҫ������URL��ַ����
			__inout  PPACKET_ITEM_HTTP_INFO lpPacketItemHttpInfo //���ؽ����ַ����Ľṹ��
	);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPrase();
};
