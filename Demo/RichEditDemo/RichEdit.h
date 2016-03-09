#pragma once

class CRichEdit : public WindowImplBase
{
public:
	CRichEdit();
	~CRichEdit();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void SetRichEditDefaultFont(const CFontInfo* pFontInfo,CRichEditUI* pRichEdit);
	void AddMsgToSendEdit(LPCTSTR lpText);
	void AddMsgToRecvEdit(LPCTSTR lpsRichEdit);
	void AddTipMsgToRecvEdit(LPCTSTR lpText);
	void AddMsg(CRichEditUI* pRichEdit, LPCTSTR lpText);
	void _RichEdit_ReplaceSel(CRichEditUI * pRichEdit, LPCTSTR lpszNewText);
	BOOL _RichEdit_InsertFace(CRichEditUI * pRichEdit, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex);
	BOOL HandleCustomPic(CRichEditUI * pRichEdit, LPCTSTR& pRichSendText, CDuiString& strRicvText);
protected:
	void OnClick(TNotifyUI& msg);
	void OnItemSelect(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnSelectFont(TNotifyUI& msg);
	void OnSelectFontSize(TNotifyUI& msg);
	void OnSelectBtnColor(TNotifyUI& msg);
	void OnSelectBtnFont(TNotifyUI& msg);
	void OnSelectBtnBold(TNotifyUI& msg);
	void OnSelectBtnItalic(TNotifyUI& msg);
	void OnSelectBtnUnderLine(TNotifyUI& msg);
	void	OnSelectBtnFace(TNotifyUI& msg);
	void	OnSelectBtnImage(TNotifyUI& msg);
	void	OnSelectBtnShock(TNotifyUI& msg);
	void	OnSelectBtnScreenShots(TNotifyUI& msg);
	void OnRichSendMessage();
	void OnBtnSendOption(TNotifyUI& msg);
	void OnMenuClick(CControlUI* pControl);
	void RestoreBtnStatus(CControlUI* pControl);
private:
	CRichEditUI* m_pSendRichEdit;
	CRichEditUI* m_pRecvRichEdit;
	BOOL			  m_Inited;
	CFontInfo      m_FontInfo;
	BOOL			  m_bReturnSendMsg;
// 	CRichEditOleCallback* m_pRichEditOleCallBackSendEdit;
// 	CRichEditOleCallback* m_pRichEditOleCallBackRecvEdit;
};