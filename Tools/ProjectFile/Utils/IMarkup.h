#pragma once

class IMarkup
{
public:
	IMarkup(){m_pNext = NULL;};
	virtual ~IMarkup(){};
public:
	virtual LPVOID GetInterface(LPCTSTR pstrName) = 0;
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) = 0;
	void SetNext(IMarkup* pNext) {m_pNext = pNext;}
	IMarkup* GetNext() const {return m_pNext;}
protected:
	IMarkup* m_pNext;
};