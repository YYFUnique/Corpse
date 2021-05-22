#pragma once

#include "UserDefine.h"
/**
 * Dynamic Library (DLL/SO/...) loading
 */

class DynamicLibrary
{
public:
	virtual ~DynamicLibrary();
public:
	/// @return Pointer to function "name", or NULL on failure.
	virtual Function FindFunction(LPCSTR lpszFunName) = 0;

	/// @return true if the library was loaded successfully.
	virtual BOOL IsValid() const = 0;

	/// @return An instance of a DynamicLibrary subclass with "modulePath" loaded.
	static DynamicLibrary* Load(LPCTSTR lpszModulePath);
};

class DynamicLibraryImpl : public DynamicLibrary
{
public:
	DynamicLibraryImpl(LPCTSTR lpszModulePath);
	virtual ~DynamicLibraryImpl();
public:
	// Use GetProcAddress to get a pointer to the relevant function.
	virtual Function FindFunction(LPCSTR lpszFunName);
	virtual BOOL IsValid() const;
protected:
	HMODULE m_hModule;
};