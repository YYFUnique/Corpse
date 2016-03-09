#pragma once
#include "UIListCtrl.h"

typedef enum tagOBJ_TYPE
{
	OBJ_TYPE_HBITMAP = 0,		//位图文件句柄
	OBJ_TYPE_ICON = 1,				//图标文件
	OBJ_TYPE_FILE = 2,					//其他可显示文件
}OBJ_TYPE;

class CToolBarUI : public CButtonUI, public IListCtrlItemUI
{
public:
	CToolBarUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	bool Select(bool bSelect = true);

	void DoEvent(TEventUI& event);
	//void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);
public:
	void SetObjType(OBJ_TYPE ObjType);
	void SetFilePath(LPCTSTR lpszFilePath);
	CDuiString GetFilePath();
	void DrawHBitmap(HDC hDC);
	void DrawIcon(HDC hDC);
	void DrawFileIcon(HDC hDC);
	void ReleaseResource();
	void GetHBitmapPosition(HBITMAP hBitmap);
	void GetIconPosition(HICON hIcon);
	void GetPicPosition(int nPicWidth,int nPicHeight);
	void PaintForeImage(HDC hDC);
	void SetPadding(RECT rcPad);
protected:
	HICON m_hFileIcon;
	HDC		m_hMemDC;
	OBJ_TYPE m_ObjType;
	SIZE m_szLeftTop;
	BITMAP m_BMInf;
	CDuiString m_sFilePath;
	SHFILEINFO	m_ShFileInfo;
	RECT		m_rcPadding;
};