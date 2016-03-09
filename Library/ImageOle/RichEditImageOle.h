// RichEditImageOle.h : CRichEditImageOle 的声明
#pragma once
#include "resource.h"       // 主符号
#include <atlctl.h>
#include "ImageOle_i.h"
#include "_IRichEditImageOleEvents_CP.h"

//最小的帧延时
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
	// 示例项
	// PROP_ENTRY_TYPE("属性名", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CRichEditImageOle)
	CONNECTION_POINT_ENTRY(__uuidof(_IRichEditImageOleEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CRichEditImageOle)
	CHAIN_MSG_MAP(CComControl<CRichEditImageOle>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// 处理程序原型:
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
	HDC *m_pDCs; // 每帧的DC
	HBITMAP *m_pBitmaps; // 每一帧的位图
	HBITMAP **m_ppOldBmps; // 保存每一帧DC旧的位图
	int *m_pFrmIndexes; // 每个100毫秒对应的帧索引
	int m_nFrmCount; // 总帧数
	int m_nTimeCount; // 100毫秒的总数，例如GIF所有帧加起来总的延迟是1秒，则这个值为10
	volatile int m_iPrevFrm; // 前一帧，定时器触发时用于判断是否需要改换帧
	volatile int m_iCurTime; // 当前时间（即第几个100毫秒）
	static DWORD m_dwOleObjectOffset; // IOleObject接口指针距离COleImage起始地址的偏移量
protected:
	// 计算IOleObject接口指针在COleImage类中的偏移量
	static DWORD GetIOleObjectOffset(void);

	// 根据IOleObjec接口指针得到COleImage的指针，其实就是减去前面的偏移量
	static CRichEditImageOle* FromOleObject(IOleObject *lpOleObject);

	// 更换帧
	void ChangeFrame();

	// 这个函数一定要重写，可以不做任何事情，如果不写，双击插入的图像时会ASSERT失败
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
