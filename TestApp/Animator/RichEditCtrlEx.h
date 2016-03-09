#pragma once
#include <richole.h>

// CRichEditCtrlEx

#define FRM_TIMER_ID			1

class CRichEditCtrlEx : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CRichEditCtrlEx)

public:
	CRichEditCtrlEx();
	virtual ~CRichEditCtrlEx();
private:
	CComPtr<IRichEditOle> m_pRichEditOle;
protected:
	static DWORD CALLBACK readFunction(DWORD dwCookie,
		LPBYTE lpBuf,			// the buffer to fill
		LONG nCount,			// number of bytes to read
		LONG* nRead);			// number of bytes actually read
	interface IExRichEditOleCallback;	// forward declaration (see below in this header file)

	IExRichEditOleCallback* m_pIRichEditOleCallback;
	BOOL m_bCallbackSet;
	long StreamInFromResource(int iRes, LPCTSTR sType);
	DECLARE_MESSAGE_MAP()
	BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	int FindFirstObject(int cpMin, int nObjectCount);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	HRESULT InsertImage(LPCTSTR lpszPathName);

protected:
	interface IExRichEditOleCallback : public IRichEditOleCallback
	{
	public:
		IExRichEditOleCallback();
		virtual ~IExRichEditOleCallback();
		int m_iNumStorages;
		IStorage* pStorage;
		DWORD m_dwRef;

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
			HMENU FAR *lphmenu);
	};
};


