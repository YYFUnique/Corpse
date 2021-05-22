#pragma once

#include <TextServ.h>
#include <Richole.h>
#include <TOM.h>
namespace DuiLib
{
	const LONG cInitTextMax = (32 * 1024) - 1;

	#ifndef LY_PER_INCH
	#define LY_PER_INCH 1440
	#endif

	#ifndef HIMETRIC_PER_INCH
	#define HIMETRIC_PER_INCH 2540
	#endif

	class UILIB_API CWinTextHost : public ITextHost
	{
	public:
		CWinTextHost();
		BOOL Init(CControlUI* pControl , const CREATESTRUCT *pcs);
		virtual ~CWinTextHost();

		ITextServices* GetTextServices(void) const { return m_pTextSrv; }
		void SetClientRect(RECT *prc);
		RECT* GetClientRect() { return &rcClient; }
		BOOL GetWordWrap(void) { return fWordWrap; }
		void SetWordWrap(BOOL fWordWrap);
		BOOL GetReadOnly();
		void SetReadOnly(BOOL fReadOnly);
		void SetFont(HFONT hFont);
		void SetColor(DWORD dwColor);
		SIZEL* GetExtent();
		void SetExtent(SIZEL *psizelExtent);
		void LimitText(LONG nChars);
		BOOL IsCaptured();

		BOOL GetAllowBeep();
		void SetAllowBeep(BOOL fAllowBeep);
		WORD GetDefaultAlign();
		void SetDefaultAlign(WORD wNewAlign);
		BOOL GetRichTextFlag();
		void SetRichTextFlag(BOOL fNew);
		LONG GetDefaultLeftIndent();
		void SetDefaultLeftIndent(LONG lNewIndent);
		BOOL SetSaveSelection(BOOL fSaveSelection);
		HRESULT OnTxInPlaceDeactivate();
		HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
		BOOL GetActiveState(void) { return fInplaceActive; }
		BOOL DoSetCursor(RECT *prc, POINT *pt);
		void SetTransparent(BOOL fTransparent);
		void GetControlRect(LPRECT prc);
		LONG SetAccelPos(LONG laccelpos);
		WCHAR SetPasswordChar(WCHAR chPasswordChar);
		void SetDisabled(BOOL fOn);
		LONG SetSelBarWidth(LONG lSelBarWidth);
		BOOL GetTimerState();

		void SetCharFormat(CHARFORMAT2W &c);
		void SetParaFormat(PARAFORMAT2 &p);
		ITextHost * GetTextHost()
		{
			AddRef();
			return this;
		}

		ITextServices * GetTextServices2()
		{
			if (NULL == m_pTextSrv)
				return NULL;
			m_pTextSrv->AddRef();
			return m_pTextSrv;
		}

		ITextDocument* GetTextDocument()
		{
			if (NULL == m_pDocument)
				return NULL;
			m_pDocument->AddRef();
			return m_pDocument;
		}

		BOOL SetOleCallback(IRichEditOleCallback* pCallback)
		{
			if (NULL == m_pTextSrv)
				return FALSE;
			HRESULT lRes = 0;
			m_pTextSrv->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback, &lRes);
			return (BOOL)lRes;
		}

		BOOL CanPaste(UINT nFormat = 0)
		{
			if (NULL == m_pTextSrv)
				return FALSE;
			HRESULT lRes = 0;
			m_pTextSrv->TxSendMessage(EM_CANPASTE, nFormat, 0L, &lRes);
			return (BOOL)lRes;
		}

		void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0)
		{
			if (NULL == m_pTextSrv)
				return;
			REPASTESPECIAL reps = { dwAspect, (DWORD_PTR)hMF };
			m_pTextSrv->TxSendMessage(EM_PASTESPECIAL, uClipFormat, (LPARAM)&reps, NULL);
		}

		// -----------------------------
		//	IUnknown interface
		// -----------------------------
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		// -----------------------------
		//	ITextHost interface
		// -----------------------------
		virtual HDC TxGetDC();
		virtual INT TxReleaseDC(HDC hdc);
		virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
		virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
		virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
		virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
		virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
		virtual void TxViewChange(BOOL fUpdate);
		virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
		virtual BOOL TxShowCaret(BOOL fShow);
		virtual BOOL TxSetCaretPos(INT x, INT y);
		virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
		virtual void TxKillTimer(UINT idTimer);
		virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
		virtual void TxSetCapture(BOOL fCapture);
		virtual void TxSetFocus();
		virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
		virtual BOOL TxScreenToClient (LPPOINT lppt);
		virtual BOOL TxClientToScreen (LPPOINT lppt);
		virtual HRESULT TxActivate( LONG * plOldState );
		virtual HRESULT TxDeactivate( LONG lNewState );
		virtual HRESULT TxGetClientRect(LPRECT prc);
		virtual HRESULT TxGetViewInset(LPRECT prc);
		virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
		virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
		virtual COLORREF TxGetSysColor(int nIndex);
		virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
		virtual HRESULT TxGetMaxLength(DWORD *plength);
		virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
		virtual HRESULT TxGetPasswordChar(TCHAR *pch);
		virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
		virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
		virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
		virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
		virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
		virtual HRESULT TxNotify(DWORD iNotify, void *pv);
		virtual HIMC TxImmGetContext(void);
		virtual void TxImmReleaseContext(HIMC himc);
		virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

	private:
		//	CRichEditUI *m_re;
		CControlUI*		m_pControl;
		ULONG			m_cRefs;					// Reference Count
		ITextServices	*m_pTextSrv;		    // pointer to Text Services object
		ITextDocument* m_pDocument;
		// Properties

		DWORD		dwStyle;				// style bits

		unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
		unsigned	fWordWrap			:1;	// Whether control should word wrap
		unsigned	fAllowBeep			:1;	// Whether beep is allowed
		unsigned	fRich				:1;	// Whether control is rich text
		unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
		unsigned	fInplaceActive		:1; // Whether control is inplace active
		unsigned	fTransparent		:1; // Whether control is transparent
		unsigned	fTimer				:1;	// A timer is set
		unsigned    fCaptured           :1;

		LONG			lSelBarWidth;			// Width of the selection bar
		LONG  		cchTextMost;			// maximum text size
		DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
		LONG		icf;
		LONG		ipf;
		RECT		rcClient;				// Client Rect for this control
		SIZEL		sizelExtent;			// Extent array
		CHARFORMAT2W cf;					// Default character format
		PARAFORMAT2	pf;					    // Default paragraph format
		LONG		laccelpos;				// Accelerator position
		WCHAR		chPasswordChar;		    // Password character
	};

	HRESULT CreateHost(CControlUI* pControl, const CREATESTRUCT *pcs, CWinTextHost **pptec);
}