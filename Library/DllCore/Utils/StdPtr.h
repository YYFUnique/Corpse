#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <atlstr.h>

class DLL_API CStdArray
{
public:
	CStdArray();
	~CStdArray();

	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);
	BOOL IsEmpty() const;

	INT_PTR Add(LPCTSTR newElement);
	INT_PTR Add(const CString& newElement);

	INT_PTR Append(const CStdArray& src);
	void Copy(const CStdArray& src);

	DWORD Find(LPCTSTR lpszItem);

	const CString& GetAt(INT_PTR nIndex) const;
	CString& ElementAt(INT_PTR nIndex);

	void SetAt(INT_PTR nIndex, LPCTSTR newElement);

	void SetAt(INT_PTR nIndex, const CString& newElement);

	void InsertAt(INT_PTR nIndex, LPCTSTR newElement, INT_PTR nCount = 1);

	void InsertAt(INT_PTR nIndex, const CString& newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void RemoveAll();

	const CString* GetData() const;
	CString* GetData();

	INT_PTR GetSize() const;
	INT_PTR GetCount() const;

	const CString& operator[](INT_PTR nIndex) const;
	CString& operator[](INT_PTR nIndex);

protected:
	void SetAtGrow(INT_PTR nIndex, LPCTSTR newElement);

	void SetAtGrow(INT_PTR nIndex, const CString& newElement);

	void InsertEmpty(INT_PTR nIndex, INT_PTR nCount);
protected:
	CString* m_pData;	
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount
};