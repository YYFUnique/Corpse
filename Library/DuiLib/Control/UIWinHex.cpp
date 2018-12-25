#include "StdAfx.h"
#include "UIWinHex.h"

namespace DuiLib
{
	CWinHexUI::CWinHexUI()
		: m_bCaretOn(FALSE)
		, m_iFont(-1)
		, m_nHitTest(0)
		, m_uButtonState(0)
		, m_dwTextColor(0xFF0000EB)
		, m_dwCaretColor(0xFF1F1FFF)
		, m_dwBackColor(0xFFE0E0FF)
	{
		m_pWinHexData = new WINHEX_DATA;
		
		m_pWinHexData->hBuffer						= NULL;
		m_pWinHexData->uIndex						=	0;
		m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = 0;
		m_pWinHexData->uColumnsPerLine		= 16;
		m_pWinHexData->nLeftMargin				=	2;
		m_pWinHexData->nAddressSpaceing	=	2;
		m_pWinHexData->nSplitSpacing			=	4;
		m_pWinHexData->bHexField					=	TRUE; /* in hexdump field*/
		m_pWinHexData->bInMid						= FALSE;

		Update();
	}

	CWinHexUI::~CWinHexUI()
	{
		if (m_pWinHexData->hBuffer)
			LocalFree(m_pWinHexData->hBuffer);

		delete m_pWinHexData;
	}

	LPCTSTR CWinHexUI::GetClass() const
	{
		return _T("WinHexUI");
	}

	UINT CWinHexUI::GetControlFlags() const
	{
		if (IsEnabled() == FALSE) return CVerticalLayoutUI::GetControlFlags();
		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	LPVOID CWinHexUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_WINHEX) == 0)
			return static_cast<CWinHexUI*>(this);
		else
			return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CWinHexUI::DoInit()
	{
		if (m_pWinHexData->uLineHeight == 0 || m_pWinHexData->uCharWidth == 0)
		{
			TFontInfo* pFontInfo = m_pManager->GetFontInfo(m_iFont);
			m_pWinHexData->uLineHeight = pFontInfo->tm.tmHeight;
			m_pWinHexData->uCharWidth = pFontInfo->tm.tmAveCharWidth;
		}
		
		CVerticalLayoutUI::DoInit();
	}

	void CWinHexUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_TIMER) {
			m_bCaretOn = !m_bCaretOn;
			RECT rcCaret = {m_ptCaret.x, m_ptCaret.y, m_ptCaret.x + m_pWinHexData->uCharWidth, m_ptCaret.y + m_pWinHexData->uLineHeight};
			InvalidateRect(m_pManager->GetPaintWindow() ,&rcCaret, FALSE);
		} else if (event.Type == UIEVENT_SETFOCUS) {
			m_bFocused = TRUE;

			m_pManager->SetTimer(this, IME_BLINK_TIMER_ID, GetCaretBlinkTime());
			//CreateCaret(m_pManager->GetPaintWindow(), NULL, 1, m_pWinHexData->uLineHeight);
			Invalidate();
		} else if (event.Type == UIEVENT_KILLFOCUS) {
			m_bCaretOn = FALSE;
			m_bFocused = FALSE;

			m_pManager->KillTimer(this, IME_BLINK_TIMER_ID);
			//DestroyCaret();
			Invalidate();
		} else if (event.Type == UIEVENT_RBUTTONUP) {
			m_uButtonState = 0;
			m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
			MoveCaret(TRUE);
			Invalidate();
		}else if (event.Type == UIEVENT_BUTTONDOWN) {
			BOOL bNewField;
			POINT ptEdit;

			m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;

			if (event.wParam & MK_SHIFT) {
				m_pWinHexData->uIndex = GetIndexFromPoint(event.ptMouse, m_nHitTest, &ptEdit, &bNewField);
				m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
			} else {
				m_nHitTest = OnHitRegionTest(event.ptMouse);
				/*LOG_PRINT(_T("pt.x:%d, HitTest:%d"),event.ptMouse.x, dwHitTest);*/
				m_pWinHexData->uIndex = GetIndexFromPoint(event.ptMouse, m_nHitTest, &ptEdit, &bNewField);
				if (m_pWinHexData->nSelEnd == m_pWinHexData->nSelStart)
					m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
				m_pWinHexData->bHexField = bNewField;
			}

			m_pWinHexData->nCaretCol = ptEdit.x;
			m_pWinHexData->nCaretLine = ptEdit.y;

			MoveCaret(TRUE);
			Invalidate();
		} else if (event.Type == UIEVENT_BUTTONUP) {
			BOOL bNewField;
			POINT ptEdit;

			m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);

			m_nHitTest = OnHitRegionTest(event.ptMouse);
			m_pWinHexData->uIndex = GetIndexFromPoint(event.ptMouse, m_nHitTest, &ptEdit, &bNewField);

			m_pWinHexData->nCaretCol = ptEdit.x;
			m_pWinHexData->nCaretLine = ptEdit.y;
			m_pWinHexData->bHexField = bNewField;
			if (m_pWinHexData->nSelEnd == m_pWinHexData->nSelStart)
				m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
			MoveCaret(TRUE);
			Invalidate();
		} else if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				BOOL bNewField;
				POINT ptEdit;

				m_nHitTest = OnHitRegionTest(event.ptMouse);
				m_pWinHexData->uIndex = GetIndexFromPoint(event.ptMouse, m_nHitTest, &ptEdit, &bNewField);

				m_pWinHexData->nCaretCol = ptEdit.x;
				m_pWinHexData->nCaretLine = ptEdit.y;
				m_pWinHexData->bHexField = bNewField;
				m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
				MoveCaret(TRUE);
				Invalidate();
			} else {
				m_uButtonState &= ~ UISTATE_CAPTURED;
			}
		} else if (event.Type == UIEVENT_KEYDOWN) {
			OnEventKeyDown(event);
		} else if (event.Type == UIEVENT_CHAR) {
			OnEventChar(event);
		} else
			CVerticalLayoutUI::DoEvent(event);
	}

	void CWinHexUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("font")) == 0)
			SetFont(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else
			CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CWinHexUI::SetFont(int nFont)
	{
		if (m_iFont == nFont)
			return;

		m_iFont = nFont;

		TFontInfo* pFontInfo = m_pManager->GetFontInfo(m_iFont);
		m_pWinHexData->uLineHeight = pFontInfo->tm.tmHeight;
		m_pWinHexData->uCharWidth = pFontInfo->tm.tmAveCharWidth;

		Invalidate();
	}

	void CWinHexUI::SetTextColor(DWORD dwTextColor)
	{
		if (m_dwTextColor == dwTextColor)
			return;

		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	void CWinHexUI::Cut()
	{
		Copy();
		Delete();
	}

	void CWinHexUI::Copy()
	{
		CHARRANGE CharRange = GetSelection();

		DWORD dwSize = CharRange.cpMax - CharRange.cpMin;
		if (dwSize == 0)
			return;

		HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dwSize + sizeof(DWORD));
		if (hGlobal == NULL)
			return;
		
		PBYTE pb = (PBYTE)GlobalLock(hGlobal);
		if (pb)
		{
			LPBYTE pLockBuf = (PBYTE)LocalLock(m_pWinHexData->hBuffer);
			if (pLockBuf)
			{
				RtlCopyMemory(pb, pLockBuf + CharRange.cpMin, dwSize);
				LocalUnlock(m_pWinHexData->hBuffer);
			}
			GlobalUnlock(hGlobal);

			if (OpenClipboard(m_pManager->GetPaintWindow()))
			{
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hGlobal);
				CloseClipboard();
			}
		}
		else
			GlobalFree(hGlobal);
	}

	void CWinHexUI::Paste()
	{
		Delete();

		DWORD dwBufSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);

		if (OpenClipboard(m_pManager->GetPaintWindow()))
		{
			HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
			if (hGlobal == NULL)
				hGlobal = GetClipboardData(CF_TEXT);
			if (hGlobal == NULL)
				return;

			DWORD dwSize = GlobalSize(hGlobal);
			SetMaxBufferSize(dwBufSize + dwSize);
			PBYTE pLockBuf = (PBYTE)LocalLock(m_pWinHexData->hBuffer);
			if (pLockBuf)
			{
				RtlMoveMemory(pLockBuf + m_pWinHexData->uIndex + dwSize,
					pLockBuf + m_pWinHexData->uIndex, dwBufSize - m_pWinHexData->uIndex);
				RtlCopyMemory(pLockBuf + m_pWinHexData->uIndex, hGlobal, dwSize);
				LocalUnlock(m_pWinHexData->hBuffer);
			}

			m_pWinHexData->uIndex = m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = 0;
			m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
			m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;

			GlobalUnlock(hGlobal);
			CloseClipboard();
		}
		
		MoveCaret(TRUE);
		Invalidate();
	}

	void CWinHexUI::Delete()
	{
		PBYTE pLockBuf;
		CHARRANGE CharRange;
		DWORD dwSize;

		if (m_pWinHexData->nSelStart < m_pWinHexData->nSelEnd)
		{
			CharRange.cpMin = m_pWinHexData->nSelStart;
			CharRange.cpMax = m_pWinHexData->nSelEnd;
		}
		else
		{
			CharRange.cpMin = m_pWinHexData->nSelEnd;
			CharRange.cpMax = m_pWinHexData->nSelStart;
		}

		if (CharRange.cpMin != CharRange.cpMax)
		{
			dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
			pLockBuf = (PBYTE)LocalLock(m_pWinHexData->hBuffer);
			if (pLockBuf)
			{
				RtlMoveMemory(pLockBuf + CharRange.cpMin, pLockBuf + CharRange.cpMax, dwSize - CharRange.cpMax);
				LocalUnlock(m_pWinHexData->hBuffer);
			}

			SetMaxBufferSize(dwSize - (CharRange.cpMax - CharRange.cpMin));
			m_pWinHexData->bInMid = FALSE;
			m_pWinHexData->uIndex = m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = CharRange.cpMin;
			m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
			m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
			
			MoveCaret(TRUE);
			Invalidate();
		}
	}

	void CWinHexUI::SelectAll()
	{
		INT nBufSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
		m_pWinHexData->uIndex = m_pWinHexData->nSelStart = 0;
		m_pWinHexData->nSelEnd = nBufSize;

		Invalidate();
		MoveCaret(TRUE);
	}

	void CWinHexUI::MoveCaret(BOOL bScroll)
	{
		int nPos = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
			nPos = m_pVerticalScrollBar->GetScrollPos();

		int nSpliteWidth = m_pWinHexData->nSplitSpacing * m_pWinHexData->uCharWidth;
		int nAddressLeft = m_rcItem.left + m_pWinHexData->nLeftMargin;
		int nAddressRight = nAddressLeft + (10 + m_pWinHexData->nAddressSpaceing) * m_pWinHexData->uCharWidth;

		m_ptCaret.x = nAddressRight + nSpliteWidth +(3 * m_pWinHexData->nCaretCol + m_pWinHexData->bInMid) * m_pWinHexData->uCharWidth;
		m_ptCaret.y = (m_pWinHexData->nCaretLine - nPos) * m_pWinHexData->uLineHeight + m_rcItem.top;

		/*if (m_pWinHexData->bEditField)
			SetCaretPos(nAddressRight +((m_pWinHexData->nSplitSpacing + 3 * m_pWinHexData->nCaretCol + m_pWinHexData->bInMid) * m_pWinHexData->uCharWidth) + 1, (m_pWinHexData->nCaretLine - nPos) * m_pWinHexData->uLineHeight + m_rcItem.top);
		else
			SetCaretPos(nAddressRight + ((3 * m_pWinHexData->uColumnsPerLine + m_pWinHexData->nCaretCol) * m_pWinHexData->uCharWidth) - 2, (m_pWinHexData->nCaretLine - nPos) * m_pWinHexData->uLineHeight + m_rcItem.top);*/
	}

	void CWinHexUI::Update()
	{
		BOOL bScrollBar = FALSE;
		UINT uVisibleLines;
		DWORD dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
		m_pWinHexData->uLines = MAX(dwSize / m_pWinHexData->uColumnsPerLine, 1);
		if (dwSize > m_pWinHexData->uColumnsPerLine && (dwSize % m_pWinHexData->uColumnsPerLine) > 0)
			++m_pWinHexData->uLines;

		if (m_pWinHexData->uLineHeight > 0) {
			m_pWinHexData->uVisibleLinesComplete = uVisibleLines = m_rcItem.bottom / m_pWinHexData->uLineHeight;
			m_pWinHexData->uVisibleLines = m_pWinHexData->uVisibleLinesComplete;
			if (m_rcItem.bottom % m_pWinHexData->uLineHeight)
				++m_pWinHexData->uVisibleLines;
		} else {
			m_pWinHexData->uVisibleLines = uVisibleLines = 0;
		}

		bScrollBar = dwSize > 0 && uVisibleLines < m_pWinHexData->uLines;
		if (m_pVerticalScrollBar)
		{
			m_pVerticalScrollBar->SetVisible(bScrollBar != FALSE);
		}
		Invalidate();
	}

	DWORD CWinHexUI::OnHitRegionTest(POINT pt)
	{
		if (pt.x <= m_pWinHexData->nLeftMargin)
			return HEHT_LEFTMARGIN;

		pt.x -= m_pWinHexData->nLeftMargin;

		int d = (10 * m_pWinHexData->uCharWidth);
		if (pt.x <= d)
			return HEHT_ADDRESS;

		pt.x -= d;
		d = (m_pWinHexData->nAddressSpaceing + m_pWinHexData->nSplitSpacing ) * m_pWinHexData->uCharWidth;
		if (pt.x <= d)
			return HEHT_ADDRESSSPACING;

		pt.x -= d;
		d = (3 * m_pWinHexData->uColumnsPerLine * m_pWinHexData->uCharWidth);
		if (pt.x <= d)
			return HEHT_HEXDUMP;
	
		pt.x -= d;
		d = (m_pWinHexData->nSplitSpacing * m_pWinHexData->uCharWidth);
		if (pt.x <= d)
			return HEHT_HEXDUMPSPACING;

		pt.x -= d;
		d = m_pWinHexData->uColumnsPerLine * m_pWinHexData->uCharWidth;
		if (pt.x <= d)
			return HEHT_ASCIIDUMP;

		return HEHT_RIGHTMARGIN;
	}

	DWORD CWinHexUI::GetIndexFromPoint(POINT pt, DWORD Hit, POINT* pEditPos, BOOL *bEditField)
	{
		int nPos = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
			nPos = m_pVerticalScrollBar->GetScrollPos();

		RECT rc = m_rcItem;
		rc.left -= m_rcPadding.left + m_rcInset.left;
		rc.top -= m_rcPadding.top + m_rcInset.top;
		rc.right -= m_rcPadding.right + m_rcInset.right;
		rc.bottom -= m_rcPadding.bottom + m_rcInset.bottom;

		pEditPos->x = 0;
		if (m_pWinHexData->uLineHeight > 0)
			pEditPos->y = MIN(nPos + ((pt.y - rc.top) / m_pWinHexData->uLineHeight), m_pWinHexData->uLines - 1);
		else
			pEditPos->y = nPos;

		int nSpliteWidth = m_pWinHexData->nSplitSpacing * m_pWinHexData->uCharWidth;
		int nAddressLeft = rc.left + m_pWinHexData->nLeftMargin;
		int nAddressRight = nAddressLeft + (10 + m_pWinHexData->nAddressSpaceing) * m_pWinHexData->uCharWidth;

		switch(Hit)
		{
			case HEHT_LEFTMARGIN:
			case HEHT_ADDRESS:
			case HEHT_ADDRESSSPACING:
			case HEHT_HEXDUMP:
			case HEHT_HEXDUMPSPACING:
					pt.x -= nAddressRight + nSpliteWidth ;
					*bEditField = TRUE;
				break;
			default:
					pt.x -= nAddressRight + nSpliteWidth * 2 + 3* m_pWinHexData->uColumnsPerLine * m_pWinHexData->uCharWidth;
					*bEditField = FALSE;
				break;	
		}

		if (pt.x > 0)
		{
			// 由上述switch语句判断当前处于16进制编辑模式，还是文本模式
			UINT nBlockWidth = (*bEditField ? m_pWinHexData->uCharWidth * 3 : m_pWinHexData->uCharWidth);
			// 在当前行内选择一个字节
			pEditPos->x = MIN(m_pWinHexData->uColumnsPerLine - 1, pt.x / nBlockWidth);
			
			// 判断光标在当前字节高位，还是低位
			if (Hit == HEHT_HEXDUMP)
				// 需要在下面修正（当鼠标点击在编辑器范围内，但是不再文本中的情况）
				m_pWinHexData->bInMid = pt.x % nBlockWidth > m_pWinHexData->uCharWidth;		
			else if (Hit = HEHT_HEXDUMP)
				m_pWinHexData->bInMid = TRUE;
			else if (Hit < HEHT_HEXDUMP)
				m_pWinHexData->bInMid = FALSE;
		}

		DWORD dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
		DWORD dwIndex = (pEditPos->y * m_pWinHexData->uColumnsPerLine) + pEditPos->x;

		// 避免出现选择在字符以外的文本
		if (dwIndex >= dwSize)
		{
			DWORD dwTmp = dwSize % m_pWinHexData->uColumnsPerLine;
			dwIndex = dwSize - 1;
			pEditPos->x = (dwTmp == 0 ? m_pWinHexData->uColumnsPerLine : dwTmp - 1);

			// 由于鼠标点击在16进制文本外面，故光标需要定位在低位上
			m_pWinHexData->bInMid = TRUE;
		}

		return dwIndex;
	}

	void CWinHexUI::SetMaxBufferSize(DWORD dwMaxSize)
	{
		if (dwMaxSize == 0)
			return;

		m_pWinHexData->dwMaxSize = dwMaxSize;
		if (m_pWinHexData->hBuffer)
			m_pWinHexData->hBuffer = LocalReAlloc(m_pWinHexData->hBuffer, dwMaxSize, LMEM_MOVEABLE);
		else
			m_pWinHexData->hBuffer = LocalAlloc(LMEM_MOVEABLE, dwMaxSize);

		Update();
	}

	LRESULT CWinHexUI::LoadBuffer(PVOID pBuffer, DWORD dwSize)
	{
		if (pBuffer != NULL && dwSize > 0)
		{
			LPVOID pLockBuf;
			if (m_pWinHexData->dwMaxSize > 0 && dwSize > m_pWinHexData->dwMaxSize)
				dwSize = m_pWinHexData->dwMaxSize;

			if (m_pWinHexData->hBuffer) 
			{
				if (dwSize > 0)
				{
					if (LocalSize(m_pWinHexData->hBuffer) != dwSize)
						m_pWinHexData->hBuffer = LocalReAlloc(m_pWinHexData->hBuffer, dwSize, LMEM_MOVEABLE | LMEM_ZEROINIT);
				} 
				else
				{
					m_pWinHexData->hBuffer = LocalFree(m_pWinHexData->hBuffer);
					m_pWinHexData->hBuffer = NULL;
					m_pWinHexData->uIndex = 0;

					Update();
					return 0;
				}
			} else if (dwSize > 0)
			{
				m_pWinHexData->hBuffer = LocalAlloc(LHND, dwSize);
			}

			if (dwSize > 0)
			{
				pLockBuf = LocalLock(m_pWinHexData->hBuffer);
				if (pLockBuf)
					RtlCopyMemory(pLockBuf, pBuffer, dwSize);
				else
					dwSize = 0;

				LocalUnlock(m_pWinHexData->hBuffer);
			}

			m_pWinHexData->uIndex = 0;
			Update();
			return dwSize;
		}
		else if (m_pWinHexData->hBuffer)
		{
			m_pWinHexData->uIndex = 0;
			m_pWinHexData->hBuffer = LocalFree(m_pWinHexData->hBuffer);
			Update();
		}

		return 0;
	}

	LRESULT CWinHexUI::CopyBuffer(PVOID pBuffer, DWORD dwSize)
	{
		size_t nCpy;

		if (m_pWinHexData->hBuffer == NULL)
			return 0;
		if (pBuffer != NULL && dwSize > 0)
		{
			nCpy = MIN(dwSize, LocalSize(m_pWinHexData->hBuffer));
			if (nCpy > 0)
			{
				PVOID pLockBuf = LocalLock(m_pWinHexData->hBuffer);
				if (pLockBuf)
					RtlCopyMemory(pBuffer, pLockBuf, nCpy);
				else
					nCpy = 0;
				LocalUnlock(m_pWinHexData->hBuffer);
			}

			return nCpy;
		}

		return (LRESULT)LocalSize(m_pWinHexData->hBuffer);
	}

	CHARRANGE CWinHexUI::GetSelection() const
	{
		CHARRANGE CharRange;
		if (m_pWinHexData->nSelStart < m_pWinHexData->nSelEnd) {
			CharRange.cpMin = m_pWinHexData->nSelStart;
			CharRange.cpMax = m_pWinHexData->nSelEnd;
		} else {
			CharRange.cpMin = m_pWinHexData->nSelEnd;
			CharRange.cpMax = m_pWinHexData->nSelStart;
		}

		return CharRange;
	}

	void CWinHexUI::PaintText(HDC hDC)
	{
		DWORD dwLines;
		RECT rc = m_rcItem;
		int nPos = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
			nPos = m_pVerticalScrollBar->GetScrollPos();

		DWORD dwHeight = (rc.bottom - rc.top);
		dwLines = dwHeight / m_pWinHexData->uLineHeight;
		if ((dwHeight % m_pWinHexData->uLineHeight) > 0)
			++dwLines;
		if (dwLines > m_pWinHexData->uLines - nPos)
			dwLines = m_pWinHexData->uLines - nPos;

		INT nFirst = nPos / m_pWinHexData->uLineHeight;
		PaintLines(hDC, nPos, nFirst, nFirst + dwLines, rc);
	}

	void CWinHexUI::PaintLines(HDC hDC, DWORD dwScrollPos, DWORD dwFirst, DWORD dwLast, RECT& rc)
	{
		DWORD dwLineTop, dwLineStart;
		INT iPos, iSave;
		PBYTE pLockBuf, pCurrent, pEnd, pLine;
		DWORD dwSize;

		DWORD dwCurrentLine = dwFirst;

		TCHAR szHex[3], szAddrOffset[15];
		CHARRANGE CharRange;
		if (m_pWinHexData->nSelStart < m_pWinHexData->nSelEnd) {
			CharRange.cpMin = m_pWinHexData->nSelStart;
			CharRange.cpMax = m_pWinHexData->nSelEnd;
		} else {
			CharRange.cpMin = m_pWinHexData->nSelEnd;
			CharRange.cpMax = m_pWinHexData->nSelStart;
		}

		if (m_pWinHexData->hBuffer) {
			dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
			pLockBuf = (PBYTE)LocalLock(m_pWinHexData->hBuffer);
		} else {
			pLockBuf = NULL;
			dwSize = 0;

			if (dwScrollPos + dwFirst == 0) {
				int nLeft = rc.left + m_pWinHexData->nLeftMargin;
				int nAddressRight = nLeft + (10 + m_pWinHexData->nAddressSpaceing) * m_pWinHexData->uCharWidth;
				RECT rcAddressText = {nLeft, rc.top, nAddressRight, rc.top + m_pWinHexData->uLineHeight};

				_stprintf_s(szAddrOffset, _countof(szAddrOffset), _T("%08lX"), dwCurrentLine);
				CRenderEngine::DrawText(hDC, m_pManager, rcAddressText, szAddrOffset, m_dwTextColor, m_iFont, DT_SINGLELINE | DT_RIGHT);	
			}
		}

		if (pLockBuf) {
			pEnd = pLockBuf + dwSize;
			dwLineTop = rc.top + dwCurrentLine * m_pWinHexData->uLineHeight;
			dwLineStart = (dwScrollPos + dwCurrentLine) * m_pWinHexData->uColumnsPerLine;
			iPos = dwLineStart;
			// 指向当前显示的字符
			pCurrent = pLockBuf + dwLineStart;
			dwLast = MIN(m_pWinHexData->uLines - dwScrollPos, dwLast);

			// 计算分割线宽度
			int nSplitWidth = m_pWinHexData->nSplitSpacing * m_pWinHexData->uCharWidth;

			while (dwCurrentLine <= dwLast && pCurrent < pEnd)
			{
				int nAddressLeft = rc.left + m_pWinHexData->nLeftMargin;
				int nAddressRight = nAddressLeft + (10 + m_pWinHexData->nAddressSpaceing) * m_pWinHexData->uCharWidth;
				RECT rcAddressText = {nAddressLeft, dwLineTop, nAddressRight, dwLineTop + m_pWinHexData->uLineHeight};

				_stprintf_s(szAddrOffset, _countof(szAddrOffset), _T("%08lX"), dwCurrentLine);
				CRenderEngine::DrawText(hDC, m_pManager, rcAddressText, szAddrOffset, m_dwTextColor, m_iFont, DT_SINGLELINE | DT_RIGHT);	

				// 计算16进制显示区域
				int nHexWidth = 3 * m_pWinHexData->uCharWidth;
				pLine = pCurrent;
				iSave = iPos;

				// 显示16进制区域
				int nHexLeft = nAddressRight + nSplitWidth;
				RECT rcHexText = {nHexLeft, dwLineTop, nHexLeft + nHexWidth, dwLineTop + m_pWinHexData->uLineHeight};
				
				// 绘制选中16进制文本背景
				if (m_pWinHexData->nSelEnd != m_pWinHexData->nSelStart)
				{
					CHARRANGE CharRange = GetSelection();
					DWORD dwLineHead = dwCurrentLine * m_pWinHexData->uColumnsPerLine;
					DWORD dwLineEnd = dwLineHead + m_pWinHexData->uColumnsPerLine - 1;

					if (CharRange.cpMin / m_pWinHexData->uColumnsPerLine <= dwCurrentLine &&
						CharRange.cpMax / m_pWinHexData->uColumnsPerLine >= dwCurrentLine)
					{
						RECT rcHexBack = rcHexText;
						// 选择符号在同一行
						if (CharRange.cpMin - dwLineHead < m_pWinHexData->uColumnsPerLine)
							rcHexBack.left += (CharRange.cpMin - dwLineHead ) * 3 * m_pWinHexData->uCharWidth;
						if (dwLineEnd > (ULONG)CharRange.cpMax)
							rcHexBack.right += (CharRange.cpMax % m_pWinHexData->uColumnsPerLine) * 3 * m_pWinHexData->uCharWidth - m_pWinHexData->uCharWidth;
						else if (CharRange.cpMax / m_pWinHexData->uColumnsPerLine >= dwCurrentLine)
							rcHexBack.right = rcHexText.left + m_pWinHexData->uColumnsPerLine * 3 * m_pWinHexData->uCharWidth - m_pWinHexData->uCharWidth;

						CRenderEngine::DrawColor(hDC, rcHexBack, m_dwBackColor);
					}
				}
				// 绘制16进制文本内容
				for (UINT m = 0; m<m_pWinHexData->uColumnsPerLine && pCurrent < pEnd; ++m) 
				{
					_stprintf_s(szHex, _countof(szHex), _T("%02X"), *(pCurrent++));
					
					CRenderEngine::DrawText(hDC, m_pManager, rcHexText, szHex, m_dwTextColor, m_iFont, DT_SINGLELINE);
					
					rcHexText.left += nHexWidth;
					rcHexText.right += nHexWidth;
					++iPos;
				}

				pCurrent = pLine;
				iPos = iSave;

				// 计算ASCII 码显示区域
				int nAsciiWidth = m_pWinHexData->uCharWidth;
				int nAsciiLeft = nAddressRight + (m_pWinHexData->nSplitSpacing + m_pWinHexData->uColumnsPerLine * 3) * m_pWinHexData->uCharWidth + nSplitWidth;
		
				RECT rcAsciiText = {nAsciiLeft, dwLineTop, nAsciiLeft + nAsciiWidth, dwLineTop + m_pWinHexData->uLineHeight};

				// 绘制ASCII 选中背景
				if (m_pWinHexData->nSelEnd != m_pWinHexData->nSelStart)
				{
					CHARRANGE CharRange = GetSelection();
					DWORD dwLineHead = dwCurrentLine * m_pWinHexData->uColumnsPerLine;
					DWORD dwLineEnd = dwLineHead + m_pWinHexData->uColumnsPerLine - 1;

					if (CharRange.cpMin / m_pWinHexData->uColumnsPerLine <= dwCurrentLine &&
						CharRange.cpMax / m_pWinHexData->uColumnsPerLine >= dwCurrentLine)
					{
						RECT rcAsciiBack = rcAsciiText;
						// 选择符号在同一行
						if (CharRange.cpMin - dwLineHead < m_pWinHexData->uColumnsPerLine)
							rcAsciiBack.left += (CharRange.cpMin - dwLineHead ) * m_pWinHexData->uCharWidth;
						if (dwLineEnd > (ULONG)CharRange.cpMax)
							rcAsciiBack.right += (CharRange.cpMax % m_pWinHexData->uColumnsPerLine) * m_pWinHexData->uCharWidth;
						else if (CharRange.cpMax / m_pWinHexData->uColumnsPerLine >= dwCurrentLine)
							rcAsciiBack.right = rcAsciiText.left + m_pWinHexData->uColumnsPerLine * m_pWinHexData->uCharWidth;

						CRenderEngine::DrawColor(hDC, rcAsciiBack, m_dwBackColor);
					}
				} 
				if (m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine == dwCurrentLine) {
					RECT rcAsciiBack = rcAsciiText;
					rcAsciiBack.left += (m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine) * m_pWinHexData->uCharWidth;
					rcAsciiBack.right = rcAsciiBack.left + m_pWinHexData->uCharWidth;
					CRenderEngine::DrawColor(hDC, rcAsciiBack, m_dwBackColor);
				}

				for (UINT m = 0; m < m_pWinHexData->uColumnsPerLine && pCurrent < pEnd; ++m)
				{
					_stprintf_s(szHex, _T("%C"), *(pCurrent++));
#ifdef _UNICODE
					szHex[0] = ((szHex[0] & _T('\x007f')) >= _T(' ') ? szHex[0] : _T('.'));
#else
					szHex[0] = ((szHex[0] & _T('7f')) >= _T(' ') ? szHex[0] : _T('.'));
#endif
					CRenderEngine::DrawText(hDC, m_pManager, rcAsciiText, szHex, m_dwTextColor, m_iFont, DT_SINGLELINE);

					rcAsciiText.left += nAsciiWidth;
					rcAsciiText.right += nAsciiWidth;
					++iPos;
				}

				dwLineTop += m_pWinHexData->uLineHeight;
				dwLineStart += m_pWinHexData->uColumnsPerLine;
				++dwCurrentLine;
			}
		} 

		LocalUnlock(m_pWinHexData->hBuffer);
	}

	void CWinHexUI::PaintAddress(HDC hDC, RECT& rcAddress, INT nLine)
	{

	}

	void CWinHexUI::PaintHexText(HDC hDC)
	{
		
	}

	void CWinHexUI::PaintBorder(HDC hDC)
	{
		CVerticalLayoutUI::PaintBorder(hDC);
		// 绘制光标

		if (m_bCaretOn)
		{
			RECT rcCaret = {m_ptCaret.x, m_ptCaret.y, m_ptCaret.x + m_pWinHexData->uCharWidth, m_ptCaret.y + m_pWinHexData->uLineHeight};
			CRenderEngine::DrawColor(hDC, rcCaret, m_dwCaretColor);

			// 绘制光标高亮区域
			PBYTE pLockBuf = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
			if (pLockBuf)
			{
				TCHAR szCaretHex[3];
				BYTE bHex = m_pWinHexData->bInMid ? *(pLockBuf + m_pWinHexData->uIndex) & 0x0F : *(pLockBuf + m_pWinHexData->uIndex) >> 4;
				_stprintf_s(szCaretHex, _countof(szCaretHex), _T("%X"), bHex);
				CRenderEngine::DrawText(hDC, m_pManager, rcCaret, szCaretHex, 0xFFFFFFFF, m_iFont, DT_SINGLELINE);
				LocalUnlock(m_pWinHexData->hBuffer);
			}
		}
	}

	void CWinHexUI::PaintStatusImage(HDC hDC)
	{

	}

	UINT CWinHexUI::GetFirstVisibleLine()
	{
		return 1;
	}

	void CWinHexUI::OnEventKeyDown(TEventUI& event)
	{
		LPBYTE pLock = NULL;
		CHARRANGE CharRange;
		DWORD dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);

		if (m_pWinHexData->nSelStart < m_pWinHexData->nSelEnd) {
			CharRange.cpMin = m_pWinHexData->nSelStart;
			CharRange.cpMax = m_pWinHexData->nSelEnd;
		} else {
			CharRange.cpMin = m_pWinHexData->nSelEnd;
			CharRange.cpMax = m_pWinHexData->nSelStart;
		}

		switch(event.chKey)
		{
			case 'X':
					if (event.wKeyState & MK_CONTROL && CharRange.cpMin != CharRange.cpMax)
						Cut();
				break;
			case 'C':
					if (event.wKeyState & MK_CONTROL && CharRange.cpMin != CharRange.cpMax)
						Copy();
				break;
			case 'V':
					if (event.wKeyState & MK_CONTROL)
						Paste();
				break;
			case 'A':
				if (event.wKeyState & MK_CONTROL)
					SelectAll();
				break;
			case VK_INSERT:
					if (CharRange.cpMin != CharRange.cpMax)
					{
						if (event.wKeyState & MK_CONTROL)
							Copy();
						else if (event.wKeyState & MK_SHIFT)
							Paste();
					}
				break;
			case VK_ESCAPE:
					if (CharRange.cpMax != CharRange.cpMin)
					{
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
						MoveCaret(TRUE);
						Invalidate();
					}
				break;
			case VK_HOME:
					m_pWinHexData->uIndex -= m_pWinHexData->nCaretCol;
					m_pWinHexData->nCaretCol = 0;
					if (m_pWinHexData->nSelEnd != m_pWinHexData->nSelStart || event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
					MoveCaret(TRUE);
					Invalidate();
				break;
			case VK_END:
					m_pWinHexData->uIndex += m_pWinHexData->uColumnsPerLine - m_pWinHexData->nCaretCol - 1;
					m_pWinHexData->nCaretCol = m_pWinHexData->uColumnsPerLine - 1;
					if (m_pWinHexData->nSelEnd != m_pWinHexData->nSelStart || event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
					MoveCaret(TRUE);
					Invalidate();
				break;
			/*case VK_DELETE:
					if (event.wKeyState & MK_CONTROL && CharRange.cpMin != CharRange.cpMax)
						Copy();
					if (CharRange.cpMax != CharRange.cpMin) {
						pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
						if (pLock)
						{
							RtlMoveMemory(pLock + CharRange.cpMin, pLock + CharRange.cpMax, dwSize - CharRange.cpMax);
							LocalUnlock(m_pWinHexData->hBuffer);
						}
						SetMaxBufferSize(dwSize - (CharRange.cpMax - CharRange.cpMin));
						m_pWinHexData->bInMid = FALSE;
						m_pWinHexData->uIndex = m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = CharRange.cpMin;
						m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
						m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
					} else {
						if (m_pWinHexData->bInMid && m_pWinHexData->bEditField) {
							pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
							if (pLock)
							{
								RtlMoveMemory(pLock + m_pWinHexData->uIndex, 
															pLock + m_pWinHexData->uIndex + 1,
															dwSize - m_pWinHexData->uIndex - 1);
								LocalUnlock(m_pWinHexData->hBuffer);
							}

							SetMaxBufferSize(dwSize - 1);
							m_pWinHexData->bInMid = FALSE;
						} else if (m_pWinHexData->uIndex < dwSize) {
							pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
							if (pLock)
							{
								RtlMoveMemory(pLock + m_pWinHexData->uIndex,
															pLock + m_pWinHexData->uIndex + 1,
															dwSize - m_pWinHexData->uIndex - 1);
								LocalUnlock(m_pWinHexData->hBuffer);
							}
							SetMaxBufferSize(dwSize - 1);
						}
						MoveCaret(TRUE);
						Invalidate();
					}
				break;*/
			/*case VK_BACK:
				if (CharRange.cpMin != CharRange.cpMax) {
					pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
					if (pLock)
					{
						RtlMoveMemory(pLock + CharRange.cpMin, pLock + CharRange.cpMax, dwSize - CharRange.cpMax);
						LocalUnlock(m_pWinHexData->hBuffer);
					}
					SetMaxBufferSize(dwSize - (CharRange.cpMax - CharRange.cpMin));
					m_pWinHexData->bInMid = FALSE;
					m_pWinHexData->uIndex = m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = CharRange.cpMin;
					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
				} else {
					if (m_pWinHexData->bInMid && m_pWinHexData->bEditField) {
						pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
						if (pLock)
						{
							RtlMoveMemory(pLock + m_pWinHexData->uIndex,
														pLock + m_pWinHexData->uIndex + 1,
														dwSize - m_pWinHexData->uIndex - 1);
							LocalUnlock(m_pWinHexData->hBuffer);
						} 
					} else if(m_pWinHexData->uIndex > 0) {
						pLock = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
						if (pLock)
						{
							RtlMoveMemory(pLock + m_pWinHexData->uIndex - 1,
														pLock + m_pWinHexData->uIndex,
														dwSize - m_pWinHexData->uIndex);
							LocalUnlock(m_pWinHexData->hBuffer);
						}
						--m_pWinHexData->uIndex;
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;
						m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
						m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
					} else
						return;
					SetMaxBufferSize(dwSize - 1);
					MoveCaret(TRUE);
					Invalidate();
				}
				break;*/
			case VK_LEFT:
				if (m_pWinHexData->uIndex > 0 ||
					(m_pWinHexData->uIndex == 0 && m_pWinHexData->bInMid != FALSE)) {
					if (m_pWinHexData->bInMid == FALSE)
						-- m_pWinHexData->uIndex;
					m_pWinHexData->bInMid = !m_pWinHexData->bInMid;

					if (event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;

					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;

					MoveCaret(TRUE);
					Invalidate();
				}
				break;
			case VK_RIGHT:
				if (m_pWinHexData->uIndex < dwSize ||
					(m_pWinHexData->uIndex == dwSize && m_pWinHexData->bInMid == FALSE)) {
					if (m_pWinHexData->bInMid != FALSE)
						++m_pWinHexData->uIndex;
					m_pWinHexData->bInMid = !m_pWinHexData->bInMid;

					if (event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;

					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
					
					MoveCaret(TRUE);
					Invalidate();
				}
				break;
			case VK_UP:
				if (m_pWinHexData->uIndex >= m_pWinHexData->uColumnsPerLine) {
					m_pWinHexData->uIndex -= m_pWinHexData->uColumnsPerLine;
					if (event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelEnd = m_pWinHexData->nSelStart = m_pWinHexData->uIndex;

					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;

					MoveCaret(TRUE);
					Invalidate();
				}
				break;
			case VK_DOWN:
					if (m_pWinHexData->uIndex + m_pWinHexData->uColumnsPerLine <= dwSize)
						m_pWinHexData->uIndex += m_pWinHexData->uColumnsPerLine;
					else
						return;

					if (event.wKeyState & MK_SHIFT)
						m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
					else
						m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = m_pWinHexData->uIndex;
			
					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
					MoveCaret(TRUE);
					Invalidate();
				break;
			default:
				break;
		}
	}

	void CWinHexUI::OnEventChar(TEventUI& event)
	{
		CHAR ch = (CHAR)event.chKey;

		if (m_pWinHexData->bHexField)
			if (('0' > ch || ch > '9') && ('a' > ch || ch > 'f') && ('A' > ch || ch > 'F'))
				return;

		DWORD dwSize;
		PBYTE pLockBuff;
		CHARRANGE CharRange;

		dwSize = m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0;
		if (m_pWinHexData->nSelStart < m_pWinHexData->nSelEnd) {
			CharRange.cpMin = m_pWinHexData->nSelStart;
			CharRange.cpMax = m_pWinHexData->nSelEnd;
		} else {
			CharRange.cpMin = m_pWinHexData->nSelEnd;
			CharRange.cpMax = m_pWinHexData->nSelStart;
		}

		if (m_pWinHexData->bHexField == FALSE) {
			if (0x20 <= ch && ch <= 0xFF) {
				if (m_pWinHexData->nSelStart != m_pWinHexData->nSelEnd) {
					pLockBuff = (PBYTE)LocalLock(m_pWinHexData->hBuffer);
					if (pLockBuff) {
						RtlMoveMemory(pLockBuff + CharRange.cpMin, pLockBuff + CharRange.cpMax, dwSize);
						LocalUnlock(m_pWinHexData->hBuffer);
					}
					//SetMaxBufferSize(dwSize);
					m_pWinHexData->bInMid = FALSE;
					dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0);
					m_pWinHexData->uIndex = m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = CharRange.cpMin;
				}

				//SetMaxBufferSize(dwSize + 1);
				pLockBuff = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
				if (pLockBuff) {
					//RtlMoveMemory(pLockBuff + m_pWinHexData->uIndex + 1, pLockBuff + m_pWinHexData->uIndex, dwSize - m_pWinHexData->uIndex);
					pLockBuff[m_pWinHexData->uIndex] = ch;
					LocalUnlock(m_pWinHexData->hBuffer);
				}

				// 已经到文本末尾，不能通过编辑继续添加
				if (m_pWinHexData->uIndex != dwSize - 1)
				{
					++m_pWinHexData->uIndex;
					m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
					m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
				}

				MoveCaret(TRUE);
				Invalidate();
			}
		} else {
			if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F') || ('a' <= ch && ch <= 'f')) {
				if (m_pWinHexData->nSelStart != m_pWinHexData->nSelEnd) {
					pLockBuff = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
					if (pLockBuff) {
						RtlMoveMemory(pLockBuff + CharRange.cpMin, pLockBuff + CharRange.cpMax, dwSize - CharRange.cpMin);
						LocalUnlock(m_pWinHexData->hBuffer);
					}

					//SetMaxBufferSize(dwSize - (CharRange.cpMax - CharRange.cpMin));
					m_pWinHexData->bInMid = !m_pWinHexData->bInMid;
					dwSize = (m_pWinHexData->hBuffer ? LocalSize(m_pWinHexData->hBuffer) : 0 );
					m_pWinHexData->uIndex = m_pWinHexData->nSelStart = m_pWinHexData->nSelEnd = CharRange.cpMin;
				}
				if (m_pWinHexData->bInMid) {
					pLockBuff = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
					if (pLockBuff) {
						pLockBuff[m_pWinHexData->uIndex] &= 0xF0;
						if ('0' <= ch && ch <= '9')
							pLockBuff[m_pWinHexData->uIndex] |= ch - '0';
						else if ('A' <= ch && ch <= 'F')
							pLockBuff[m_pWinHexData->uIndex] |= ch + 10 - 'A';
						else if ('a' <= ch && ch <= 'f')
							pLockBuff[m_pWinHexData->uIndex] |= ch + 10 - 'a';
						LocalUnlock(m_pWinHexData->hBuffer);
					}
					// 已经到文本末尾，不能通过编辑继续添加
					if (m_pWinHexData->uIndex != dwSize - 1)
					{
						m_pWinHexData->bInMid = !m_pWinHexData->bInMid;
						++m_pWinHexData->uIndex;
					}					
				} else {
					//SetMaxBufferSize(dwSize + 1);
					pLockBuff = (PBYTE) LocalLock(m_pWinHexData->hBuffer);
					if (pLockBuff) {
						/*RtlMoveMemory(pLockBuff + m_pWinHexData->uIndex + 1, 
													pLockBuff + m_pWinHexData->uIndex,
													dwSize - m_pWinHexData->uIndex);*/
						pLockBuff[m_pWinHexData->uIndex] &= 0x0F;
						if ('0' <= ch && ch <= '9')
							pLockBuff[m_pWinHexData->uIndex] |= (ch - '0') << 4;
						else if ('A' <= ch && ch <= 'F')
							pLockBuff[m_pWinHexData->uIndex] |= (ch + 10 - 'A') << 4;
						else if ('a' <= ch && ch <= 'f')
							pLockBuff[m_pWinHexData->uIndex] |= (ch + 10 - 'a') << 4;
						LocalUnlock(m_pWinHexData->hBuffer);
					}
					m_pWinHexData->bInMid = !m_pWinHexData->bInMid;
				}

				m_pWinHexData->nCaretCol = m_pWinHexData->uIndex % m_pWinHexData->uColumnsPerLine;
				m_pWinHexData->nCaretLine = m_pWinHexData->uIndex / m_pWinHexData->uColumnsPerLine;
				MoveCaret(TRUE);
				Invalidate();
			}
		}
	}
}