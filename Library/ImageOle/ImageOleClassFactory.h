#pragma once

class CImageOleClassFactory : IClassFactory
{
public:
	CImageOleClassFactory();
	~CImageOleClassFactory();

public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID riid, void** ppv);

	// IClassFactory
	STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppv);
	STDMETHOD(LockServer)(BOOL fLock);

protected:
	ULONG m_uRefCount;
};