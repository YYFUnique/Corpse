#pragma once

//{24c7f2bb-3f48-42eb-9d6f-81b756ea15ba}
DEFINE_GUID(CLSID_ShellHook, 0x24c7f2bb, 0x3f48, 0x42eb, 0x9d, 0x6f, 0x81, 0xb7, 0x56, 0xea, 0x15, 0xba);

class CShellHook : public ICopyHook, public IShellExecuteHook
{
public:
	CShellHook();
	~CShellHook();

	HRESULT STDMETHODCALLTYPE QueryInterface (REFIID iid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef ();
	ULONG STDMETHODCALLTYPE Release ();

	UINT STDMETHODCALLTYPE CopyCallback (HWND hwnd, UINT wFunc, UINT wFlags, LPCTSTR pszSrcFile, DWORD dwSrcAttribs,
		LPCTSTR pszDestFile, DWORD dwDestAttribs);

	HRESULT STDMETHODCALLTYPE Execute (LPSHELLEXECUTEINFO pei);
private:
	ULONG      m_lRefCount;
};
