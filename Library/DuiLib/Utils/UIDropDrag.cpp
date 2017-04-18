#include "stdafx.h"
#include "UIDropDrag.h"
#include <shlobj.h>
#include <atlbase.h>
namespace DuiLib
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//CUIDropSource
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CUIDropSource::CUIDropSource()
	{
		m_lRefCount = 1;
	}

	CUIDropSource::~CUIDropSource()
	{
		m_lRefCount = 0;
	}

	ULONG CUIDropSource::AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	ULONG CUIDropSource::Release()
	{
		ULONG lRef = InterlockedDecrement(&m_lRefCount);
		if (0 == lRef)
			delete this;

		return m_lRefCount;
	}

	HRESULT CUIDropSource::QueryInterface(REFIID riid, void **ppvObject) 
	{  
		if (riid == IID_IUnknown || riid == IID_IDropSource )
		{
			AddRef();
			*ppvObject = static_cast<CUIDropSource*>(this);
			return S_OK;
		}  
		else  
		{  
			*ppvObject = 0;  
			return E_NOINTERFACE;  
		}  
	}  

	HRESULT CUIDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
	{
		if (fEscapePressed != FALSE)
			return DRAGDROP_S_CANCEL;    

		// if the <LeftMouse> button has been released, then do the drop!  
		if ((grfKeyState & MK_LBUTTON) == 0)  
			return DRAGDROP_S_DROP;  

		// continue with the drag-drop  
		return S_OK;  
	}

	HRESULT CUIDropSource::GiveFeedback(DWORD dwEffect)
	{
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

	HRESULT CUIDropSource::CreateDropSource(IDropSource **ppDropSource)  
	{  
		if (ppDropSource == 0)  
			return E_INVALIDARG;  

		*ppDropSource = new CUIDropSource();  

		return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;  
	}  

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//CUIDropTarget
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CUIDropTarget::CUIDropTarget(void)
	{
		m_pDropHelper = NULL;
		m_lRefCount = 1;
		m_bDropTargetHelper = FALSE;
	}

	CUIDropTarget::~CUIDropTarget(void)
	{
		if (m_pDropHelper)
		{
			m_pDropHelper->Release();
			m_pDropHelper = NULL;
		}
		
		m_bDropTargetHelper = FALSE;
		m_lRefCount = 0;
	}

	BOOL CUIDropTarget::DragDropRegister(HWND hWnd, DWORD AcceptKeyState /*= MK_LBUTTON*/, IDropTargetCallBack* pUIDropTarget /*= NULL*/)
	{
		if (!IsWindow(hWnd)) return FALSE;
		m_pUIDropTarget = pUIDropTarget;

		HRESULT hRet = ::RegisterDragDrop(hWnd, this);
		m_hWnd = hWnd;

		if (SUCCEEDED(hRet))
		{
			m_dAcceptKeyState = AcceptKeyState;
			return true;
		}
		else { return FALSE; }
	}

	BOOL CUIDropTarget::DragDropRevoke(HWND hWnd)
	{
		if (!IsWindow(hWnd))
			return FALSE;

		HRESULT hRet = ::RevokeDragDrop(hWnd);

		return SUCCEEDED(hRet);
	}

	BOOL CUIDropTarget::SetDropTargetHelper(BOOL bEnable)
	{
		m_bDropTargetHelper = bEnable;
		// Create an instance of the shell DnD helper object.
		
		//如果为开启，并且对象不为空
		if (bEnable != FALSE && m_pDropHelper == NULL)
		{
			HRESULT hRet = CoCreateInstance(CLSID_DragDropHelper,NULL,
														CLSCTX_INPROC_SERVER,IID_IDropTargetHelper,(LPVOID*)&m_pDropHelper);
			//如果创建COM对象失败，关闭接口开关
			if (FAILED(hRet))
				m_bDropTargetHelper = FALSE;
		}
		else if (m_pDropHelper != NULL && bEnable == FALSE)
		{
			m_pDropHelper->Release();
			m_pDropHelper = NULL;
		}
		
		return m_bDropTargetHelper;
	}

	HRESULT CUIDropTarget::QueryInterface(REFIID riid, void **ppvObject)
	{
		static const QITAB qit[] =
		{
			QITABENT(CUIDropTarget, IDropTarget),
			{ 0 }
		};

		return QISearch(this, qit, riid, ppvObject);
	}

	ULONG CUIDropTarget::AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	ULONG CUIDropTarget::Release()
	{
		ULONG lRef = InterlockedDecrement(&m_lRefCount);
		if (0 == lRef)
			delete this;

		return m_lRefCount;
	}

	HRESULT CUIDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		//默认为COPY行为
		*pdwEffect = DROPEFFECT_COPY;

		if (grfKeyState != m_dAcceptKeyState)
		{
			*pdwEffect = DROPEFFECT_NONE;
			return S_OK;
		}

		//我这里只关心CE_HDROP类型，如果需要，可以调用EnumDragData函数来枚举所有类型
		//EnumDragData(pDataObj);
		FORMATETC cFmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}; 
		GetDragData(pDataObj, cFmt);

		HRESULT hRet = S_OK;
		if (m_pUIDropTarget != NULL)
		{
			m_pUIDropTarget->OnDragEnter(pDataObj,grfKeyState,pt,pdwEffect);
			if (FAILED(hRet))
				return hRet;
		}

		if (m_bDropTargetHelper)
			m_pDropHelper->DragEnter(m_hWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);

		return hRet;
	}

	HRESULT CUIDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		if (grfKeyState != m_dAcceptKeyState)
		{
			*pdwEffect = DROPEFFECT_NONE;
			return S_OK;
		}

		HRESULT hRet = S_OK;

		if (m_pUIDropTarget != NULL)
		{
			m_pUIDropTarget->OnDragOver(grfKeyState, pt, pdwEffect);
			if (FAILED(hRet))
				return hRet;
		}		

		if (m_bDropTargetHelper)
			m_pDropHelper->DragOver((LPPOINT)&pt, *pdwEffect);

		return hRet;
	}

	HRESULT CUIDropTarget::DragLeave()
	{
		if (m_bDropTargetHelper)
			m_pDropHelper->DragLeave();

		HRESULT hRet = S_OK;
		if (m_pUIDropTarget != NULL)
			hRet = m_pUIDropTarget->OnDragLeave();

		return hRet;
	}

	HRESULT CUIDropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		HRESULT hRet = S_OK;
		if (m_pUIDropTarget != NULL)
		{
			hRet = m_pUIDropTarget->OnDrop(pDataObj, grfKeyState, pt, pdwEffect);
			if (FAILED(hRet))
				return hRet;
		}

		if (m_bDropTargetHelper)
			m_pDropHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);

		m_DragData.Empty();

		return S_OK;
	}

	BOOL CUIDropTarget::EnumDragData(IDataObject *pDataObject)
	{
		IEnumFORMATETC *pEnumFmt = NULL;  

		do 
		{
			//如果获取成功，则可以通过IEnumFORMATETC接口的Next方法，来枚举所有的数据格式：  
			HRESULT hRet = pDataObject->EnumFormatEtc(DATADIR_GET,&pEnumFmt);  
			pEnumFmt->Reset();  

			if (FAILED(hRet))
				break;

			FORMATETC cFmt = {0};  
			ULONG nFetched = 0;

			while(TRUE)  
			{
				hRet = pEnumFmt->Next(1, &cFmt, &nFetched);  
				if (FAILED(hRet))
					break;

				switch (cFmt.cfFormat)
				{
					case CF_HDROP:
						if (GetDragData(pDataObject, cFmt))
							return TRUE;
						break;
					default:
						break;
				}
				//暂时直接退出，防止循环
				//break;
			}
		} while (FALSE);
	
		if (pEnumFmt != NULL)
		{
			pEnumFmt->Release();
			pEnumFmt = NULL;
		}

		return FALSE;
	}

	BOOL CUIDropTarget::GetDragData(IDataObject *pDataObject, FORMATETC cFmt)
	{  
		STGMEDIUM stgMedium;  

		HRESULT hRet = S_OK;

		do 
		{
			hRet = pDataObject->GetData(&cFmt, &stgMedium);
			if (FAILED(hRet))
				break;

			if (stgMedium.pUnkForRelease != NULL)  
				break;

			//可以扩充这块代码，配合EnumDragData函数来保存更多类型的数据  
			switch (stgMedium.tymed)  
			{  
				case TYMED_HGLOBAL:  
				{
					LPDRAGDATA pData = new DRAGDATA;
					pData->cfFormat = cFmt.cfFormat;
					memcpy(&pData->stgMedium, &stgMedium, sizeof(STGMEDIUM));  

					m_DragData.Add(pData);

					return true;  
					break;  
				}  
				//暂时只关心 TYMED_HGLOBAL 这一种类型
				default:
					::ReleaseStgMedium(&stgMedium);
				break;  
			}  
		} while (FALSE);

		return FALSE;  
	}  


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//CUIDataObject
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CUIDataObject::CUIDataObject()
	{
		m_lRefCount  = 1;
	}

	CUIDataObject::~CUIDataObject()
	{
		for(size_t i = 0; i < m_StgMedium.size(); ++i)
		{
			ReleaseStgMedium(m_StgMedium[i]);
			delete m_StgMedium[i];
		}
		for(size_t j = 0; j < m_ArrFormatEtc.size(); ++j)
			delete m_ArrFormatEtc[j];
	}

	ULONG CUIDataObject::AddRef(void)  
	{ 
		return InterlockedIncrement(&m_lRefCount);  
	}  

	ULONG CUIDataObject::Release(void)  
	{ 
		ULONG lRef = InterlockedDecrement(&m_lRefCount); 

		if (lRef == 0)
			delete this; 
		
		return lRef;  
	}  

	HRESULT CUIDataObject::QueryInterface(REFIID riid, void **ppvObject)  
	{  
		static const QITAB qit[] =  
		{  
			QITABENT(CUIDataObject, IDataObject),  
			{ 0 }  
		};  

		return QISearch(this, qit, riid, ppvObject); 
	}  

	HRESULT CUIDataObject::CreateDataObject(IDataObject **ppDataObject)  
	{  
		if (ppDataObject == 0)  
			return E_INVALIDARG;  

		*ppDataObject = new CUIDataObject; 

		return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;  
	}  

	HRESULT CUIDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
	{  
		if ((NULL == pMedDest) || (NULL ==pMedSrc) || (NULL == pFmtSrc))
			return E_INVALIDARG; 

		switch(pMedSrc->tymed)  
		{
			case TYMED_HGLOBAL:  
					pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
				break;  
			case TYMED_GDI:  
					pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);  
				break;  
			case TYMED_MFPICT:  
					pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);  
				break;  
			case TYMED_ENHMF:  
					pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);  
				break;  
			case TYMED_FILE:  
					pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);  
				break;  
			case TYMED_ISTREAM:  
					pMedDest->pstm = pMedSrc->pstm;
					pMedSrc->pstm->AddRef();  
				break;  
			case TYMED_ISTORAGE:  
					pMedDest->pstg = pMedSrc->pstg;
					pMedSrc->pstg->AddRef();
				break;  
			case TYMED_NULL: 
				break;
			default:  
			break; 
		}  

		pMedDest->tymed = pMedSrc->tymed;  
		pMedDest->pUnkForRelease = NULL;  

		if(pMedSrc->pUnkForRelease != NULL)  
		{  
			pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
			pMedSrc->pUnkForRelease->AddRef();  
		}  

		return S_OK;  
	}  

	HRESULT CUIDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)  
	{  
		if (ppEnumFormatEtc == NULL)
			return E_POINTER;

		*ppEnumFormatEtc = NULL;
		switch (dwDirection)
		{
		case DATADIR_GET:
			*ppEnumFormatEtc= new CUIEnumFormatEtc(m_ArrFormatEtc);
			if (*ppEnumFormatEtc == NULL)
				return E_OUTOFMEMORY;
			(*ppEnumFormatEtc)->AddRef(); 
			break;
		case DATADIR_SET:
		default:
			return E_NOTIMPL;
			break;
		}

		return S_OK;
	}  

	HRESULT CUIDataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)  
	{  
		if (pFormatEtc == NULL || pMedium == NULL)
			return E_INVALIDARG;
		pMedium->hGlobal = NULL;

		ATLASSERT(m_StgMedium.size() == m_ArrFormatEtc.size());
		for (size_t i=0; i < m_ArrFormatEtc.size(); ++i)
		{
			if (pFormatEtc->tymed & m_ArrFormatEtc[i]->tymed &&
				pFormatEtc->dwAspect == m_ArrFormatEtc[i]->dwAspect &&
				pFormatEtc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
			{
				CopyMedium(pMedium, m_StgMedium[i], m_ArrFormatEtc[i]);
				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}  

	HRESULT CUIDataObject::GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)  
	{  
		return E_NOTIMPL;
	}  

	HRESULT CUIDataObject::QueryGetData(FORMATETC *pFormatEtc)  
	{  
		if (pFormatEtc == NULL)
			return E_INVALIDARG;

		if ((DVASPECT_CONTENT & pFormatEtc->dwAspect) == FALSE)
			return DV_E_DVASPECT; 

		HRESULT hRet = DV_E_TYMED;  
		for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i)  
		{  
			if (pFormatEtc->tymed & m_ArrFormatEtc[i]->tymed)
			{
				if (pFormatEtc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
					return S_OK;
				else
					hRet = DV_E_CLIPFORMAT;
			}
			else
				hRet = DV_E_TYMED;
		}  
		return hRet; 
	}  

	HRESULT CUIDataObject::GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)  
	{  
		if (pFormatEtcOut == NULL)
			return E_INVALIDARG;
		return DATA_S_SAMEFORMATETC;
	}  

	HRESULT CUIDataObject::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium,  BOOL fRelease)
	{  
		if (pFormatEtc == NULL || pMedium == NULL)
			return E_INVALIDARG;

		ATLASSERT(pFormatEtc->tymed == pMedium->tymed);
		FORMATETC* pFetc=new FORMATETC;
		STGMEDIUM* pStgMed = new STGMEDIUM;

		if (pFetc == NULL || pStgMed == NULL)
			return E_OUTOFMEMORY;

		ZeroMemory(pFetc, sizeof(FORMATETC));
		ZeroMemory(pStgMed, sizeof(STGMEDIUM));

		*pFetc = *pFormatEtc;
		m_ArrFormatEtc.push_back(pFetc);

		if (fRelease)
			*pStgMed = *pMedium;
		else
			CopyMedium(pStgMed, pMedium, pFormatEtc);
		
		m_StgMedium.push_back(pStgMed);

		return S_OK;
	}  

	HRESULT CUIDataObject::DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)  
	{  
		return OLE_E_ADVISENOTSUPPORTED;  
	}  

	HRESULT CUIDataObject::DUnadvise(DWORD dwConnection)  
	{  
		return E_NOTIMPL;  
	}  

	HRESULT CUIDataObject::EnumDAdvise(IEnumSTATDATA **ppEnumAdvise)  
	{  
		return OLE_E_ADVISENOTSUPPORTED;  
	}  

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//CUIDragSourceHelper
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CUIDragSourceHelper::CUIDragSourceHelper(IDataObject* pDataObject)
		:m_pDataObject(NULL)
	{
		HRESULT hRet = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, 
													IID_IDragSourceHelper, (void**)&m_pDragSourceHelper);
		if (FAILED(hRet))
			m_pDragSourceHelper = NULL;

		SetObject(pDataObject);
	}

	CUIDragSourceHelper::~CUIDragSourceHelper()
	{
		if (m_pDragSourceHelper != NULL)
		{
			m_pDragSourceHelper->Release();
			m_pDragSourceHelper = NULL;
		}
		if (m_pDataObject != NULL)
		{
			m_pDataObject->Release();
			m_pDataObject = NULL;
		}
	}

	void CUIDragSourceHelper::Release()
	{
		delete this;
	}

	HRESULT CUIDragSourceHelper::InitializeFromBitmap(HBITMAP hBitmap, POINT& pt, RECT& rcPos, COLORREF crColorKey /*=GetSysColor(COLOR_WINDOW)*/)
	{
		if (m_pDragSourceHelper == NULL)
			return E_FAIL;

		SHDRAGIMAGE ShDragImage = {0};
		SIZE szImage={rcPos.right-rcPos.left,rcPos.bottom-rcPos.top};
		ShDragImage.sizeDragImage = szImage;
		ShDragImage.hbmpDragImage = hBitmap;
		ShDragImage.crColorKey = crColorKey;
		ShDragImage.ptOffset.x = pt.x - rcPos.left;
		ShDragImage.ptOffset.y = pt.y - rcPos.top;

		return m_pDragSourceHelper->InitializeFromBitmap(&ShDragImage, m_pDataObject);
	}

	HRESULT CUIDragSourceHelper::InitializeFromWindow(HWND hwnd, POINT& pt)
	{		
		if (m_pDragSourceHelper == NULL)
			return E_FAIL;
		if (m_pDataObject == NULL)
			return E_POINTER;

		return m_pDragSourceHelper->InitializeFromWindow(hwnd, &pt, m_pDataObject);
	}

	BOOL CUIDragSourceHelper::SetObject(IDataObject* pDataObject)
	{
		if (pDataObject == NULL)
			return FALSE;
		//减少对象引用次数
		if (m_pDataObject != NULL)
			m_pDataObject->Release();

		//增加对象引用次数
		m_pDataObject = pDataObject;
		m_pDataObject->AddRef();

		return TRUE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//CUIEnumFormatEtc
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CUIEnumFormatEtc::CUIEnumFormatEtc(const FormatEtcArray& ArrFE):
	m_cRefCount(0),m_iCur(0)
	{
		for(size_t i = 0; i < ArrFE.size(); ++i)
			m_pFmtEtc.push_back(ArrFE[i]);
	}

	CUIEnumFormatEtc::CUIEnumFormatEtc(const PFormatEtcArray& ArrFE):
	m_cRefCount(0),m_iCur(0)
	{
		for(size_t i = 0; i < ArrFE.size(); ++i)
			m_pFmtEtc.push_back(*ArrFE[i]);
	}

	STDMETHODIMP  CUIEnumFormatEtc::QueryInterface(REFIID refiid, void FAR* FAR* ppv)
	{
		*ppv = NULL;
		if (IID_IUnknown==refiid || IID_IEnumFORMATETC==refiid)
			*ppv=this;

		if (*ppv != NULL)
		{
			((LPUNKNOWN)*ppv)->AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) CUIEnumFormatEtc::AddRef(void)
	{
		return ++m_cRefCount;
	}

	STDMETHODIMP_(ULONG) CUIEnumFormatEtc::Release(void)
	{
		long nTemp = --m_cRefCount;
		ATLASSERT(nTemp >= 0);
		if(nTemp == 0)
			delete this;

		return nTemp; 
	}

	STDMETHODIMP CUIEnumFormatEtc::Next( ULONG celt,LPFORMATETC lpFormatEtc, ULONG FAR *pceltFetched)
	{
		if(pceltFetched != NULL)
			*pceltFetched=0;

		ULONG cReturn = celt;

		if(celt <= 0 || lpFormatEtc == NULL || m_iCur >= m_pFmtEtc.size())
			return S_FALSE;

		if(pceltFetched == NULL && celt != 1) // pceltFetched can be NULL only for 1 item request
			return S_FALSE;

		while (m_iCur < m_pFmtEtc.size() && cReturn > 0)
		{
			*lpFormatEtc++ = m_pFmtEtc[m_iCur++];
			--cReturn;
		}
		if (pceltFetched != NULL)
			*pceltFetched = celt - cReturn;

		return (cReturn == 0) ? S_OK : S_FALSE;
	}

	STDMETHODIMP CUIEnumFormatEtc::Skip(ULONG celt)
	{
		if ((m_iCur + int(celt)) >= m_pFmtEtc.size())
			return S_FALSE;
		m_iCur += celt;
		return S_OK;
	}

	STDMETHODIMP CUIEnumFormatEtc::Reset(void)
	{
		m_iCur = 0;
		return S_OK;
	}

	STDMETHODIMP CUIEnumFormatEtc::Clone(IEnumFORMATETC FAR * FAR*ppCloneEnumFormatEtc)
	{
		if(ppCloneEnumFormatEtc == NULL)
			return E_POINTER;

		CUIEnumFormatEtc *newEnum = new CUIEnumFormatEtc(m_pFmtEtc);
		if(newEnum ==NULL)
			return E_OUTOFMEMORY;  	
		newEnum->AddRef();
		newEnum->m_iCur = m_iCur;
		*ppCloneEnumFormatEtc = newEnum;
		return S_OK;
	}  //class CUIEnumFormatEtc
}//namespace DuiLib