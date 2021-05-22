#pragma once
#include "ILexer.h"
class Document;
class ILexInterface
{
public:
	ILexInterface(Document* pDoc);
	virtual ~ILexInterface();
public:
	void Colorise(int nStartPos, int nEndPos);
	BOOL UseContainerLexing() const;
protected:
	Document* m_pDoc;
	ILexer*			m_pInstance;
	BOOL			m_bPerformingStyle;
};