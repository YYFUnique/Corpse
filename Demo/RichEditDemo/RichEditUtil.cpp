#include "stdafx.h"
#include "RichEditUtil.h"
#include "DuiLib/Utils/DuiFunc.h"
#include "ImageOleCtrl/ImageOle.h"

DWORD RichEdit_GetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	HRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
	return (DWORD)lRes;
}

BOOL RichEdit_SetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	HRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
	return (BOOL)lRes;
}

// 设置默认字体
void RichEdit_SetDefFont(ITextServices * pTextServices, LPCTSTR lpFontName,	
						 int nFontSize,	COLORREF clrText, BOOL bBold, 
						 BOOL bItalic, BOOL bUnderLine, BOOL bIsLink)
{
	CHARFORMAT cf = {0};
	RichEdit_GetDefaultCharFormat(pTextServices, cf);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask |= CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask |= CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_LINK;

	cf.crTextColor = clrText;				// 设置字体颜色
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	if (bBold)								// 设置粗体
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	if (bItalic)							// 设置倾斜
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	if (bUnderLine)							// 设置下划线
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	if (bIsLink)							// 设置超链接
		cf.dwEffects |= CFE_LINK;
	else
		cf.dwEffects &= ~CFE_LINK;

	RichEdit_SetDefaultCharFormat(pTextServices, cf);
}

IRichEditOle* RichEdit_GetOleInterface(ITextServices* pTextServices)
{
	IRichEditOle *pRichEditOle = NULL;
	pTextServices->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, NULL);
	return pRichEditOle;
}

int RichEdit_GetWindowTextLength(ITextServices* pTextServices)
{
	HRESULT lRes = 0;
	pTextServices->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, &lRes);
	return (int)lRes;
}

int RichEdit_GetTextRange(ITextServices * pTextServices, CHARRANGE * lpchrg, CDuiString& strText)
{
	strText.Empty();

	if (NULL == lpchrg || lpchrg->cpMax <= lpchrg->cpMin)
		return 0;

	LONG nLen = lpchrg->cpMax - lpchrg->cpMin;
	TCHAR * pText = new TCHAR[nLen+1];
	if (NULL == pText)
		return 0;

	memset(pText, 0, (nLen+1)*sizeof(TCHAR));

	TEXTRANGE tr = { 0 };
	tr.chrg = *lpchrg;
	tr.lpstrText = pText;

	HRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, &lRes);

	strText = pText;
	delete []pText;

	return (int)lRes;
}

// 获取文本
void RichEdit_GetText(ITextServices* pTextServices, CDuiString& strText)
{
	REOBJECT ReObject;
	LONG nFaceId, nPos = 0;
	CDuiString strOrgText, strTemp;

	IRichEditOle* pRichEditOle = RichEdit_GetOleInterface(pTextServices);
	if (NULL == pRichEditOle)
		return;

	CHARRANGE CharRange = {0, RichEdit_GetWindowTextLength(pTextServices)};
	RichEdit_GetTextRange(pTextServices, &CharRange, strOrgText);

	for (LONG i = 0; i < (int)strOrgText.GetLength(); i++)
	{
		memset(&ReObject, 0, sizeof(REOBJECT));
		ReObject.cbStruct = sizeof(REOBJECT);
		ReObject.cp = i;
		HRESULT hRet = pRichEditOle->GetObject(REO_IOB_USE_CP, &ReObject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hRet))
		{
			if (ReObject.cp > 0 && ReObject.cp > nPos)
			{
				strTemp = strOrgText.Mid(nPos, ReObject.cp-nPos);
				Replace(strTemp, _T("/"), _T("//"));
				strText += strTemp;
			}
			nPos = ReObject.cp + 1;

			if (NULL == ReObject.poleobj)
				continue;

			if (CLSID_ImageOle == ReObject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hRet = ReObject.poleobj->QueryInterface(IID_IImageOle, (void**)&pImageOle);
				if (SUCCEEDED(hRet))
				{
					pImageOle->GetFaceId(&nFaceId);
					if (nFaceId != -1)
					{
						TCHAR cBuf[32] = {0};
						wsprintf(cBuf, _T("/f[\"%03d\"]"), nFaceId);
						strText += cBuf;
					}
					else
					{
						strText += _T("/c[\"");
						BSTR bstrFileName = NULL;
						pImageOle->GetFileName(&bstrFileName);
						strText += bstrFileName;
						::SysFreeString(bstrFileName);
						strText += _T("\"]");
					}
					pImageOle->Release();
				}
			}
			ReObject.poleobj->Release();
		}
	}

	int nLen = strOrgText.GetLength();
	if (nPos < nLen)
	{
		strTemp = strOrgText.Right(nLen-nPos);
		Replace(strTemp, _T("/"), _T("//"));
		strText += strTemp;
	}

	pRichEditOle->Release();
}


void RichEdit_GetSel(ITextServices * pTextServices, LONG& nStartChar, LONG& nEndChar)
{
	CHARRANGE cr = { 0, 0 };
	pTextServices->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
	nStartChar = cr.cpMin;
	nEndChar = cr.cpMax;
}

int RichEdit_SetSel(ITextServices * pTextServices, LONG nStartChar, LONG nEndChar)
{
	CHARRANGE cr = { nStartChar, nEndChar };
	HRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lRes);
	return (int)lRes;
}

// 替换选中文本
void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText,	
						 LPCTSTR lpFontName, int nFontSize,	COLORREF clrText, 
						 BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink, 
						 int nStartIndent, BOOL bCanUndo/* = FALSE*/)
{
	long lStartChar = 0, lEndChar = 0;
	RichEdit_GetSel(pTextServices, lStartChar, lEndChar);
	RichEdit_ReplaceSel(pTextServices, lpszNewText, bCanUndo);
	lEndChar = lStartChar + _tcslen(lpszNewText);
	RichEdit_SetSel(pTextServices, lStartChar, lEndChar);
	RichEdit_SetFont(pTextServices, lpFontName, nFontSize, clrText, bBold, bItalic, bUnderLine, bIsLink);
	RichEdit_SetStartIndent(pTextServices, nStartIndent);
	RichEdit_SetSel(pTextServices, lEndChar, lEndChar);
}

void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText, BOOL bCanUndo/* = FALSE*/)
{
	pTextServices->TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText, NULL);
}

// 设置左缩进(单位:缇)
BOOL RichEdit_SetStartIndent(ITextServices * pTextServices, int nSize)
{
	PARAFORMAT2 pf2;
	memset(&pf2, 0, sizeof(pf2));
	pf2.cbSize = sizeof(PARAFORMAT2);
	if (nSize == -1)
	{
		pf2.dwMask = PFM_ALIGNMENT;
		pf2.wAlignment = PFA_CENTER;
	}else
	{
		pf2.dwMask = PFM_STARTINDENT|PFM_ALIGNMENT;
		pf2.wAlignment = PFA_LEFT;
		pf2.dxStartIndent = nSize;
	}
	
	HRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf2, &lRes);
	return (BOOL)lRes;
}

DWORD RichEdit_GetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
	return (DWORD)lRes;
}

BOOL RichEdit_SetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	pTextServices->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
	return (BOOL)lRes;
}

// 设置字体
void RichEdit_SetFont(ITextServices * pTextServices, LPCTSTR lpFontName, int nFontSize,
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink)
{
	CHARFORMAT cf;
	memset(&cf, 0, sizeof(cf));
	RichEdit_GetSelectionCharFormat(pTextServices, cf);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_LINK;

	cf.crTextColor = clrText;				// 设置字体颜色
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	if (bBold)								// 设置粗体
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	if (bItalic)							// 设置倾斜
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	if (bUnderLine)							// 设置下划线
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	if (bIsLink)							// 设置超链接
		cf.dwEffects |= CFE_LINK;
	else
		cf.dwEffects &= ~CFE_LINK;

	RichEdit_SetSelectionCharFormat(pTextServices, cf);
}


// 插入表情图片
BOOL RichEdit_InsertFace(ITextServices *pTextServices, ITextHost *pTextHost,	
						 LPCTSTR lpszFileName, int nFaceId,	int nFaceIndex, 
						 COLORREF clrBg, BOOL bAutoScale, int nReservedWidth)
{
	BSTR bstrFileName = NULL;
	IRichEditOle * pRichEditOle = NULL;
	IOleClientSite *pOleClientSite = NULL;
	IImageOle* pImageOle = NULL;
	IOleObject *pOleObject = NULL;
	HRESULT hRet = E_FAIL;

	if (NULL == pTextServices || NULL == pTextHost || NULL == lpszFileName || NULL == *lpszFileName)
		return FALSE;

	TCHAR cProtocol[16] = {0};
	_tcsncpy(cProtocol, lpszFileName, 7);
	if ((_tcsicmp(cProtocol, _T("http://")) != 0) && ::GetFileAttributes(lpszFileName) == 0xFFFFFFFF)
		return FALSE;

	bstrFileName = ::SysAllocString(lpszFileName);
	if (NULL == bstrFileName)
		return FALSE;

	do 
	{
		pTextServices->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, NULL);
		if (NULL == pRichEditOle)
			break;

		hRet = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER, IID_IImageOle, (void**)&pImageOle);
		if (FAILED(hRet))
			break;

		hRet = pImageOle->QueryInterface(IID_IOleObject, (void **)&pOleObject);
		if (FAILED(hRet))
			break;

		pRichEditOle->GetClientSite(&pOleClientSite);
		if (NULL == pOleClientSite)
			break;

		pOleObject->SetClientSite(pOleClientSite);
		HWND hNotifyWnd = GetDuiLibMgr().GetMainWndHandle();
		pImageOle->SetNotifyHwnd(hNotifyWnd);
		pImageOle->LoadFileFromPath(bstrFileName);

		REOBJECT ReObject = {0};
		ReObject.cbStruct	  = sizeof(REOBJECT);
		ReObject.clsid			= CLSID_ImageOle;
		ReObject.cp		    = REO_CP_SELECTION;
		ReObject.dvaspect = DVASPECT_CONTENT;
		ReObject.dwFlags  = REO_BELOWBASELINE;
		ReObject.poleobj  = pOleObject;
		ReObject.polesite = pOleClientSite;
		ReObject.dwUser   = 0;

		hRet = pRichEditOle->InsertObject(&ReObject);
		if (FAILED(hRet))
			break;

		hRet = ::OleSetContainedObject(pOleObject, TRUE);
		if (FAILED(hRet))
			break;

	} while (FALSE);

	if (pOleObject != NULL)
		pOleObject->Release();

	if (pImageOle != NULL)
		pImageOle->Release();

	if (pOleClientSite != NULL)
		pOleClientSite->Release();

	if (pRichEditOle != NULL)
		pRichEditOle->Release();

	if (bstrFileName != NULL)
		::SysFreeString(bstrFileName);

	return SUCCEEDED(hRet);
}