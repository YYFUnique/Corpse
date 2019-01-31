#pragma once

#include <richedit.h>
#include <richole.h>
#include <textserv.h>

#define		EN_PASTE											0x0800	// 粘贴消息
#define		WM_UPDATEPICTUREFRAME				(WM_USER+0x1000)		//RichEdit控件界面刷新

// {C5B3A481-21E3-4cab-8ABD-3C6A85892AD2}
_declspec(selectany) CLSID CLSID_ImageOle = { 0xc5b3a481, 0x21e3, 0x4cab, { 0x8a, 0xbd, 0x3c, 0x6a, 0x85, 0x89, 0x2a, 0xd2 } };

// {E41FE3BB-7218-423d-952E-96E556AEE30F}
_declspec(selectany) IID IID_IImageOle = { 0xe41fe3bb, 0x7218, 0x423d, { 0x95, 0x2e, 0x96, 0xe5, 0x56, 0xae, 0xe3, 0x0f } };

// {89FC1386-91E5-415f-AD8F-04415C15F8F5}
_declspec(selectany) IID IID_IRichEditOleCallbackEx = { 0x89fc1386, 0x91e5, 0x415f, { 0xad, 0x8f, 0x04, 0x41, 0x5c, 0x15, 0xf8, 0xf5 } };

typedef enum _tagFILE_FROM_TYPE
{
	FILE_FROM_SYSTEM = 0,	//文件来源于软件系统
	FILE_FROM_SELF,				//文件来源于
}FILE_FROM_TYPE;

struct NMRICHEDITOLECALLBACK
{
	NMHDR hdr;
	LPCTSTR lpszText;	// 粘贴文本
};

interface __declspec(uuid("E41FE3BB-7218-423d-952E-96E556AEE30F")) 
IImageOle : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE SetNotifyHwnd(HWND hWnd) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetRichEdit(CRichEditUI* pRichEdit) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetFaceId(LONG nFaceId) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFaceId(LONG* nFaceId) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFaceIndex(LONG nFaceIndex) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFaceIndex(LONG* nFaceIndex) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetFileName(BSTR* lpszFileName) = 0;
	virtual HRESULT STDMETHODCALLTYPE LoadFileFromPath(BSTR bstrFileName) = 0;
};

interface __declspec(uuid("89FC1386-91E5-415f-AD8F-04415C15F8F5")) 
IRichEditOleCallbackEx : public IRichEditOleCallback
{
	//STDMETHOD(SetNotifyHwnd)(HWND hWnd) PURE;	// 设置通知窗口句柄
	//STDMETHOD(SetRichEditHwnd)(HWND hWnd) PURE;	// 设置RichEdit窗口句柄
	//STDMETHOD(SetTextServices)(ITextServices *pTextServices) PURE;	// 设置ITextServices接口指针
	STDMETHOD(GetNewStorage(LPSTORAGE* lplpstg)) PURE;
	STDMETHOD(GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
								LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)) PURE;
	STDMETHOD(ShowContainerUI(BOOL fShow)) PURE;
	STDMETHOD(QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)) PURE;
	STDMETHOD(DeleteObject(LPOLEOBJECT lpoleobj)) PURE;
	STDMETHOD(QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
		DWORD reco, BOOL fReally, HGLOBAL hMetaPict)) PURE;
	STDMETHOD(ContextSensitiveHelp(BOOL fEnterMode)) PURE;
	STDMETHOD(GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)) PURE;
	STDMETHOD(GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)) PURE;
	STDMETHOD(GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
		HMENU FAR *lphmenu)) PURE;
};

interface __declspec(uuid("89FC1386-91E5-415f-AD8F-04415C15F8F5")) 
IRichEditOleCallback2 : public IRichEditOleCallback
{
	STDMETHOD(SetNotifyHwnd)(HWND hWnd) PURE;	// 设置通知窗口句柄
	STDMETHOD(SetRichEditHwnd)(HWND hWnd) PURE;	// 设置RichEdit窗口句柄
	STDMETHOD(SetTextServices)(ITextServices *pTextServices) PURE;	// 设置ITextServices接口指针
};
