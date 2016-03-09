#include "StdAfx.h"
#include "LsList.h"

CNodeData* CNodeData::Create(CNodeData*& pHead, UINT_PTR nMax, UINT_PTR cbElement)
{
	if (nMax == 0 || cbElement == 0)
		return NULL;

	CNodeData* pNode = (CNodeData*) new BYTE[sizeof(CNodeData) + nMax * cbElement];

	pNode->pNext = pHead;
	pHead = pNode; 
	return pNode;
}

void CNodeData::FreeDataChain()     // free this one and links
{
	CNodeData* pNode = this;
	while (pNode != NULL)
	{
		BYTE* pBytes = (BYTE*) pNode;
		CNodeData* pNext = pNode->pNext;
		delete[] pBytes;
		pNode = pNext;
	}
}