#pragma once

class CShellCopyClassFactory : public IClassFactory
{
public:
	CShellCopyClassFactory();
	~CShellCopyClassFactory();

	HRESULT STDMETHODCALLTYPE QueryInterface (REFIID iid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef ();
	ULONG STDMETHODCALLTYPE Release ();

	HRESULT STDMETHODCALLTYPE CreateInstance (IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	HRESULT STDMETHODCALLTYPE LockServer (BOOL fLock);

protected:
	ULONG      m_lRefCount;
};