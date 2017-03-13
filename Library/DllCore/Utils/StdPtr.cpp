#include "StdAfx.h"
#include "StdPtr.h"

static inline void ConstructElement(CString* pNewData)
{
	ASSERT(pNewData!=NULL);
	new( pNewData ) CString;
}


static inline void DestructElement(CString* pOldData)
{
	ASSERT(pOldData!=NULL);
	pOldData->~CString();
}

static void _CopyElements(CString* pDest, CString* pSrc, INT_PTR nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
}

static void _ConstructElements(CString* pNewData, INT_PTR nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		ConstructElement(pNewData);
		pNewData++;
	}
}

static void _DestructElements(CString* pOldData, INT_PTR nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		DestructElement(pOldData);
		pOldData++;
	}
}

CStdArray::CStdArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CStdArray::~CStdArray()
{
	_DestructElements(m_pData, m_nSize);
	delete[] (BYTE*)m_pData;
}

INT_PTR CStdArray::GetSize() const
{ 
	return m_nSize; 
}

INT_PTR CStdArray::GetCount() const
{
	return m_nSize; 
}

const CString* CStdArray::GetData() const
{ 
	return (const CString*)m_pData; 
}

CString* CStdArray::GetData()
{
	return (CString*)m_pData; 
}

INT_PTR CStdArray::Add(LPCTSTR newElement)
{ 
	INT_PTR nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

INT_PTR CStdArray::Add(const CString& newElement)
{
	INT_PTR nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

INT_PTR CStdArray::Append(const CStdArray& src)
{
	ASSERT(this != &src);   // cannot append to itself

	INT_PTR nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	_CopyElements(m_pData + nOldSize, src.m_pData, src.m_nSize);

	return nOldSize;
}

void CStdArray::Copy(const CStdArray& src)
{
	ASSERT(this != &src);   // cannot append to itself

	if(this != &src)
	{
		SetSize(src.m_nSize);
		_CopyElements(m_pData, src.m_pData, src.m_nSize);
	}
}

BOOL CStdArray::IsEmpty() const
{ 
	return m_nSize == 0; 
}

void CStdArray::RemoveAll()
{ 
	SetSize(0);
}

DWORD CStdArray::Find(LPCTSTR lpszItem)
{
	for (int n=0;n<m_nSize;++n)
	{
		const CString& strItem = m_pData->GetAt(n);
		if (strItem.CompareNoCase(lpszItem) == 0)
			return n;
	}
	return -1;
}

const CString& CStdArray::GetAt(INT_PTR nIndex) const
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}

void CStdArray::SetAt(INT_PTR nIndex, LPCTSTR newElement)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement; 
}

void CStdArray::SetAt(INT_PTR nIndex, const CString& newElement)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement;
}

CString& CStdArray::ElementAt(INT_PTR nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}

void CStdArray::SetAtGrow(INT_PTR nIndex, LPCTSTR newElement)
{
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}

void CStdArray::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	ASSERT(nNewSize >= 0);

	if (nGrowBy >= 0)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing

		_DestructElements(m_pData, m_nSize);
		delete[] (BYTE*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(CString));    // no overflow
#endif
		m_pData = (CString*) new BYTE[nNewSize * sizeof(CString)];

		_ConstructElements(m_pData, nNewSize);

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			_ConstructElements(&m_pData[m_nSize], nNewSize-m_nSize);

		}

		else if (m_nSize > nNewSize)  // destroy the old elements
			_DestructElements(&m_pData[nNewSize], m_nSize-nNewSize);

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		INT_PTR nGrowArrayBy = m_nGrowBy;
		if (nGrowArrayBy == 0)
		{
			// heuristically determine growth when nGrowArrayBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowArrayBy = min(1024, max(4, m_nSize / 8));
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowArrayBy)
			nNewMax = m_nMaxSize + nGrowArrayBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(CString)); // no overflow
#endif
		CString* pNewData = (CString*) new BYTE[(size_t)nNewMax * sizeof(CString)];

		// copy new data from old 
		Checked::memcpy_s(pNewData, (size_t)nNewMax * sizeof(CString), 
			m_pData, (size_t)m_nSize * sizeof(CString));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);

		_ConstructElements(&pNewData[m_nSize], nNewSize-m_nSize);

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

void CStdArray::SetAtGrow(INT_PTR nIndex, const CString& newElement)
{
	ASSERT(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
	{
		// Make sure newElement is not a reference to an element in the array.
		// Or else, it will be invalidated by the reallocation.
		ASSERT(	(nIndex < m_nMaxSize) ||
			(&newElement < m_pData) ||
			(&newElement >= (m_pData + m_nMaxSize) ) );
		SetSize(nIndex+1);
	}
	m_pData[nIndex] = newElement;
}


void CStdArray::InsertAt(INT_PTR nIndex, LPCTSTR newElement, INT_PTR nCount)
{
	// make room for new elements
	InsertEmpty(nIndex, nCount);

	// copy elements into the empty space
	CString temp = newElement;
	while (nCount--)
		m_pData[nIndex++] = temp;
}

void CStdArray::InsertAt(INT_PTR nIndex, const CString& newElement, INT_PTR nCount)
{
	// make room for new elements
	InsertEmpty(nIndex, nCount);
	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

void CStdArray::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	INT_PTR nUpperBound = nIndex + nCount;
	ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nUpperBound);

	_DestructElements(&m_pData[nIndex], nCount);

	if (nMoveCount)
	{
		Checked::memmove_s(&m_pData[nIndex], nMoveCount * sizeof(CString), 
			&m_pData[nUpperBound], nMoveCount * sizeof(CString));
	}

	m_nSize -= nCount;
}

const CString& CStdArray::operator[](INT_PTR nIndex) const
{ 
	return GetAt(nIndex); 
}

CString& CStdArray::operator[](INT_PTR nIndex)
{ 
	return ElementAt(nIndex); 
}


void CStdArray::InsertEmpty(INT_PTR nIndex, INT_PTR nCount)
{
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		INT_PTR nOldSize = m_nSize;
		INT_PTR nOverlapSize = min(nCount, nOldSize - nIndex);

		SetSize(m_nSize + nCount);  // grow it to new size

		// destroy slots we are about to overwrite
		_DestructElements(&m_pData[m_nSize - nOverlapSize], nOverlapSize);

		// shift old data up to fill gap 
		Checked::memmove_s(&m_pData[nIndex+nCount], (m_nSize-(nIndex+nCount)) * sizeof(CString), 
			&m_pData[nIndex], (nOldSize-nIndex) * sizeof(CString));

		// re-init the now-vacant slots we moved data from
		_ConstructElements(&m_pData[nIndex], nOverlapSize);
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
}