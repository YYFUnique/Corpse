#include "StdAfx.h"
#include "DynamicLibrary.h"

DynamicLibrary::~DynamicLibrary()
{

}

DynamicLibrary* DynamicLibrary::Load(LPCTSTR lpszModulePath)
{
	return static_cast<DynamicLibrary*>(new DynamicLibraryImpl(lpszModulePath));
}

DynamicLibraryImpl::DynamicLibraryImpl(LPCTSTR lpszModulePath)
{
	m_hModule = LoadLibrary(lpszModulePath);
}

DynamicLibraryImpl::~DynamicLibraryImpl()
{
	if (m_hModule != NULL)
		FreeLibrary(m_hModule);
}

Function DynamicLibraryImpl::FindFunction(LPCSTR lpszFunName)
{
	if (m_hModule != NULL) {
		// C++ standard doesn't like casts betwen function pointers and void pointers so use a union
		union {
			FARPROC fp;
			Function f;
		} fnConv;
		fnConv.fp = ::GetProcAddress(m_hModule, lpszFunName);
		return fnConv.f;
	}

	return NULL;
}

BOOL DynamicLibraryImpl::IsValid() const
{
	return m_hModule != NULL;
}