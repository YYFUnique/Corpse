// ImageOle.h : CImageOle ������
#pragma once
#include "resource.h"       // ������
#include <atlctl.h>
#include "Animator_i.h"
#include "_IImageOleEvents_CP.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif


// CImageOle
class ATL_NO_VTABLE CImageOle :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IImageOle, &IID_IImageOle, &LIBID_AnimatorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CImageOle>,
	public IOleControlImpl<CImageOle>,
	public IOleObjectImpl<CImageOle>,
	public IOleInPlaceActiveObjectImpl<CImageOle>,
	public IViewObjectExImpl<CImageOle>,
	public IOleInPlaceObjectWindowlessImpl<CImageOle>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CImageOle>,
	public CProxy_IImageOleEvents<CImageOle>,
	public IPersistStorageImpl<CImageOle>,
	public ISpecifyPropertyPagesImpl<CImageOle>,
	public IQuickActivateImpl<CImageOle>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CImageOle>,
#endif
	public IProvideClassInfo2Impl<&CLSID_ImageOle, &__uuidof(_IImageOleEvents), &LIBID_AnimatorLib>,
#ifdef _WIN32_WCE // Ҫ�� Windows CE ����ȷ���ظÿؼ���Ҫ�� IObjectSafety
	public IObjectSafetyImpl<CImageOle, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CImageOle, &CLSID_ImageOle>,
	public CComControl<CImageOle>
{
public:


	CImageOle()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGEOLE)


BEGIN_COM_MAP(CImageOle)
	COM_INTERFACE_ENTRY(IImageOle)
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
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // Ҫ�� Windows CE ����ȷ���ظÿؼ���Ҫ�� IObjectSafety
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CImageOle)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// ʾ����
	// PROP_ENTRY_TYPE("������", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CImageOle)
	CONNECTION_POINT_ENTRY(__uuidof(_IImageOleEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CImageOle)
	CHAIN_MSG_MAP(CComControl<CImageOle>)
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
			&IID_IImageOle,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)


	// ��С��֡��ʱ
#define MIN_FRM_DELAY				100

private:
	CDC *m_pDCs; // ÿ֡��DC
	CBitmap *m_pBitmaps; // ÿһ֡��λͼ
	CBitmap **m_ppOldBmps; // ����ÿһ֡DC�ɵ�λͼ
	int *m_pFrmIndexes; // ÿ��100�����Ӧ��֡����
	int m_nFrmCount; // ��֡��
	int m_nTimeCount; // 100���������������GIF����֡�������ܵ��ӳ���1�룬�����ֵΪ10
	volatile int m_iPrevFrm; // ǰһ֡����ʱ������ʱ�����ж��Ƿ���Ҫ�Ļ�֡
	volatile int m_iCurTime; // ��ǰʱ�䣨���ڼ���100���룩
	static DWORD m_dwOleObjectOffset; // IOleObject�ӿ�ָ�����COleImage��ʼ��ַ��ƫ����


public:

	// ʵ������һ���ᱻ���õĺ���
	HRESULT FinalConstruct();

	// ɾ��ʵ��ǰһ���ᱻ���õĺ���
	void FinalRelease();

	// ����IOleObject�ӿ�ָ����COleImage���е�ƫ����
	static DWORD GetIOleObjectOffset(void);

	// ����IOleObjec�ӿ�ָ��õ�COleImage��ָ�룬��ʵ���Ǽ�ȥǰ���ƫ����
	static CImageOle *FromOleObject(IOleObject *lpOleObject);

	// ���ļ����أ���Ҳ���������һ����IStream���صĺ���������ֻ����ʾ����
	HRESULT LoadFromFile(BSTR lpszPathName, CRichEditCtrl *pContainer, UINT nMaxWidth = 0);

	// ���ƺ���
	HRESULT OnDraw(ATL_DRAWINFO& di);

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

// IImageOle
public:
	//	HRESULT OnDraw(ATL_DRAWINFO& di)
		


	DECLARE_PROTECT_FINAL_CONSTRUCT()
};

OBJECT_ENTRY_AUTO(__uuidof(ImageOle), CImageOle)
