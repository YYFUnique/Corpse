
// TrayMgrDlg.h : 头文件
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
	HICON hIcon;	         //托盘图标的句柄		 	
}TRAYDATAFIREWALL,*PTRAYDATAFIREWALL;

typedef struct _TRAYDATAINFO
{
	CString strTitle;					//托盘提示文字
	UINT hTrayIcon;				//托盘图标
	HWND hWnd;						//窗口句柄
	UINT uID;								//图标ID
	UINT uBtnID;
	UINT uCallbackMessage;	//回调函数消息号
	BOOL bShowTray;				//是否显示图标
	UINT bTrayIcon;					//托盘图标
	TRAY_AREA Pos;
}TRAYDATAINFO,*PTRAYDATAINFO;

typedef CList<TRAYDATAINFO,TRAYDATAINFO&> CTRAYDATAINFO;
// CTrayMgrDlg 对话框
class CTrayMgrDlg : public CDialog
{
// 构造
public:
	CTrayMgrDlg(CWnd* pParent = NULL);	// 标准构造函数
	CString ConvertDwordToString(DWORD dwNum,BOOL bHex  = TRUE);
	CString ConvertDwordToMessageString(DWORD dwMessageID);
	UINT SetTrayIcon(HICON hIcon);
	void PostMessageToTrayIcon(LPARAM lParam);
	void PostMoveMessage(UINT uOldPos,UINT uNewPos);
// 对话框数据
	enum { IDD = IDD_TRAYMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL GetTrayAreaIcon();
	BOOL GetOverflowAreaIcon();
	BOOL GetIcon(HWND hWnd,int nItemCount,TRAY_AREA TrayArea);
// 实现
protected:
	HICON m_hIcon;
	CListCtrl m_ListCtrl;
	CImageList m_Img;
	CTRAYDATAINFO m_TrayDataInfo;		
	HWND m_hWndTray[2];
	// 生成的消息映射函数
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
