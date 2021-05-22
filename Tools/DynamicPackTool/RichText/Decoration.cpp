#include "StdAfx.h"
#include "Decoration.h"


Decoration::Decoration(int nIndicator)
	: m_nIndicator(nIndicator)
	, m_pNext(NULL)
{

}

Decoration::~Decoration()
{

}

void Decoration::SetNext(Decoration* pNext)
{
	m_pNext = pNext;
}

Decoration* Decoration::GetNext() const
{
	return m_pNext;
}

RunStyles Decoration::GetRunStyles() const
{
	return m_rs;
}

int Decoration::GetIndicator() const
{
	return m_nIndicator;
}

BOOL Decoration::IsEmpty() const
{
	return (m_rs.GetRuns() == 1) && (m_rs.IsAllSameAs(0));
}

int Decoration::ValueAt(int nPos)
{
	return m_rs.ValueAt(nPos);
}

int Decoration::StartRun(int nPos)
{
	return m_rs.StartRun(nPos);
}

int Decoration::EndRun(int nPos)
{
	return m_rs.EndRun(nPos);
}

BOOL Decoration::FillRange(int nPos, int nVal, int& nLen)
{
	return m_rs.FillRange(nPos, nVal, nLen);
}

void Decoration::DeleteRange(int nPos, int nLen)
{
	m_rs.DeleteRange(nPos, nLen);
}

void Decoration::InsertSpace(int nPos, int nLen)
{
	m_rs.InsertSpace(nPos, nLen);
}


/////////////////////////////////////////////////////////////////////////
DecorationList::DecorationList()
	: m_nCurrentIndicator(0)
	, m_nCurrentValue(1)
	, m_pCurrent(NULL)
	, m_DocumentLen(0)
	, m_pRoot(NULL)
	, m_bClickNotified(FALSE)
{

}

DecorationList::~DecorationList()
{
	Decoration* pDeco = m_pRoot;
	while(pDeco) {
		Decoration* pDecoNext = pDeco->GetNext();
		delete pDeco;
		pDeco = pDecoNext;
	}

	m_pRoot = NULL;
	m_pCurrent = NULL;
}

Decoration* DecorationList::DecorationFromIndicator(int nIndicator) const
{
	for (Decoration* pDeco = m_pRoot; pDeco; pDeco = pDeco->GetNext()) {
		if (pDeco->GetIndicator() == nIndicator)
			return pDeco;
	}

	return NULL;
}

Decoration* DecorationList::Create(int nIndicator, int nLen)
{
	m_nCurrentIndicator = nIndicator;
	Decoration* pDecoNew = new Decoration(nIndicator);
	pDecoNew->InsertSpace(0, nLen);

	Decoration* pDecoPrev = NULL;
	Decoration* pDeco = m_pRoot;
	while(pDeco && (pDeco->GetIndicator() < nIndicator)) {
		pDecoPrev = pDeco;
		pDeco = pDeco->GetNext();
	}

	if (pDecoPrev == NULL) {
		pDecoNew->SetNext(m_pRoot);
		m_pRoot = pDecoNew;
	} else {
		pDecoNew->SetNext(pDeco);
		pDecoPrev->SetNext(pDecoNew);
	}

	return pDecoNew;
}

void DecorationList::Delete(int nIndicator)
{
	Decoration* pDecoDelete = NULL;
	if (m_pRoot) { 
		if (m_pRoot->GetIndicator() == nIndicator) {
			pDecoDelete = m_pRoot;
			m_pRoot = m_pRoot->GetNext();
		} else {
			Decoration* pDeco = m_pRoot;
			while(pDeco->GetNext() && !pDecoDelete) {
				if (pDeco->GetNext() && pDeco->GetNext()->GetIndicator() == nIndicator) {
					pDecoDelete = pDeco->GetNext();
					pDeco->SetNext(pDecoDelete->GetNext());
				} else
					pDeco = pDeco->GetNext();
			}
		}
		if (pDecoDelete) {
			delete pDecoDelete;
			m_pCurrent = NULL;
		}
	}
}

void DecorationList::SetCurrentIndicator(int nIndicator)
{
	m_nCurrentIndicator = nIndicator;
	m_pCurrent = DecorationFromIndicator(nIndicator);
	m_nCurrentValue = 1;
}

void DecorationList::SetCurrentValue(int nVal)
{
	m_nCurrentValue = nVal ? nVal : 1;
}

BOOL DecorationList::FillRange(int &nPos, int nVal, int &nFillLen)
{
	if (m_pCurrent == NULL) {
		m_pCurrent = DecorationFromIndicator(m_nCurrentValue);
		if (m_pCurrent == NULL) 
			m_pCurrent = Create(m_nCurrentValue, m_DocumentLen);
	}

	BOOL bChanged = m_pCurrent->FillRange(nPos, nVal, nFillLen);
	if (m_pCurrent->IsEmpty())
		Delete(m_nCurrentValue);

	return bChanged;
}

void DecorationList::InsertSpace(int nPos, int nLen)
{
	const BOOL bEnd = nPos == m_DocumentLen;
	m_DocumentLen += nLen;

	for (Decoration* pDeco = m_pRoot; pDeco; pDeco = pDeco->GetNext()) {
		pDeco->InsertSpace(nPos, nLen);
		if (bEnd)
			pDeco->FillRange(nPos, 0, nLen);
	}
}

void DecorationList::DeleteRange(int nPos, int nLen)
{
	m_DocumentLen -= nLen;
	Decoration* pDeco = m_pRoot;
	while(pDeco) {
		pDeco->DeleteRange(nPos, nLen);
		pDeco = pDeco->GetNext();
	}

	DeleteAnyEmpty();
}

void DecorationList::DeleteAnyEmpty()
{
	Decoration* pDeco = m_pRoot;
	while(pDeco) {
		if (m_DocumentLen == 0 || pDeco->IsEmpty()) {
			Delete(pDeco->GetIndicator());
			pDeco = m_pRoot;
		} else {
			pDeco = pDeco->GetNext();
		}
	}
}

int DecorationList::AllOnFor(int nPos)
{
	int nMask = 0;
	Decoration* pDeco = m_pRoot;
	while(pDeco) {
		if (pDeco->ValueAt(nPos))
			nMask |= 1<< pDeco->GetIndicator();
	}

	return nMask;
}

int DecorationList::ValueAt(int nIndicator, int nPos)
{
	Decoration* pDeco = DecorationFromIndicator(nIndicator);
	if (pDeco)
		return pDeco->ValueAt(nPos);

	return 0;
}

int DecorationList::Start(int nIndicator, int nPos)
{
	Decoration* pDeco = DecorationFromIndicator(nIndicator);
	if (pDeco)
		return pDeco->StartRun(nPos);
	return 0;
}

int DecorationList::End(int nIndicator, int nPos)
{
	Decoration* pDeco = DecorationFromIndicator(nIndicator);
	if (pDeco)
		pDeco->EndRun(nPos);

	return 0;
}