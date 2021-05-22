#pragma once

#include "CellBuffer.h"
#include "Document.h"
/**
 * To optimise processing of document modifications by DocWatchers, a hint is passed indicating the
 * scope of the change.
 * If the DocWatcher is a document view then this can be used to optimise screen updating.
 */

class DocModification
{
public:
	DocModification(int nModificationType, int nPos = 0, int nLen = 0, int nLineAdd = 0, const CHAR* chData = NULL, int nLine = 0);
	DocModification(int nModificationType, const Action& act, int nLineAdd = 0);
public:
	int GetModificationType() const;
	int GetPos() const;
	int GetLength() const;
	int GetLine() const;
	int GetAddLines() const;
	const CHAR* GetData() const;

	void SetLine(int nLine);
	void SetToken(int nToken);
	void SetFoldLevelNow(int nFoldLevelNow);
	void SetFoldLevelPrev(int nFoldLevelPrev);
	void SetAnnotationLinesAdd(int nAnnotationLinesAdd);
protected:
	int		m_nModificationType;
	int		m_nPos;
	int		m_nLen;
	int		m_nLineAdd;	/**< Negative if lines deleted. */
	const CHAR* m_chData;	/**< Only valid for changes to text, not for changes to style. */
	int		m_nLine;
	int		m_nFoldLevelNow;
	int		m_nFoldLevelPrev;
	int		m_nAnnotationLinesAdd;
	int		m_nToken;
};