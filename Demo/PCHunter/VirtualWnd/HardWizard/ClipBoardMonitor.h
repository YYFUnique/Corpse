#pragma once

typedef enum tgaCLIPBOARDTYPE
{
	CLIPBOARDTYPE_Text = 0,	//文本内容
	CLIPBOARDTYPE_Img,			//图片内容
	CLIPBOADRTYPE_File,
	CLIPBOADRTYPE_Execute,
	CLIPBOADRTYPE_Folder,
	CLIPBOADRTYPE_Disk,
}CLIPBOADRTYPE;

class CClipBoardMonitor : public CNotifyPump
{
public:
	CClipBoardMonitor();
	~CClipBoardMonitor();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	void OnDrawClipboard();
	void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
protected:
	void ShowClipBoardData();
	HICON GetFileTypeIcon(LPCTSTR lpszSrcFilePath);
	void InsertItemToList(LPCTSTR lpszClipData, CLIPBOADRTYPE ClipBoardType);
	CString ConvertClipBoardTypeToString(CLIPBOADRTYPE ClipBoardType);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnLoadItem(TNotifyUI& msg);
private:
	BOOL						m_bFirstRun;
	HWND						m_hNextClipboard;
	CControlBuilder		m_RootBuilder;
	CDialogBuilder		m_DialogBuilder;
	CPaintManagerUI*	m_pPaintManager;
};