#include "stdafx.h"
#include "RichEditOleCallback.h"

CRichEditOleCallback::CRichEditOleCallback()
{
	m_iStorage = 0;
	HRESULT hRet = ::StgCreateDocfile(NULL,
				STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DELETEONRELEASE,
				0, &m_pStg);
	if (hRet != S_OK || m_pStg == NULL)
	{
	}
	AddRef();
}

CRichEditOleCallback::~CRichEditOleCallback()
{

}

HRESULT CRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	WCHAR szName[MAX_PATH];
	swprintf_s(szName,_countof(szName), L"REStorage_%d", ++m_iStorage);

	if (m_iStorage % 100 == 0)
		m_pStg->Commit(STGC_DEFAULT);

	HRESULT hRet = m_pStg->CreateStorage(szName, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, lplpstg);
	if (FAILED(hRet) && hRet == E_OUTOFMEMORY)
	{
		// 由于存储空间不足，导致失败时，尝试向Storage提交后重试
		m_pStg->Commit(STGC_DEFAULT);
		hRet = m_pStg->CreateStorage(szName, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, lplpstg);
	}

	return hRet;
}

HRESULT CRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{
	HRESULT hRet = S_OK;
	*ppvObject = NULL;

	if (iid == IID_IUnknown || iid == IID_IRichEditOleCallbackEx) {
		*ppvObject = this;
		AddRef();
	} else {
		hRet = E_NOINTERFACE;
	}

	return hRet;
}

ULONG CRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}

ULONG CRichEditOleCallback::Release()
{
	if (--m_dwRef == 0)
		delete this;

	return m_dwRef;
}

HRESULT CRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME *lplpFrame, LPOLEINPLACEUIWINDOW *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::GetClipboardData(CHARRANGE *lpchrg, DWORD reco, LPDATAOBJECT *lplpdataobj)
{
	/*演示自定义剪贴板格式的复制
    if(RECO_COPY == reco || RECO_CUT == reco)
    {
        wchar_t * pBuf = new WCHAR[lpchrg->cpMax - lpchrg->cpMin +1];
        TEXTRANGE txtRng;
        txtRng.chrg = *lpchrg;
        txtRng.lpstrText = pBuf;
        m_pRichedit->SSendMessage(EM_GETTEXTRANGE,0,(LPARAM)&txtRng);
        pBuf[lpchrg->cpMax - lpchrg->cpMin] =0;
        
        int  strBytes=  (lpchrg->cpMax - lpchrg->cpMin +1) * 2;  
        HGLOBAL hG = GlobalAlloc(GMEM_DDESHARE, strBytes);  
        void* pBuffer = GlobalLock(hG);  
        {  
            memcpy(pBuffer, pBuf, strBytes);  
            GlobalUnlock(hG);  
        }  
        delete []txtRng.lpstrText;

        FORMATETC fmt;  
        fmt.cfFormat = KCF_SMILEY;  
        fmt.dwAspect = DVASPECT_CONTENT;  
        fmt.lindex = -1;  
        fmt.ptd = NULL;  
        fmt.tymed = TYMED_HGLOBAL;  

        STGMEDIUM stg;  
        stg.tymed = TYMED_HGLOBAL;  
        stg.hGlobal = hG;  
        stg.pUnkForRelease = NULL;  

        HRESULT hr =CreateDataObject(&fmt,&stg,1,lplpdataobj);


        return hr;
    }
    */
    return E_NOTIMPL;
}

HRESULT CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT *lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (fReally == FALSE)
		return S_OK;

	/*演示自定义剪贴板格式的粘贴
    if(RECO_DROP == reco || RECO_PASTE == reco)
    {
        FORMATETC fmt;  
        fmt.cfFormat = KCF_SMILEY;  
        fmt.dwAspect = DVASPECT_CONTENT;  
        fmt.lindex = -1;  
        fmt.ptd = NULL;  
        fmt.tymed = TYMED_HGLOBAL;  
        //如果KCF_SMILEY 剪贴板格式可用  
        if (SUCCEEDED(lpdataobj->QueryGetData(&fmt)) )
        {  
            STGMEDIUM stg;  
            HRESULT hr = lpdataobj->GetData(&fmt, &stg);  

            int nSize = GlobalSize(stg.hGlobal);  
            void* pBuffer = GlobalLock(stg.hGlobal);  
            {  

                STRACE(L"QueryAcceptData:%s",(LPWSTR)pBuffer);
                GlobalUnlock(stg.hGlobal);  
            }  
            return S_OK;  
        } 
    }
    */
    return E_NOTIMPL;
}

HRESULT CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return S_OK;
}

HRESULT CRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE *lpchrg, HMENU *lphmenu)
{
	return S_OK;
}