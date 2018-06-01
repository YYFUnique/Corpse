#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
{
public:
    ~CRenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;

    static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
    static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
    static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
    static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderEngine
{
public:
	static BOOL SaveBitmapFile(HBITMAP hBitmap,LPCTSTR lpszFileName);
    static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
	static void DrawWinBorder(HDC dc,RECT rcClient/*,HWND hMainWnd*/);
    static TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
	static TImageInfo* LoadImage(LPBYTE pData, DWORD dwSize, DWORD mask);
	static HBITMAP ConvertIconToBitmap(HICON hIcon, int* x, int* y);
    static void FreeImage(TImageInfo* bitmap, BOOL bDelete);
    static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
        const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, 
        bool hole = false, bool xtiled = false, bool ytiled = false);
	static bool DrawImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rcControl, const RECT& rcPaint, CImageAttribute& image);
	static bool DrawImageInfo(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, const TDrawInfo* pDrawInfo);
    static bool DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, 
        LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
    static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

    // 以下函数中的颜色参数alpha值无效
    static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
    static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);

    static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
        DWORD dwTextColor, int iFont, UINT uStyle);
    static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
        DWORD dwTextColor, RECT* pLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle);
    static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc);
	static SIZE GetTextSize(HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle);
	static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
	static void DrawColor(HDC hDC, const RECT& rc, DWORD color,SIZE BorderRound,long bTopLeftRound=1,long bTopRightRound=1,long bBottomRightRound=1,long bBottomLeftRound=1);
	static void DrawPolygon(HDC hDC,POINT * pPoints,int nNumber,DWORD dwFillColor,DWORD dwBorderColor=0,int mode=0);//0 Fill 1:border 2:border and fill
	static void Draw3dRect(HDC hDC,const RECT& rect,DWORD clrTopLeft, DWORD clrBottomRight);
	static void CheckAlphaColor(DWORD& dwColor);
	static void ClearAlphaPixel(LPBYTE pBits, int bitsWidth, LPRECT rc);
	static void RestoreAlphaColor(LPBYTE pBits, int bitsWidth, LPRECT rc);
	static void SetAlphaColor(LPBYTE pBits, int bitsWidth, PRECT rc, BYTE ThisA);
	static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits);
	static void AdjustImage(bool bUseHSL, TImageInfo* pImageInfo, short H, short S, short L);
#ifdef RENDER_GDIPLUS
	static void DrawGradient(HDC hDC, const RECT& rc, DWORD *dwColors, float*fPositions, int colorCount,bool bVertical,SIZE borderRound,long bTopLeftRound=1,long bTopRightRound=1,long bBottomRightRound=1,long bBottomLeftRound=1);
#endif
};

} // namespace DuiLib

#endif // __UIRENDER_H__
