#include "StdAfx.h"
#include "UIDataBuilder.h"

namespace DuiLib
{
	////////////////////////////////////////////////////////////
	////CControlData
	////////////////////////////////////////////////////////////
	CControlData::CControlData()
	{

	}

	CControlData::~CControlData()
	{

	}

	void CControlData::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("id")) == 0) SetTextId(pstrValue);
	}

	CDuiString CControlData::GetTextId()
	{
		return m_strTextId;
	}

	void CControlData::SetTextId(LPCTSTR pstrTextId)
	{
		if (m_strTextId == pstrTextId)	return;

		m_strTextId = pstrTextId;
	}

	////////////////////////////////////////////////////////////
	////CLanData
	////////////////////////////////////////////////////////////
	CLanData::CLanData()
	{

	}

	void CLanData::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("text")) == 0)	 SetText(pstrValue);
		else if (_tcsicmp(pstrName, _T("tooltip")) == 0)	SetTooltip(pstrValue);
		else
			CControlData::SetAttribute(pstrName, pstrValue);
	}

	CDuiString CLanData::GetText()
	{
		return m_strText;
	}

	void CLanData::SetText(LPCTSTR pstrText)
	{
		if (m_strText == pstrText)	return;

		m_strText = pstrText;
	}

	CDuiString CLanData::GetTooltip()
	{
		return m_strTooltip;
	}
	
	void CLanData::SetTooltip(LPCTSTR pstrTooltip)
	{
		if (m_strTooltip == pstrTooltip)		return;

		m_strTooltip = pstrTooltip;
	}

	////////////////////////////////////////////////////////////
	////CDataBuilderUI
	////////////////////////////////////////////////////////////
	CDataBuilderUI* CDataBuilderUI::GetInstance()
	{
		static CDataBuilderUI sResBuilderMgr;
		return &sResBuilderMgr;
	}

	CDataBuilderUI::CDataBuilderUI()
	{

	}

	CDataBuilderUI::~CDataBuilderUI()
	{

	}

	BOOL CDataBuilderUI::LoadLanguage(LPCTSTR pstrXml)
	{
		if (pstrXml == NULL || _tcslen(pstrXml) == 0)
			return FALSE;

		CMarkup xml;
		if( *(pstrXml) == _T('<') ) {
			if( !xml.Load(pstrXml) ) return FALSE;
		}
		else {
			if( !xml.LoadFromFile(pstrXml) ) return FALSE;
		}
		CMarkupNode Root = xml.GetRoot();
		if( !Root.IsValid() ) return FALSE;

		ResetTextMap();
		_Parse(&Root, NULL, NULL);

		return TRUE;
	}

	void CDataBuilderUI::_Parse(CMarkupNode* pRoot, CControlData* pParent, CPaintManagerUI* pManager)
	{
		for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {

			CControlData* pData = NULL;
			LPCTSTR pstrClass = node.GetName();
			SIZE_T cchLen = _tcslen(pstrClass);
			switch (cchLen)
			{
				case 3:
						if (_tcsicmp(pstrClass, _T("Lan")) == 0) pData = new CLanData;
					break;
			}

			if (pData == NULL)
				continue;

			if( node.HasAttributes() ) {
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					pData->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}

			m_mTextResourceHashMap.Insert(pData->GetTextId(),pData);
		}
	}

	CControlData* CDataBuilderUI::GetControlData(LPCTSTR lpstrId)
	{
		if (lpstrId == NULL || _tcslen(lpstrId) == 0) return NULL;

		return (CControlData*)m_mTextResourceHashMap.Find(lpstrId);
	}

	void CDataBuilderUI::ReloadText()
	{
		/*if(m_pQuerypInterface == NULL) return;
		LPCTSTR lpstrId = NULL;
		LPCTSTR lpstrText;
		for( int i = 0; i < m_mTextResourceHashMap.GetSize(); i++ )
		{
			lpstrId = m_mTextResourceHashMap.GetAt(i);
			if (lpstrId == NULL) continue;
			lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, NULL);
			if(lpstrText != NULL) {
				CDuiString * lpStr = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(lpstrId));
				lpStr->Assign(lpstrText);
			}
		}*/
	}

	void CDataBuilderUI::ResetTextMap()
	{
		for( int i = 0; i< m_mTextResourceHashMap.GetSize(); i++ )
		{
			LPCTSTR lpszKey = m_mTextResourceHashMap.GetAt(i);
			if (lpszKey != NULL)
			{
				CControlData* pData = static_cast<CControlData*>(m_mTextResourceHashMap.Find(lpszKey));
				delete pData;
				pData = NULL;
			}
		}
		m_mTextResourceHashMap.RemoveAll();
	}
}