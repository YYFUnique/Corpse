#pragma once
#include <oleidl.h>
#include <shobjidl.h>
#include <ShlGuid.h>
#include <Shlwapi.h>
#include <vector>

typedef struct tagDRAGDATA
{
	CLIPFORMAT cfFormat;
	STGMEDIUM stgMedium;
}DRAGDATA,*LPDRAGDATA;

typedef struct tagDATASTORAGE
{  
	FORMATETC* pFMatec;
	STGMEDIUM* pMedium;		//��קý������
}DATASTORAGE,*LPDATASTORAGE;

namespace DuiLib
{
	typedef std::vector<FORMATETC> FormatEtcArray;
	typedef std::vector<FORMATETC*> PFormatEtcArray;
	typedef std::vector<STGMEDIUM*> PStgMediumArray;

	#pragma warning( disable: 4251 )
	class IDropTargetCallBack
	{
	public:
		virtual HRESULT OnDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl,  DWORD *pdwEffect) = 0;
		virtual HRESULT  OnDragOver(DWORD grfKeyState, POINTL pt,DWORD *pdwEffect) = 0;
		virtual HRESULT  OnDragLeave() = 0;
		virtual HRESULT  OnDrop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) = 0;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class UILIB_API CUIDropSource : public IDropSource
	{
	public:
		CUIDropSource();
		~CUIDropSource();

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
		ULONG STDMETHODCALLTYPE AddRef();
		ULONG STDMETHODCALLTYPE Release();

		virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);

		virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

		static HRESULT CreateDropSource(IDropSource **ppDropSource);
	protected:
	
	private:
		volatile LONG   m_lRefCount;
	};//class CUIDropSource

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class UILIB_API CUIDropTarget : public IDropTarget
	{
	public:
		CUIDropTarget(void);

		//����������ק����
		BOOL DragDropRegister(HWND hWnd, DWORD AcceptKeyState=MK_LBUTTON, IDropTargetCallBack* pUIDropTarget = NULL);
		//�رմ�����ק����
		BOOL DragDropRevoke(HWND hWnd);

		//�����Ƿ���ϵͳ������ק����
		BOOL SetDropTargetHelper(BOOL bEnable);

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
		ULONG STDMETHODCALLTYPE AddRef();
		ULONG STDMETHODCALLTYPE Release();
		//����
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		//�ƶ�
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		//�뿪
		HRESULT STDMETHODCALLTYPE DragLeave();
		//�ͷ�
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	protected:
		~CUIDropTarget(void);

		BOOL GetDragData(IDataObject *pDataObject,FORMATETC cFmt);
		BOOL EnumDragData(IDataObject *pDataObject);

	private:
		HWND m_hWnd;
		volatile LONG  m_lRefCount;
		DWORD m_dAcceptKeyState;

		CStdPtrArray m_DragData;
		//�Ƿ���ϵͳ����������
		BOOL	 m_bDropTargetHelper;
		IDropTargetCallBack* m_pUIDropTarget;
		//�����ʹ�øýӿ��Ϸ�ʱ��ͼ�껹������һ���Ӻţ���������ϵͳ����������ʾ��Ӧ���ļ���ͼ��
		IDropTargetHelper* m_pDropHelper;
	};//class CUIDropTarget

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class UILIB_API CUIDataObject : public IDataObject
	{
		public:
			CUIDataObject();  
			
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject); 
			ULONG   STDMETHODCALLTYPE AddRef(void); 
			ULONG   STDMETHODCALLTYPE Release(void); 

			HRESULT STDMETHODCALLTYPE GetData               (FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);  
			HRESULT STDMETHODCALLTYPE GetDataHere           (FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);  
			HRESULT STDMETHODCALLTYPE QueryGetData          (FORMATETC *pFormatEtc);  
			HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc (FORMATETC *pFormatEct,  FORMATETC *pFormatEtcOut);  
			HRESULT STDMETHODCALLTYPE SetData               (FORMATETC *pFormatEtc,  STGMEDIUM *pMedium,  BOOL fRelease);  
			HRESULT STDMETHODCALLTYPE EnumFormatEtc         (DWORD      dwDirection, IEnumFORMATETC **ppEnumFormatEtc);  
			HRESULT STDMETHODCALLTYPE DAdvise               (FORMATETC *pFormatEtc,  DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);  
			HRESULT STDMETHODCALLTYPE DUnadvise             (DWORD      dwConnection);  
			HRESULT STDMETHODCALLTYPE EnumDAdvise           (IEnumSTATDATA **ppEnumAdvise);  

			static HRESULT CreateDataObject(IDataObject **ppDataObject);
		protected:
			HRESULT CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);

			~CUIDataObject(); 
		private:
			LONG       m_lRefCount;  

			PFormatEtcArray m_ArrFormatEtc;
			PStgMediumArray m_StgMedium;
	};//class CUIDataObject

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class UILIB_API CUIDragSourceHelper
	{
	public:
		CUIDragSourceHelper(IDataObject* pDataObject);
		virtual ~CUIDragSourceHelper();

		// IDragSourceHelper
		HRESULT InitializeFromBitmap(HBITMAP hBitmap, POINT& pt, RECT& rc, COLORREF crColorKey=GetSysColor(COLOR_WINDOW));
		HRESULT InitializeFromWindow(HWND hwnd, POINT& pt);

		void Release();
		BOOL SetObject(IDataObject* pDataObject);
	private:
		IDragSourceHelper* m_pDragSourceHelper;
		IDataObject*			 m_pDataObject;
	};

	class UILIB_API CUIEnumFormatEtc : public IEnumFORMATETC
	{
		public:
			CUIEnumFormatEtc(const FormatEtcArray& ArrFE);
			CUIEnumFormatEtc(const PFormatEtcArray& ArrFE);
			//IUnknown members
			STDMETHOD(QueryInterface)(REFIID, void FAR* FAR*);
			STDMETHOD_(ULONG, AddRef)(void);
			STDMETHOD_(ULONG, Release)(void);

			//IEnumFORMATETC members
			STDMETHOD(Next)(ULONG, LPFORMATETC, ULONG FAR *);
			STDMETHOD(Skip)(ULONG);
			STDMETHOD(Reset)(void);
			STDMETHOD(Clone)(IEnumFORMATETC FAR * FAR*);

	private:
		ULONG           m_cRefCount;
		FormatEtcArray  m_pFmtEtc;
		size_t           m_iCur;
	}; //class CUIEnumFormatEtc

}//namespace DuiLib
