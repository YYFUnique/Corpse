
// TrayMgrDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

typedef enum _TRAY_AREA
{
	Tray = 0,
	Overflow,
}TRAY_AREA;

typedef struct _TRAYDATAFIREWALL
{	
	HWND hwnd;
	UINT uID;				 
	UINT uCallbackMessage;	 
	DWORD Reserved[2];		
	HICON hIcon;	         //����ͼ��ľ��		 	
}TRAYDATAFIREWALL,*PTRAYDATAFIREWALL;

typedef struct _TRAYDATAINFO
{
	CString strTitle;					//������ʾ����
	UINT hTrayIcon;				//����ͼ��
	HWND hWnd;						//���ھ��
	UINT uID;								//ͼ��ID
	UINT uBtnID;
	UINT uCallbackMessage;	//�ص�������Ϣ��
	BOOL bShowTray;				//�Ƿ���ʾͼ��
	UINT bTrayIcon;					//����ͼ��
	TRAY_AREA Pos;
}TRAYDATAINFO,*PTRAYDATAINFO;

typedef CList<TRAYDATAINFO,TRAYDATAINFO&> CTRAYDATAINFO;
// CTrayMgrDlg �Ի���
class CTrayMgrDlg : public CDialog
{
// ����
public:
	CTrayMgrDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CString ConvertDwordToString(DWORD dwNum,BOOL bHex  = TRUE);
	CString ConvertDwordToMessageString(DWORD dwMessageID);
	UINT SetTrayIcon(HICON hIcon);
	void PostMessageToTrayIcon(LPARAM lParam);
	void PostMoveMessage(UINT uOldPos,UINT uNewPos);
// �Ի�������
	enum { IDD = IDD_TRAYMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	BOOL GetTrayAreaIcon();
	BOOL GetOverflowAreaIcon();
	BOOL GetIcon(HWND hWnd,int nItemCount,TRAY_AREA TrayArea);
// ʵ��
protected:
	HICON m_hIcon;
	CListCtrl m_ListCtrl;
	CImageList m_Img;
	CTRAYDATAINFO m_TrayDataInfo;		
	HWND m_hWndTray[2];
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnListTrayIcon(WPARAM wParam , LPARAM lParam);
	afx_msg void OnShowTrayIcon();
	afx_msg void OnLBtnDown();
	afx_msg void OnRBtnClick();
	afx_msg void OnLDBDown();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnRClick(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
};
