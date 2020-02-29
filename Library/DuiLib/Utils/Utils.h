#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		BOOL EqualRect(const LPCRECT lprc);
		BOOL IsRectNull() const;
		BOOL IsRectEmpty() const;
		void CopyRect(LPCRECT lpSrcRect);
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
		bool PtInRect(POINT pt);
		void SetRect(POINT topLeft, POINT bottomRight);

		// absolute position of rectangle
		void Move(int x,int y);
		void MoveToY(int y) throw();
		void MoveToX(int x) throw();
		void MoveToXY(int x, int y) throw();
		void MoveToXY(POINT point) throw();
	};

	class UILIB_API CDuiImage
	{
	public:
		CDuiImage();
		~CDuiImage();
		void CreateEmptyImage(int nWidth,int nHeight,int nbb);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiArray
	{
	public:
		CDuiArray();
		~CDuiArray();

		void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);
		BOOL IsEmpty() const;

		INT_PTR Add(LPCTSTR newElement);
		INT_PTR Add(const CString& newElement);

		INT_PTR Append(const CDuiArray& src);
		void Copy(const CDuiArray& src);

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

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		bool RemoveAll();
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	template<typename T = LPVOID,typename T1 = T>
	class UILIB_API TStdValArray : public CStdValArray
	{
	public:
		TStdValArray(int iElementSize = sizeof(T), int iPreallocSize = 0):CStdValArray(iElementSize,iPreallocSize){};
		bool Add(const T1 pData){return CStdValArray::Add((LPCVOID)&pData);};
		bool InsertAt(int iIndex,const T pData){
			if( iIndex == m_nCount ) return Add(pData);
			if( iIndex < 0 || iIndex > m_nCount ) return false;
			if( ++m_nCount >= m_nAllocated) {
				int nAllocated = m_nAllocated * 2;
				if( nAllocated == 0 ) nAllocated = 11;
				LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
				if( pVoid != NULL ) {
					m_nAllocated = nAllocated;
					m_pVoid = pVoid;
				}
				else {
					--m_nCount;
					return false;
				}
			}

			memmove(&m_pVoid + (iIndex+1) * m_iElementSize, &m_pVoid + iIndex * m_iElementSize,m_iElementSize);
			::CopyMemory(m_pVoid + (iIndex * m_iElementSize), &pData, m_iElementSize);
			return true;
		};
		bool SetAt(int iIndex, const T pData) {
			if( iIndex < 0 || iIndex > m_nCount ) return false;
			::CopyMemory(m_pVoid + (iIndex * m_iElementSize), &pData, m_iElementSize);
			return true;
		};
		T GetData(){return static_cast<T>(CStdValArray::GetData());};
		bool SetData() {

		};
		T GetAt(int iIndex) const {return *static_cast<T1*>(CStdValArray::GetAt(iIndex));};
		T operator[](int nIndex) const{return (T)CStdValArray::operator[](nIndex);};
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	template<typename T = LPVOID>
	class UILIB_API TStdPtrArray : public CStdPtrArray
	{
	public:
		TStdPtrArray(int iPreallocSize = 0):CStdPtrArray(iPreallocSize){};
		TStdPtrArray(const TStdPtrArray& src):CStdPtrArray(src){};
		int Find(T iIndex) const {return CStdPtrArray::Find(iIndex);};
		bool Add(T pData){return CStdPtrArray::Add(pData);};
		bool SetAt(int iIndex, T pData){return CStdPtrArray::SetAt(iIndex,pData);};
		bool InsertAt(int iIndex, T pData){return CStdPtrArray::InsertAt(iIndex,pData);};
		bool Remove(int iIndex,bool bDeleteObj = false){
			if(bDeleteObj){
				T p = GetAt(iIndex);
				if(p)
					delete p;
			}
			return CStdPtrArray::Remove(iIndex);
		}
		T* GetData(){return static_cast<T>(CStdPtrArray::GetData());};
		T GetAt(int iIndex) const {return static_cast<T>(CStdPtrArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return static_cast<T>(CStdPtrArray::operator[](nIndex));};
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();
		
		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;
		operator LPCTSTR();

		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& CDuiString::operator=(LPCSTR lpStr);
		const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
		const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
		const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;

		CDuiString& Trim( _In_ TCHAR szTarget);
		CDuiString& Trim(_In_ LPCTSTR lpszTarget);
		CDuiString& TrimRight( _In_ TCHAR szTarget );
		CDuiString& TrimLeft( _In_ TCHAR szTarget );
		CDuiString& TrimRight( _In_ LPCTSTR lpszTarget );
		CDuiString& TrimLeft( _In_ LPCTSTR lpszTarget );

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;

		int FindOneOf(LPCTSTR pstr, int iPos = 0) const;

		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
        int __cdecl Format(LPCTSTR pstrFormat, va_list Args);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
		int __cdecl AppendFormat(LPCTSTR pstrFormat, ...);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		LPVOID Find(UINT nId,bool optimize = true) const;
		bool Insert(UINT nId,LPVOID pData);
		LPVOID Set(UINT nId,LPVOID pData);
		bool Remove(LPCTSTR key);
		bool Remove(UINT nId);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

}// namespace DuiLib

#endif // __UTILS_H__