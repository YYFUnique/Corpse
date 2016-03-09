#pragma once

class CDropTarget : public IDropTarget
{
public:
	CDropTarget();
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject,DWORD grfKeyState, POINTL pt,DWORD * pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect);
protected:
	ULONG m_tbRefCount;
};