#include "StdAfx.h"
#include "ILexInterface.h"

#include "Document.h"

ILexInterface::ILexInterface(Document* pDoc)
	: m_pDoc(pDoc)
	, m_pInstance(0)
	, m_bPerformingStyle(FALSE)
{

}

ILexInterface::~ILexInterface()
{

}

void ILexInterface::Colorise(int nStartPos, int nEndPos)
{
	if (m_pDoc && m_pInstance && m_bPerformingStyle == FALSE) {
		// Protect against reentrance, which may occur, for example, when
		// fold points are discovered while performing styling and the folding
		// code looks for child lines which may trigger styling.
		m_bPerformingStyle = TRUE;

		int nDocLen = m_pDoc->GetLength();
		if (nEndPos == -1)
			nEndPos = nDocLen;
		int nLen = nEndPos - nStartPos;

		int nStyleStart = 0;
		if (nStartPos > 0)
			nStyleStart = m_pDoc->StyleAt(nStartPos - 1) & m_pDoc->m_nStylingBitsMask;

		if (nLen > 0) {
			m_pInstance->Lex(nStartPos, nLen, nStyleStart, m_pDoc);
			m_pInstance->Fold(nStartPos, nLen, nStyleStart, m_pDoc);
		}

		m_bPerformingStyle = FALSE;
	}
}
	
BOOL ILexInterface::UseContainerLexing() const
{
	return m_pInstance == NULL;
}