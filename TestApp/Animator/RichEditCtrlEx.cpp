// RichEditCtrlEx.cpp : 实现文件
//

#include "stdafx.h"
#include "Animator.h"
#include "RichEditCtrlEx.h"
#include "ImageOle.h"
#include <richole.h>


// CRichEditCtrlEx

IMPLEMENT_DYNAMIC(CRichEditCtrlEx, CRichEditCtrl)

CRichEditCtrlEx::CRichEditCtrlEx()
{
	m_pRichEditOle = NULL;
	m_bCallbackSet = FALSE;
	m_pIRichEditOleCallback = NULL;
}

CRichEditCtrlEx::~CRichEditCtrlEx()
{
	if (m_pIRichEditOleCallback)
	{
		delete = m_pIRichEditOleCallback;
		m_pIRichEditOleCallback = NULL;
	}
}


BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CRichEditCtrl)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CRichEditCtrlEx 消息处理程序


//
// 在这里进行一些必要的修改
//


int CRichEditCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// m_pIRichEditOleCallback should have been created in PreSubclassWindow

	ASSERT( m_pIRichEditOleCallback != NULL );	

	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	if ( m_bCallbackSet == FALSE)
	{
		SetOLECallback( m_pIRichEditOleCallback );
	}

	return 0;
}


void CRichEditCtrlEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	// 下面这个可选，去掉RichEdit的双字体设置
	// 因为说实话，我不知道怎么分别设置中英文字体，如果哪位同学知道请告诉
	// 我一下
	DWORD fontStyle = SendMessage(EM_GETLANGOPTIONS);
	fontStyle &= ~IMF_DUALFONT;
	SendMessage(EM_SETLANGOPTIONS, 0, fontStyle);

	// 修改为背景透明，刷避免闪烁，我们自己绘制白色背景
	ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// 这个接口指针使用非常频繁，存下来提高效率
	m_pRichEditOle = GetIRichEditOle();

	// 启动定时器，用于更新动画
	SetTimer(FRM_TIMER_ID, MIN_FRM_DELAY, NULL);

	CRichEditCtrl::PreSubclassWindow();

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback;
	ASSERT( m_pIRichEditOleCallback != NULL );

	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}

//
// 窗口销毁时进行一些必要的清理工作
//
void CRichEditCtrlEx::OnDestroy()
{
	// 停止定时器，这是必要的
	KillTimer(FRM_TIMER_ID);
	// 清空内容，目的是删除所有插入的COleImage对象
	SetWindowText(_T(""));
	// 但是别忘了，RichEdit支持多级撤消，对象虽然删除了，但是撤消缓存
	// 中还保留有对象的引用，清空撤消缓存才能真正的释放这些对象
	EmptyUndoBuffer();

	// 上面的工作必须在父类OnDestroy前调用，否则窗口已经销毁，清理没用
	CRichEditCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

//
// 绘制背景
//
BOOL CRichEditCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}

//
// 查找第一个字符索引大于或等于cpMin的对象在所有对象中的索引
//
int CRichEditCtrlEx::FindFirstObject(int cpMin, int nObjectCount)
{
	// 标准的二分查找算法，不用解释了
	int low = 0;
	int high = nObjectCount - 1;
	REOBJECT reoMid = {0};
	reoMid.cbStruct = sizeof(REOBJECT);
	while (low <= high) {
		int mid = (low + high) >> 1;
		if (m_pRichEditOle->GetObject(mid, &reoMid, REO_GETOBJ_POLEOBJ) != S_OK) {
			return -1;
		}
		reoMid.poleobj->Release();
		if (reoMid.cp == cpMin) {
			return mid;
		} else if (reoMid.cp < cpMin) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}

	// 只不过最后没找到时不是返回-1，而是返回low，此时low必然大于或等于high
	// 刚好满足条件
	return low;
}

//
// 定时处理函数，更新动画
//
void CRichEditCtrlEx::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 定时器ID必须为我们所设置的定时器ID，不要以为调用SetTimer这里收到的就一定时我们
	// 设置的定时器，别忘了父类还可能设置，这一点不用怀疑，我测试到过！
	// 当写入很多行文本，滚动条出现后，再在最后插入动画，然后向上滚动到底部，再继续向下
	// 滚动，虽然滚动条不动了，但是动画却动得特别快，显然收到了父类的定时器
	// 因此这个定时器ID最好设置一个不常用的值，这里随便设置了一个，为1
	if (nIDEvent == FRM_TIMER_ID) {

		// 得到对象总数，大于0时才需要刷新
		int nObjectCount = m_pRichEditOle->GetObjectCount();
		if (nObjectCount > 0) {
			CRect rc;
			GetRect(rc); // 得到可输入的矩形区域
			// 分别使用左上角和右下角的点得到最小和最大字符索引
			// 即可见区域的最小和最大字符索引
			int cpMin = CharFromPos(rc.TopLeft());
			int cpMax = CharFromPos(rc.BottomRight());

			// 使用二分查找算法找到第一个字符索引大于或等于cpMin的对象索引
			int iFirst = FindFirstObject(cpMin, nObjectCount);
			REOBJECT reo = {0};
			reo.cbStruct = sizeof(REOBJECT);

			// 从第一个索引开始遍历对象更换帧
			for (int i = iFirst; i < nObjectCount; i++) {
				//将对象保存在数组中，能否提高工作效率
				//当前对象是否是Ole对象
				if (m_pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ) == S_OK) {
					reo.poleobj->Release();
					// 当前对象的字符索引大于最大字符索引，说明对象不在可见区域，停止遍历
					if (reo.cp > cpMax) {
						break;
					}

					// 是COleImage对象时才能更新
					if (InlineIsEqualGUID(reo.clsid, CLSID_ImageOle)) {
						// 更换帧
						CImageOle *pImageOle = CImageOle::FromOleObject(reo.poleobj);
						pImageOle->ChangeFrame();
					}
				}
			}
		}
	} else {
		CRichEditCtrl::OnTimer(nIDEvent);
	}
}

//
// 插入图像
//
HRESULT CRichEditCtrlEx::InsertImage(LPCTSTR lpszPathName)
{
	// 全部使用智能指针
	CComPtr<IStorage> spStorage;
	CComPtr<ILockBytes> spLockBytes;
	CComPtr<IOleClientSite> spOleClientSite;	
	CComPtr<CImageOle> spImageOle;
	CComPtr<IOleObject> spOleObject;
	CLSID clsid;
	REOBJECT reobject;
	HRESULT hr = E_FAIL;

	do {

		// 创建LockBytes
		hr = CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (hr != S_OK) {
			break;
		}

		ASSERT(spLockBytes != NULL);

		// 创建Storage
		hr = StgCreateDocfileOnILockBytes(spLockBytes,
			STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &spStorage);
		if (hr != S_OK) {
			break;
		}

		// 获取ClientSite
		hr = m_pRichEditOle->GetClientSite(&spOleClientSite);
		if (hr != S_OK) {
			break;
		}

		// 创建COleImage实例
		hr = CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC, IID_IImageOle, (LPVOID*) &spImageOle);
		if (hr != S_OK) {
			break;
		}

		// 加载图像
		hr = spImageOle->LoadFromFile(_bstr_t(lpszPathName), this, 400);
		if (hr != S_OK) {
			break;
		}

		// 获取IOleObject接口
		hr = spImageOle->QueryInterface(IID_IOleObject, (LPVOID *) &spOleObject);
		if (hr != S_OK) {
			break;
		}

		// 获取IOleObject的用户CLSID
		hr = spImageOle->GetUserClassID(&clsid);
		if (hr != S_OK) {
			break;
		}

		// 填充OLE对象属性
		ZeroMemory(&reobject, sizeof(REOBJECT));		
		reobject.cbStruct	= sizeof(REOBJECT);
		reobject.clsid		= clsid;
		reobject.cp			= REO_CP_SELECTION;
		reobject.dvaspect	= DVASPECT_CONTENT;
		reobject.dwFlags	= REO_BELOWBASELINE;
		reobject.poleobj	= spImageOle;
		reobject.polesite	= spOleClientSite;
		reobject.pstg		= spStorage;
		SIZEL sizel = {0};
		reobject.sizel = sizel;

		// 插入OLE对象
		hr = m_pRichEditOle->InsertObject(&reobject);
		if (hr != S_OK) {
			break;
		}

		// 通知OLE容器保证OLE对象被正确引用
		hr = OleSetContainedObject(spOleObject, TRUE);

	} while (0);

	return hr;
}


long CRichEditCtrlEx::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);

	DWORD len = SizeofResource(hInst, hRsrc); 
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
	ASSERT(lpRsrc); 

	CMemFile mfile;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &mfile;

	return StreamIn( SF_RTF, es );
}


/* static */
DWORD CALLBACK CRichEditCtrlEx::readFunction(DWORD dwCookie,
											  LPBYTE lpBuf,			// the buffer to fill
											  LONG nCount,			// number of bytes to read
											  LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

CRichEditCtrlEx::IExRichEditOleCallback::IExRichEditOleCallback()
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &pStorage );

	if ( pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

CRichEditCtrlEx::IExRichEditOleCallback::~IExRichEditOleCallback()
{
}

HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
															LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
														  DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
CRichEditCtrlEx::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
														 HMENU FAR *lphmenu)
{
	return S_OK;
}






