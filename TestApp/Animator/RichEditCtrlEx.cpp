// RichEditCtrlEx.cpp : ʵ���ļ�
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



// CRichEditCtrlEx ��Ϣ�������


//
// ���������һЩ��Ҫ���޸�
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
	// TODO: �ڴ����ר�ô����/����û���

	// ���������ѡ��ȥ��RichEdit��˫��������
	// ��Ϊ˵ʵ�����Ҳ�֪����ô�ֱ�������Ӣ�����壬�����λͬѧ֪�������
	// ��һ��
	DWORD fontStyle = SendMessage(EM_GETLANGOPTIONS);
	fontStyle &= ~IMF_DUALFONT;
	SendMessage(EM_SETLANGOPTIONS, 0, fontStyle);

	// �޸�Ϊ����͸����ˢ������˸�������Լ����ư�ɫ����
	ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// ����ӿ�ָ��ʹ�÷ǳ�Ƶ�������������Ч��
	m_pRichEditOle = GetIRichEditOle();

	// ������ʱ�������ڸ��¶���
	SetTimer(FRM_TIMER_ID, MIN_FRM_DELAY, NULL);

	CRichEditCtrl::PreSubclassWindow();

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback;
	ASSERT( m_pIRichEditOleCallback != NULL );

	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}

//
// ��������ʱ����һЩ��Ҫ��������
//
void CRichEditCtrlEx::OnDestroy()
{
	// ֹͣ��ʱ�������Ǳ�Ҫ��
	KillTimer(FRM_TIMER_ID);
	// ������ݣ�Ŀ����ɾ�����в����COleImage����
	SetWindowText(_T(""));
	// ���Ǳ����ˣ�RichEdit֧�ֶ༶������������Ȼɾ���ˣ����ǳ�������
	// �л������ж�������ã���ճ�����������������ͷ���Щ����
	EmptyUndoBuffer();

	// ����Ĺ��������ڸ���OnDestroyǰ���ã����򴰿��Ѿ����٣�����û��
	CRichEditCtrl::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
}

//
// ���Ʊ���
//
BOOL CRichEditCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}

//
// ���ҵ�һ���ַ��������ڻ����cpMin�Ķ��������ж����е�����
//
int CRichEditCtrlEx::FindFirstObject(int cpMin, int nObjectCount)
{
	// ��׼�Ķ��ֲ����㷨�����ý�����
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

	// ֻ�������û�ҵ�ʱ���Ƿ���-1�����Ƿ���low����ʱlow��Ȼ���ڻ����high
	// �պ���������
	return low;
}

//
// ��ʱ�����������¶���
//
void CRichEditCtrlEx::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	// ��ʱ��ID����Ϊ���������õĶ�ʱ��ID����Ҫ��Ϊ����SetTimer�����յ��ľ�һ��ʱ����
	// ���õĶ�ʱ���������˸��໹�������ã���һ�㲻�û��ɣ��Ҳ��Ե�����
	// ��д��ܶ����ı������������ֺ����������붯����Ȼ�����Ϲ������ײ����ټ�������
	// ��������Ȼ�����������ˣ����Ƕ���ȴ�����ر�죬��Ȼ�յ��˸���Ķ�ʱ��
	// ��������ʱ��ID�������һ�������õ�ֵ���������������һ����Ϊ1
	if (nIDEvent == FRM_TIMER_ID) {

		// �õ���������������0ʱ����Ҫˢ��
		int nObjectCount = m_pRichEditOle->GetObjectCount();
		if (nObjectCount > 0) {
			CRect rc;
			GetRect(rc); // �õ�������ľ�������
			// �ֱ�ʹ�����ϽǺ����½ǵĵ�õ���С������ַ�����
			// ���ɼ��������С������ַ�����
			int cpMin = CharFromPos(rc.TopLeft());
			int cpMax = CharFromPos(rc.BottomRight());

			// ʹ�ö��ֲ����㷨�ҵ���һ���ַ��������ڻ����cpMin�Ķ�������
			int iFirst = FindFirstObject(cpMin, nObjectCount);
			REOBJECT reo = {0};
			reo.cbStruct = sizeof(REOBJECT);

			// �ӵ�һ��������ʼ�����������֡
			for (int i = iFirst; i < nObjectCount; i++) {
				//�����󱣴��������У��ܷ���߹���Ч��
				//��ǰ�����Ƿ���Ole����
				if (m_pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ) == S_OK) {
					reo.poleobj->Release();
					// ��ǰ������ַ�������������ַ�������˵�������ڿɼ�����ֹͣ����
					if (reo.cp > cpMax) {
						break;
					}

					// ��COleImage����ʱ���ܸ���
					if (InlineIsEqualGUID(reo.clsid, CLSID_ImageOle)) {
						// ����֡
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
// ����ͼ��
//
HRESULT CRichEditCtrlEx::InsertImage(LPCTSTR lpszPathName)
{
	// ȫ��ʹ������ָ��
	CComPtr<IStorage> spStorage;
	CComPtr<ILockBytes> spLockBytes;
	CComPtr<IOleClientSite> spOleClientSite;	
	CComPtr<CImageOle> spImageOle;
	CComPtr<IOleObject> spOleObject;
	CLSID clsid;
	REOBJECT reobject;
	HRESULT hr = E_FAIL;

	do {

		// ����LockBytes
		hr = CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (hr != S_OK) {
			break;
		}

		ASSERT(spLockBytes != NULL);

		// ����Storage
		hr = StgCreateDocfileOnILockBytes(spLockBytes,
			STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &spStorage);
		if (hr != S_OK) {
			break;
		}

		// ��ȡClientSite
		hr = m_pRichEditOle->GetClientSite(&spOleClientSite);
		if (hr != S_OK) {
			break;
		}

		// ����COleImageʵ��
		hr = CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC, IID_IImageOle, (LPVOID*) &spImageOle);
		if (hr != S_OK) {
			break;
		}

		// ����ͼ��
		hr = spImageOle->LoadFromFile(_bstr_t(lpszPathName), this, 400);
		if (hr != S_OK) {
			break;
		}

		// ��ȡIOleObject�ӿ�
		hr = spImageOle->QueryInterface(IID_IOleObject, (LPVOID *) &spOleObject);
		if (hr != S_OK) {
			break;
		}

		// ��ȡIOleObject���û�CLSID
		hr = spImageOle->GetUserClassID(&clsid);
		if (hr != S_OK) {
			break;
		}

		// ���OLE��������
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

		// ����OLE����
		hr = m_pRichEditOle->InsertObject(&reobject);
		if (hr != S_OK) {
			break;
		}

		// ֪ͨOLE������֤OLE������ȷ����
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






