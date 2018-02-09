#pragma once

//#include <comdef.h>

// {C5B3A481-21E3-4cab-8ABD-3C6A85892AD2}
_declspec(selectany) CLSID CLSID_Monitor = { 0xc5b3a481, 0x21e3, 0x4cab, { 0x8a, 0xbd, 0x3c, 0x6a, 0x85, 0x89, 0x2a, 0xd2 } };

// {E41FE3BB-7218-423d-952E-96E556AEE30F}
_declspec(selectany) IID IID_IMonitor = { 0xe41fe3bb, 0x7218, 0x423d, { 0x95, 0x2e, 0x96, 0xe5, 0x56, 0xae, 0xe3, 0x0f } };

interface __declspec(uuid("E41FE3BB-7218-423d-952E-96E556AEE30F")) 
IMonitor : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE AddAppInfo(LPCTSTR lpszFilePath) = 0;
};