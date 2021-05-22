#pragma once

#include "RESearch.h"
// Define a way for the Regular Expression code to access the document
class DocumentIndexer : public ICharacterIndexer 
{
protected:
	Document *m_pDoc;
	int	m_nEnd;
public:
	DocumentIndexer(Document *pDoc, int nEnd)
		: m_pDoc(pDoc), m_nEnd(nEnd)
	{
	}

	  virtual ~DocumentIndexer() 
	  {
	  }

	  virtual CHAR CharAt(int nIndex) 
	  {
		  if (nIndex < 0 || nIndex >= m_nEnd)
			  return 0;
		  else
			  return m_pDoc->CharAt(nIndex);
	  }
};