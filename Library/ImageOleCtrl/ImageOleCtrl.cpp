#include "stdafx.h"
#include "ImageOleCtrl.h"
#include <tom.h>

#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID CDECL name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
			   0x00,0xAA,0x00,0x47,0xBE,0x5D);

CImageOleCtrl::CImageOleCtrl()
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

CImageOleCtrl::~CImageOleCtrl(void)
{
	if (m_pImageEx != NULL)
	{
		delete m_pImageEx;
		m_pImageEx = NULL;
	}
}

HRESULT WINAPI CImageOleCtrl::QueryInterface(REFIID iid, void ** ppvObject)
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

ULONG WINAPI CImageOleCtrl::AddRef(void)
{
	m_ulRef++;
	return m_ulRef;
}

ULONG WINAPI CImageOleCtrl::Release(void)
{
	if (--m_ulRef == 0)
	{
		delete this;
		return 0;
	}

	return m_ulRef;
}

HRESULT WINAPI  CImageOleCtrl::SetNotifyHwnd(HWND hWnd)
{
	m_NotifyWnd = hWnd;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::SetRichEdit(CRichEditUI* pRichEdit)
{
	m_pRichEdit = pRichEdit;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::SetFaceId(LONG nFaceId)
{
	m_nFaceId = nFaceId;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::GetFaceId(LONG* nFaceId)
{
	*nFaceId = 0xFFFFFFFF;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::SetFaceIndex(LONG nFaceIndex)
{
	m_nFaceIndex = nFaceIndex;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::GetFaceIndex(LONG* nFaceIndex)
{
	*nFaceIndex = m_nFaceIndex;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::GetFileName(BSTR* lpszFileName)
{
	m_strFileName.CopyTo(lpszFileName);
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::LoadFileFromPath(BSTR bstrFileName)
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

HRESULT WINAPI CImageOleCtrl::SetClientSite(IOleClientSite *pClientSite)
{
	m_pOleClientSite = pClientSite;
	if(m_pOleClientSite) m_pOleClientSite->AddRef();
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::GetClientSite(IOleClientSite **ppClientSite)
{
	*ppClientSite = m_pOleClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::Close(DWORD dwSaveOption)
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

HRESULT WINAPI CImageOleCtrl::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::GetUserClassID(CLSID *pClsid)
{
	*pClsid = CLSID_ImageOle;
	return S_OK;
}

HRESULT WINAPI CImageOleCtrl::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
	return E_NOTIMPL;
}

// 设置控件可使用的空间（控件在屏幕上使用的区域范围）
HRESULT WINAPI CImageOleCtrl::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

// 获取控件可使用的空间
HRESULT WINAPI CImageOleCtrl::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
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

HRESULT WINAPI CImageOleCtrl::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
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

HRESULT WINAPI CImageOleCtrl::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOleCtrl::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
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
BOOL CImageOleCtrl::GetOleRect( LPRECT lpRect )
{
	IRichEditOle* pRichEditOle = m_pRichEdit->GetRichEditOle();
	if(!pRichEditOle) return FALSE;

	BOOL bRet=FALSE;

	int nObjCount = pRichEditOle->GetObjectCount();
	int i = 0;
	for (i = 0;i < nObjCount;i++)
	{	
		REOBJECT reo;
		ZeroMemory(&reo, sizeof(REOBJECT));
		reo.cbStruct = sizeof(REOBJECT);

		HRESULT hr = pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ);
		if (hr != S_OK)
			continue;

		reo.poleobj->Release();

		if (reo.poleobj == (IOleObject *)this)
		{
			ITextDocument *pTextDocument = NULL;
			ITextRange *pTextRange = NULL;

			pRichEditOle->QueryInterface(IID_ITextDocument, (void **)&pTextDocument);
			if (!pTextDocument)
				break;

			long nLeft = 0;
			long nBottom = 0;
			pTextDocument->Range(reo.cp, reo.cp, &pTextRange);
			if (reo.dwFlags & REO_BELOWBASELINE)
				hr = pTextRange->GetPoint(TA_BOTTOM|TA_LEFT, &nLeft, &nBottom);
			else
				hr = pTextRange->GetPoint(TA_BASELINE|TA_LEFT, &nLeft, &nBottom);

			pTextDocument->Release();
			pTextRange->Release();

			CDuiRect rcRichedit;
			GetWindowRect(m_pRichEdit->GetManager()->GetPaintWindow(),&rcRichedit);
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

void CImageOleCtrl::OnNextFrame()
{
	m_nTimePass+=10;
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
