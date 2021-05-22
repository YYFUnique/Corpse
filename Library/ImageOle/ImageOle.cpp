#include "stdafx.h"
#include "ImageOle.h"
#include <tom.h>

DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
			   0x00,0xAA,0x00,0x47,0xBE,0x5D);

CImageOle::CImageOle()
:m_pRichEdit(NULL)
,m_ulRef(0)
,m_pOleClientSite(NULL)
,m_pAdvSink(NULL)
,m_NotifyWnd(NULL)
,m_iFrame(0)
,m_nTimePass(0)
,m_pImageEx(NULL)
{
}

CImageOle::~CImageOle(void)
{
	if (m_pImageEx != NULL)
	{
		delete m_pImageEx;
		m_pImageEx = NULL;
	}
}

HRESULT WINAPI CImageOle::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IImageOle)
	{
		*ppvObject = (IImageOle *)this;
		((IImageOle *)(*ppvObject))->AddRef();
		return S_OK;
	}
	if (iid == IID_IUnknown || iid == IID_IOleObject)
	{
		*ppvObject = (IOleObject *)this;
		((IOleObject *)(*ppvObject))->AddRef();
		return S_OK;
	}
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
		((IViewObject2 *)(*ppvObject))->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG WINAPI CImageOle::AddRef(void)
{
	m_ulRef++;
	return m_ulRef;
}

ULONG WINAPI CImageOle::Release(void)
{
	if (--m_ulRef == 0)
	{
		delete this;
		return 0;
	}

	return m_ulRef;
}

HRESULT WINAPI  CImageOle::SetNotifyHwnd(HWND hWnd)
{
	m_NotifyWnd = hWnd;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetRichEdit(CRichEditUI* pRichEdit)
{
	m_pRichEdit = pRichEdit;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetFaceId(LONG nFaceId)
{
	m_nFaceId = nFaceId;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetFaceId(LONG* nFaceId)
{
	*nFaceId = 0xFFFFFFFF;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetFaceIndex(LONG nFaceIndex)
{
	m_nFaceIndex = nFaceIndex;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetFaceIndex(LONG* nFaceIndex)
{
	*nFaceIndex = m_nFaceIndex;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetFileName(BSTR* lpszFileName)
{
	m_strFileName.CopyTo(lpszFileName);
	return S_OK;
}

HRESULT WINAPI CImageOle::LoadFileFromPath(BSTR bstrFileName)
{
	if (m_pImageEx)
		delete m_pImageEx;

	m_strFileName = bstrFileName;

	m_pImageEx = new CImageEx(m_NotifyWnd);
	if (m_pImageEx != NULL)
		return m_pImageEx->LoadFromFile(bstrFileName);
	else
		return E_OUTOFMEMORY;
}

HRESULT WINAPI CImageOle::SetClientSite(IOleClientSite *pClientSite)
{
	m_pOleClientSite = pClientSite;
	if(m_pOleClientSite) m_pOleClientSite->AddRef();
	return S_OK;
}

HRESULT WINAPI CImageOle::GetClientSite(IOleClientSite **ppClientSite)
{
	*ppClientSite = m_pOleClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::Close(DWORD dwSaveOption)
{
	if(m_pOleClientSite)
	{
		m_pOleClientSite->Release();
		m_pOleClientSite = NULL;
	}
	if (m_pAdvSink != NULL)
	{
		m_pAdvSink->Release();
		m_pAdvSink = NULL;
	}
// 	if (m_pImageEx)
// 	{
// 		delete m_pImageEx;
// 		m_pImageEx = NULL;
// 	}
// 	if(m_pSkin)
// 	{
// 		m_pSkin->Release();
// 		m_pSkin=NULL;
// 	}

	//m_pRichedit->GetContainer()->UnregisterTimelineHandler(this);

	return S_OK;
}

HRESULT WINAPI CImageOle::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetUserClassID(CLSID *pClsid)
{
	*pClsid = CLSID_ImageOle;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
	return E_NOTIMPL;
}

// 设置控件可使用的空间（控件在屏幕上使用的区域范围）
HRESULT WINAPI CImageOle::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

// 获取控件可使用的空间
HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
							   DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, 
							   LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
							   BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), 
							   ULONG_PTR dwContinue)
{
	if (lindex != -1)
		return S_FALSE;

	RECT rcItem;
	rcItem.left   = lprcBounds->left;
	rcItem.top    = lprcBounds->top;
	rcItem.right  = lprcBounds->right;
	rcItem.bottom = lprcBounds->bottom;

	if(m_pImageEx != NULL)
		m_pImageEx->ShowImage(hdcDraw,&rcItem);

	return S_OK;
}

HRESULT WINAPI CImageOle::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
	if (pAdvSink != NULL)
	{
		m_pAdvSink = pAdvSink;
		m_pAdvSink->AddRef();
	}

	if (m_pImageEx != NULL/* && m_pSkin->IsClass(CDuiSkinGif::GetClassName())*/)
	{
		m_iFrame=0;
		m_nTimeDelay=100;//m_pImageEx->GetFrameDelay(0);
		m_nTimePass=0;
		//m_pRichedit->GetContainer()->RegisterTimelineHandler(this);
	}

	return S_OK;
}

HRESULT WINAPI CImageOle::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
 	if (m_pImageEx != NULL)
 	{
 		SIZE sz=m_pImageEx->GetImageSize();
 
 		HDC hDC = ::GetDC(NULL);
 		lpsizel->cx = ::MulDiv(sz.cx, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
 		lpsizel->cy = ::MulDiv(sz.cx, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
 		::ReleaseDC(NULL, hDC);
 	}

	return S_OK;
}

//需要用枚举的办法来获得指定OLE的显示位置，不知道有没有其它更好的办法。
BOOL CImageOle::GetOleRect( LPRECT lpRect )
{
	IRichEditOle* pRichEditOle = m_pRichEdit->GetRichEditOle();
	if(!pRichEditOle) return FALSE;

	BOOL bRet=FALSE;

	int nObjCount = pRichEditOle->GetObjectCount();
	for (int n = 0; n < nObjCount; ++n)
	{	
		REOBJECT Reobject;
		ZeroMemory(&Reobject, sizeof(REOBJECT));
		Reobject.cbStruct = sizeof(REOBJECT);

		HRESULT hRet = pRichEditOle->GetObject(n, &Reobject, REO_GETOBJ_POLEOBJ);
		if (hRet != S_OK)
			continue;

		Reobject.poleobj->Release();

		if (Reobject.poleobj == (IOleObject *)this)
		{
			ITextDocument *pTextDocument = NULL;
			ITextRange *pTextRange = NULL;

			pRichEditOle->QueryInterface(IID_ITextDocument, (void **)&pTextDocument);
			if (!pTextDocument)
				break;

			LONG nLeft = 0;
			LONG nBottom = 0;
			pTextDocument->Range(Reobject.cp, Reobject.cp, &pTextRange);
			if (Reobject.dwFlags & REO_BELOWBASELINE)
				hRet = pTextRange->GetPoint(TA_BOTTOM|TA_LEFT, &nLeft, &nBottom);
			else
				hRet = pTextRange->GetPoint(TA_BASELINE|TA_LEFT, &nLeft, &nBottom);

			pTextDocument->Release();
			pTextRange->Release();

			//CDuiRect rcPos;
			//GetWindowRect(m_pRichEdit->GetManager()->GetPaintWindow(),&rcPos);
// 			CSize szOle=m_pSkin->GetSkinSize();
// 
// 			lpRect->left   = nLeft - rcRichedit.left;
// 			lpRect->bottom = nBottom - rcRichedit.top;
// 			lpRect->right  = lpRect->left + szOle.cx ;
// 			lpRect->top    = lpRect->bottom - szOle.cy;

			bRet=TRUE;
			break;
		}
	}

	pRichEditOle->Release();
	return bRet;
}

void CImageOle::OnNextFrame()
{
	m_nTimePass +=10;
	if(m_nTimePass>=m_nTimeDelay)
	{
		CDuiRect rcOle;
		if(GetOleRect(&rcOle))
		{
			//m_pRichEdit->NotifyInvalidateRect(rcOle);
		}

// 		CDuiSkinGif *pSkinGif=static_cast<CDuiSkinGif*>(m_pSkin);
// 		DUIASSERT(pSkinGif);
// 		m_iFrame++;
// 		if(m_iFrame==pSkinGif->GetStates())
// 			m_iFrame=0;
// 
// 		m_nTimeDelay=pSkinGif->GetFrameDelay(m_iFrame);
// 		m_nTimePass=0;
	}
}
