#pragma once
#include "../DllCore.h"
#include <BaseTsd.h>

class CNodeData
{
protected:
	CNodeData(DWORD dwLen){
		pData = new BYTE[dwLen];
	}
	virtual ~CNodeData(){
		if (pData != NULL)
			delete[] pData;
	}
protected:
	BYTE* pData;
	CNodeData* pNext;
public:
	void* GetNodeData() { return pData; }
	static CNodeData* PASCAL Create(CNodeData*& pHead, UINT_PTR nMax, UINT_PTR cbElement){
		if (nMax == 0 || cbElement == 0)
			return NULL;

		CNodeData* pNode = (CNodeData*) new CNodeData(nMax * cbElement);

		pNode->pNext = pHead;
		pHead = pNode; 
		return pNode;
	}

	// 利用虚函数的动态绑定技术，将原本在exe模块执行的代码
	// 通过虚函数自动调制到内存分配模块空间
	virtual void FreeDataChain(){
		CNodeData* pNode = this;
		while (pNode != NULL)
		{
			CNodeData* pNext = pNode->pNext;
			delete pNode;
			pNode = pNext;
		}
	}
};

#ifndef __POSITION
typedef struct __POSITION
{
}__POSITION,*POSITION;
#endif
template<class TYPE, class ARG_TYPE = const TYPE&>
class CDuiList /*: public CObject*/
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
	CDuiList(INT_PTR nBlockSize = 10);
	~CDuiList();
public:
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	BOOL IsEmpty() const;

	TYPE& GetHead();
	const TYPE& GetHead() const;
	TYPE& GetTail();
	const TYPE& GetTail() const;

	TYPE RemoveHead();
	TYPE RemoveTail();

	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);

	void AddHead(CDuiList* pNewList);
	void AddTail(CDuiList* pNewList);

	void RemoveAll();

	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	TYPE& GetNext(POSITION& rPosition); 
	const TYPE& GetNext(POSITION& rPosition) const;
	TYPE& GetPrev(POSITION& rPosition);
	const TYPE& GetPrev(POSITION& rPosition) const;

	TYPE& GetAt(POSITION position);
	const TYPE& GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newElement);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
	POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	INT_PTR m_nCount;
	CNode* m_pNodeFree;
	CNodeData* m_pBlocks;
	INT_PTR m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);
};


#define DUI_INLINE	/*inline*/

template<class TYPE, class ARG_TYPE>
DUI_INLINE INT_PTR CDuiList<TYPE, ARG_TYPE>::GetCount() const
{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE INT_PTR CDuiList<TYPE, ARG_TYPE>::GetSize() const
{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE BOOL CDuiList<TYPE, ARG_TYPE>::IsEmpty() const
{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE TYPE& CDuiList<TYPE, ARG_TYPE>::GetHead()
{ /*ENSURE(m_pNodeHead != NULL);*/
return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE const TYPE& CDuiList<TYPE, ARG_TYPE>::GetHead() const
{ ENSURE(m_pNodeHead != NULL);
return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE TYPE& CDuiList<TYPE, ARG_TYPE>::GetTail()
{ ENSURE(m_pNodeTail != NULL);
return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE const TYPE& CDuiList<TYPE, ARG_TYPE>::GetTail() const
{ ENSURE(m_pNodeTail != NULL);
return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE POSITION CDuiList<TYPE, ARG_TYPE>::GetHeadPosition() const
{ return (POSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE POSITION CDuiList<TYPE, ARG_TYPE>::GetTailPosition() const
{ return (POSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE TYPE& CDuiList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) // return *Position++
{ CNode* pNode = (CNode*) rPosition;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
rPosition = (POSITION) pNode->pNext;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE const TYPE& CDuiList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const // return *Position++
{ CNode* pNode = (CNode*) rPosition;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
rPosition = (POSITION) pNode->pNext;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE TYPE& CDuiList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) // return *Position--
{ CNode* pNode = (CNode*) rPosition;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
rPosition = (POSITION) pNode->pPrev;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE const TYPE& CDuiList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const // return *Position--
{ CNode* pNode = (CNode*) rPosition;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
rPosition = (POSITION) pNode->pPrev;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE TYPE& CDuiList<TYPE, ARG_TYPE>::GetAt(POSITION position)
{ CNode* pNode = (CNode*) position;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE const TYPE& CDuiList<TYPE, ARG_TYPE>::GetAt(POSITION position) const
{ CNode* pNode = (CNode*) position;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
return pNode->data; }
template<class TYPE, class ARG_TYPE>
DUI_INLINE void CDuiList<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newElement)
{ CNode* pNode = (CNode*) pos;
/*ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));*/
pNode->data = newElement; }

template<class TYPE, class ARG_TYPE>
CDuiList<TYPE, ARG_TYPE>::CDuiList(INT_PTR nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void CDuiList<TYPE, ARG_TYPE>::RemoveAll()
{
	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		pNode->data.~TYPE();

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	if (m_pBlocks != NULL)
	{
		// 利用虚函数的动态绑定技术，将原本在exe模块执行的代码
		// 通过虚函数自动调制到内存分配模块空间
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
}

template<class TYPE, class ARG_TYPE>
CDuiList<TYPE, ARG_TYPE>::~CDuiList()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class TYPE, class ARG_TYPE>
typename CDuiList<TYPE, ARG_TYPE>::CNode*
CDuiList<TYPE, ARG_TYPE>::NewNode(CNode* pPrev, CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CNodeData* pNewBlock = CNodeData::Create(m_pBlocks, m_nBlockSize,sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*)pNewBlock->GetNodeData();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (INT_PTR i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
/*	ENSURE(m_pNodeFree != NULL);  // we must have something*/

	CDuiList::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

#pragma push_macro("new")
#undef new
	::new( (void*)( &pNode->data ) ) TYPE;
#pragma pop_macro("new")
	return pNode;
}

template<class TYPE, class ARG_TYPE>
void CDuiList<TYPE, ARG_TYPE>::FreeNode(CNode* pNode)
{
	pNode->data.~TYPE();
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSITION CDuiList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CDuiList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CDuiList<TYPE, ARG_TYPE>::AddHead(CDuiList* pNewList)
{
	ASSERT_VALID(this);

	ENSURE(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements to head (maintain order)
	POSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void CDuiList<TYPE, ARG_TYPE>::AddTail(CDuiList* pNewList)
{
	/*ASSERT_VALID(this);
	ENSURE(pNewList != NULL);
	ASSERT_VALID(pNewList);*/

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE CDuiList<TYPE, ARG_TYPE>::RemoveHead()
{
	/*ASSERT_VALID(this);*/
	/*ENSURE(m_pNodeHead != NULL);  // don't call on empty list !!!*/
	/*ASSERT(AfxIsValidAddress(m_pNodeHead, sizeof(CNode)));*/

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
TYPE CDuiList<TYPE, ARG_TYPE>::RemoveTail()
{
	ASSERT_VALID(this);
	ENSURE(m_pNodeTail != NULL);  // don't call on empty list !!!
	ASSERT(AfxIsValidAddress(m_pNodeTail, sizeof(CNode)));

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSITION CDuiList<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CDuiList<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	ASSERT(AfxIsValidAddress(pOldNode, sizeof(CNode)));
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CDuiList<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	CNode* pOldNode = (CNode*) position;
	//ASSERT(AfxIsValidAddress(pOldNode, sizeof(CNode)));

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		//ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		//ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}
