// RichEditImageOle.h : CRichEditImageOle ������
#pragma once
#include "resource.h"       // ������
#include <atlctl.h>
#include "ImageOle_i.h"
#include "_IRichEditImageOleEvents_CP.h"

//��С��֡��ʱ
#define MIN_FRM_DELAY				100

// CRichEditImageOle
class ATL_NO_VTABLE CRichEditImageOle :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IRichEditImageOle, &IID_IRichEditImageOle, &LIBID_ImageOleLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CRichEditImageOle>,
	public IOleControlImpl<CRichEditImageOle>,
	public IOleObjectImpl<CRichEditImageOle>,
	public IOleInPlaceActiveObjectImpl<CRichEditImageOle>,
	public IViewObjectExImpl<CRichEditImageOle>,
	public IOleInPlaceObjectWindowlessImpl<CRichEditImageOle>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CRichEditImageOle>,
	public CProxy_IRichEditImageOleEvents<CRichEditImageOle>,
	public IPersistStorageImpl<CRichEditImageOle>,
	public ISpecifyPropertyPagesImpl<CRichEditImageOle>,
	public IQuickActivateImpl<CRichEditImageOle>,
	public IDataObjectImpl<CRichEditImageOle>,
	public IProvideClassInfo2Impl<&CLSID_RichEditImageOle, &__uuidof(_IRichEditImageOleEvents), &LIBID_ImageOleLib>,
	public CComCoClass<CRichEditImageOle, &CLSID_RichEditImageOle>,
	public CComControl<CRichEditImageOle>
{
public:
	CRichEditImageOle()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_RICHEDITIMAGEOLE)


BEGIN_COM_MAP(CRichEditImageOle)
	COM_INTERFACE_ENTRY(IRichEditImageOle)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CRichEditImageOle)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// ʾ����
	// PROP_ENTRY_TYPE("������", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CRichEditImageOle)
	CONNECTION_POINT_ENTRY(__uuidof(_IRichEditImageOleEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CRichEditImageOle)
	CHAIN_MSG_MAP(CComControl<CRichEditImageOle>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// �������ԭ��:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IRichEditImageOle,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IRichEditImageOle
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);
		
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
	STDMETHOD(LoadFileFromPath)(BSTR lpszFilePath, UINT nMaxWidth);

private:
	HDC *m_pDCs; // ÿ֡��DC
	HBITMAP *m_pBitmaps; // ÿһ֡��λͼ
	HBITMAP **m_ppOldBmps; // ����ÿһ֡DC�ɵ�λͼ
	int *m_pFrmIndexes; // ÿ��100�����Ӧ��֡����
	int m_nFrmCount; // ��֡��
	int m_nTimeCount; // 100���������������GIF����֡�������ܵ��ӳ���1�룬�����ֵΪ10
	volatile int m_iPrevFrm; // ǰһ֡����ʱ������ʱ�����ж��Ƿ���Ҫ�Ļ�֡
	volatile int m_iCurTime; // ��ǰʱ�䣨���ڼ���100���룩
	static DWORD m_dwOleObjectOffset; // IOleObject�ӿ�ָ�����COleImage��ʼ��ַ��ƫ����
protected:
	// ����IOleObject�ӿ�ָ����COleImage���е�ƫ����
	static DWORD GetIOleObjectOffset(void);

	// ����IOleObjec�ӿ�ָ��õ�COleImage��ָ�룬��ʵ���Ǽ�ȥǰ���ƫ����
	static CRichEditImageOle* FromOleObject(IOleObject *lpOleObject);

	// ����֡
	void ChangeFrame();

	// �������һ��Ҫ��д�����Բ����κ����飬�����д��˫�������ͼ��ʱ��ASSERTʧ��
	STDMETHOD(DoVerb)(
		_In_ LONG /* iVerb */, 
		_In_opt_ LPMSG /* pMsg */, 
		_Inout_ IOleClientSite* /* pActiveSite */, 
		_In_ LONG /* lindex */,
		_In_ HWND /* hwndParent */, 
		_In_ LPCRECT /* lprcPosRect */)
	{
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(RichEditImageOle), CRichEditImageOle)
