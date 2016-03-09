
// ClipBoardMontorDlg.h : ͷ�ļ�
//

#pragma once

typedef enum tgaMENULIST
{
	MENULIST_TrayMenu = 0,	//����ͼ��˵�
	MENULIST_ListMenu,			//�Ի���˵�
	MENULIST_SendToMenu,
}MENULIST;

typedef enum tgaCLIPBOARDTYPE
{
	CLIPBOARDTYPE_Text = 0,	//�ı�����
	CLIPBOARDTYPE_Img,			//ͼƬ����
	CLIPBOADRTYPE_File,
	CLIPBOADRTYPE_Execute,
	CLIPBOADRTYPE_Folder,
	CLIPBOADRTYPE_Disk,
}CLIPBOADRTYPE;

typedef struct tgaMENUINFOEX
{
	DWORD dwMenuId;
	ULONG_PTR dwParam;
}MENUINFOEX,*PMENUINFOEX;

typedef CList<MENUINFOEX,MENUINFOEX&> CMenuInfoList;
// CClipBoardMontorDlg �Ի���
class CClipBoardMontorDlg : public CDialog
{
// ����
public:
	CClipBoardMontorDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CClipBoardMontorDlg();
// �Ի�������
	enum { IDD = IDD_CLIPBOARDMONTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:
	BOOL GetSystemSendToList(HWND hWnd, HMENU hMenu);
	BOOL ShowClipBoardData();
	HMENU GetSendToSubMenuHandle(CMenu* pMainMenu);
	BOOL InsertItemToList(LPCTSTR lpszClipData,CLIPBOADRTYPE ClipBoardType);
	CString ConvertClipBoardTypeToString(CLIPBOADRTYPE ClipBoardType);
	ULONG_PTR  GetMenuItemData(UINT nMenuID);
	CLIPBOADRTYPE GetFileType(LPCTSTR lpszFilePath);
	HBITMAP ConvertIconToBitmap(HICON hIcon);
// ʵ��
protected:
	BOOL m_RunFirst;
	CImageList m_ImgList;
	HICON m_hIcon;
	HWND m_hNextClipboard;
	CListCtrl m_ListCtrl;
	CMenuInfoList m_MenuInfoList;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();
	afx_msg BOOL OnDBLClick(UINT nID,NMHDR* pNMHDR,LRESULT* pLResult);
	afx_msg BOOL OnRClick(UINT nID,NMHDR* pNMHDR,LRESULT* pLResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuCopy();
	afx_msg void OnMenuDelete();
	afx_msg void OnSendTo(UINT nID);
};
