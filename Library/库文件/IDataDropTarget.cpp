#include "stdafx.h"
#include "IDataDropTarget.h"

namespace DuiLib
{
	IDataDropTarget::IDataDropTarget()
		:m_lRefCount(-1)
		,m_piDropHelper(NULL)
		,m_bUseDnDHelper(false)
		,m_hWnd(NULL)
	{
		HRESULT hRet = CoCreateInstance(CLSID_DragDropHelper,NULL,CLSCTX_INPROC_SERVER,IID_IDropTargetHelper,(void**)&m_piDropHelper);
		if (SUCCEEDED(hRet))
			m_bUseDnDHelper = true;
	}

	IDataDropTarget::~IDataDropTarget()
	{
		if (m_piDropHelper != NULL)
			m_piDropHelper->Release();
		m_bUseDnDHelper = false;
	}

	bool IDataDropTarget::DragDropRegister(HWND hWnd,DWORD AcceptKeyState)
	{
		if(IsWindow(hWnd) == FALSE)
			return false;

		m_hWnd = hWnd;
		HRESULT hRet = ::RegisterDragDrop (hWnd,this);
		
		if(FAILED(hRet))
			return false;

		m_dAcceptKeyState = AcceptKeyState;
		return true;
	}

	bool IDataDropTarget::DragDropRevoke()
	{
		if(IsWindow(m_hWnd) == FALSE)
			return false;

		HRESULT hRet = ::RevokeDragDrop(m_hWnd);

		return SUCCEEDED(hRet);
	}

	HRESULT STDMETHODCALLTYPE IDataDropTarget::QueryInterface(REFIID riid, __RPC__deref_out void **ppvObject)
	{
		HRESULT hRet = S_OK;
		if (IsBadWritePtr(ppvObject,sizeof(void*)))
			return E_POINTER;

		*ppvObject = NULL;
		if (InlineIsEqualGUID(riid,IID_IUnknown))
			*ppvObject = (IUnknown*) this;
		else if (InlineIsEqualGUID(riid,IID_IDropTarget))
			*ppvObject = (IDropTarget*) this;
		else if (InlineIsEqualGUID(riid,IID_IIDataDropTarget))
			*ppvObject = (IDataDropTarget*) this;
		else
			hRet = E_NOINTERFACE;

		if (SUCCEEDED(hRet))
			((IUnknown*)*ppvObject)->AddRef();

		return hRet;
	}

	ULONG STDMETHODCALLTYPE IDataDropTarget::AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	ULONG STDMETHODCALLTYPE IDataDropTarget::Release()
	{
		ULONG lRef = InterlockedDecrement(&m_lRefCount);
		if (lRef == 0)
			delete this;
		return m_lRefCount;
	}

	HRESULT STDMETHODCALLTYPE IDataDropTarget::DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
	{
		if (m_bUseDnDHelper)
			m_piDropHelper->DragEnter(m_hWnd,pDataObj,(LPPOINT)&pt,*pdwEffect);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE IDataDropTarget::DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
	{
		if (m_bUseDnDHelper)
			m_piDropHelper->DragOver((LPPOINT)&pt,*pdwEffect);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE IDataDropTarget::DragLeave()
	{
		if (m_bUseDnDHelper)
			m_piDropHelper->DragLeave();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE IDataDropTarget::Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
	{
		if (m_bUseDnDHelper)
			m_piDropHelper->Drop(pDataObj,(LPPOINT)&pt,*pdwEffect);
		return S_OK;
	}
}