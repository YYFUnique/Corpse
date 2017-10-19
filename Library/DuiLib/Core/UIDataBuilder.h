#pragma once

namespace DuiLib 
{
	// �ؼ����ֲ�ѯ�ӿ�
	class UILIB_API IQueryControlText
	{
	public:
		virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
	};

	class CControlData
	{
	public:
		CControlData();
		virtual ~CControlData();
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CDuiString GetTextId();
		void SetTextId(LPCTSTR pstrTextId);
	private:
		CDuiString m_strTextId;
	};

	class CLanData : public CControlData
	{
	public:
		CLanData();

	public:
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CDuiString GetText();
		void SetText(LPCTSTR pstrText);
		CDuiString GetTooltip();
		void SetTooltip(LPCTSTR pstrTooltip);
	private:
		CDuiString m_strText;
		CDuiString m_strTooltip;
		CDuiString m_strData;
	};

	class UILIB_API CDataBuilderUI
	{
	public:
		static CDataBuilderUI* GetInstance();

	public:
		void ReloadText();
		void ResetTextMap();
		BOOL LoadLanguage(LPCTSTR lpszLanguage);
		CControlData* GetControlData(LPCTSTR lpstrId);

	private:
		CDataBuilderUI();
		~CDataBuilderUI();

		void _Parse(CMarkupNode* parent, CControlData* pParent = NULL, CPaintManagerUI* pManager = NULL);
	protected:
		CDuiString m_sLauguage;
		CStdStringPtrMap m_mTextHashMap;
		IQueryControlText*	m_pQuerypInterface;
		CStdStringPtrMap m_mTextResourceHashMap;
	};
}