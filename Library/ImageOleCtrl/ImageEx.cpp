#include "stdafx.h"
#include "ImageEx.h"

#define     MIN_FRM_DELAY			100

CImageEx::CImageEx(HWND hWnd)
:m_PageGuid(GUID_NULL)
,m_iCurrentImage(0)
,m_pImage(NULL)
,m_hDCs(NULL)
,m_phBitmaps(NULL)
,m_phOldBitmaps(NULL)
/*,m_hWndContainer(hWnd)*/
,m_hTimer(NULL)
,m_hQueueTimer(NULL)
,m_pPropertyItem(NULL)
{
	m_pImageOleCtrl = new IMAGEOLECTRL;
	if (m_pImageOleCtrl != NULL)
	{
		m_pImageOleCtrl->hMainWnd = hWnd;
		m_pImageOleCtrl->pImageEx = this;
	}
}

CImageEx::~CImageEx()
{
	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if (m_phOldBitmaps != NULL)
	{

	}

	if (m_hDCs != NULL)
	{
		delete[] m_hDCs;
		m_hDCs = NULL;
	}

	if (m_phBitmaps != NULL)
	{
		delete[] m_phBitmaps;
		m_phBitmaps = NULL;
	}

	if (m_hQueueTimer != NULL)
	{
		DeleteTimerQueueTimer(m_hTimer,m_hQueueTimer,NULL);
	}
	if (m_hTimer != NULL)
	{
		DeleteTimerQueue(m_hTimer);
	}
	if (m_pImageOleCtrl != NULL)
	{
		delete m_pImageOleCtrl;
	}
	if (m_pPropertyItem != NULL)
	{
		delete m_pPropertyItem;
	}
}

HRESULT CImageEx::LoadFromFile(BSTR& bstrFileName)
{	
	if (m_pImage)
		delete m_pImage;
	m_pImage = Image::FromFile(bstrFileName);
	if (m_pImage == NULL) {
		return E_FAIL;
	} else if (m_pImage->GetLastStatus() != Ok) {
		delete m_pImage;
		return E_FAIL;
	}

	// ��ȡ��֡������̬ͼ����JPG��PNG�ȵõ�����0��Ҫ��Ϊ1
	m_PageGuid = FrameDimensionTime;
	m_nFrmCount = m_pImage->GetFrameCount(&m_PageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// �õ�ͼ��ߴ�
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();

	//����ͼ��
// 	if (nMaxWidth > 0 && uWidth > nMaxWidth) {
// 		uHeight = uHeight * nMaxWidth / uWidth;
// 		uWidth = nMaxWidth;
// 	}

	// ת����HIMETRIC����0.01���׵�λ�ĳߴ�
	// ���ÿؼ��ߴ�

	m_iCurrentImage = 0;
	 // ��֡������1ʱ
	if (m_nFrmCount > 1)
	{
		// �õ���֡���ӳ�ʱ��
		int nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pPropertyItem = (PropertyItem *) new BYTE[nSize];
		m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
		LONG *pDelays =(LONG *) m_pPropertyItem->value;

		// �����ܵ��ӳ�ʱ���м���100����
		for (int i = 0; i < m_nFrmCount; i++) {
			// �õ����ӳ�ʱ�䵥λ��10���룬����10ת���ɺ���
			// ��ʹ����������ķ����õ��м���100���롣
			// �������ֵ��35����ô����35��10���룬����10��350���룬���������룬�õ�4
			if (pDelays[i] < 200)
				pDelays[i] = 200;
		}		
		m_hTimer = CreateTimerQueue();

		CreateTimerQueueTimer(&m_hQueueTimer,m_hTimer,(WAITORTIMERCALLBACK)TimerRoutine,
			m_pImageOleCtrl,GetFrameDelay(m_iCurrentImage),GetFrameDelay(m_iCurrentImage),WT_EXECUTEDEFAULT);
		// �ٵõ�ÿ��100�����Ӧ��֡����
		// �����1֡200���룬��2֡300���룬��ô
		// m_pFrmIndexes[0] = 0
		// m_pFrmIndexes[1] = 0
		// m_pFrmIndexes[2] = 1
		// m_pFrmIndexes[3] = 1
		// m_pFrmIndexes[4] = 1
		// �����Ϳ�ͨ���������õ���ǰ��100����Ӧ�û�����һ֡
// 		m_pFrmIndexes = new int[m_nTimeCount];
// 		for (int i = 0, j = 0; i < m_nFrmCount; i++) {
// 			for (int k = 0; k < pDelays[i]; k++) {
// 				m_pFrmIndexes[j++] = i;
// 			}
// 		}
		// new�������ڴ������delete
		/*delete [] (BYTE *) m_pPropertyItem;*/
	}

	// ����ÿһ֡�Ļ���λͼ��DC
// 	m_hDCs = new HDC[m_nFrmCount];
// 	m_phBitmaps = new HBITMAP[m_nFrmCount];
// 	m_phOldBitmaps = new HBITMAP[m_nFrmCount];
// 	HDC hDC = ::GetDC(m_hWndContainer);
// 	for (int i = 0; i < m_nFrmCount; i++)
// 	{
// 		m_hDCs[i] = CreateCompatibleDC(hDC);
// 		m_phBitmaps[i] = CreateCompatibleBitmap(hDC, m_nWidth, m_nHeight);
// 		m_phOldBitmaps[i] = (HBITMAP)SelectObject(m_hDCs[i],&m_phBitmaps[i]);
// 		// ����InvalidateRectʱָ���˲��������������ʹ��͸���������л���
// 		// ��ô͸�����ֻ��ᱣ��ǰ��֡�����Ƶ����ݣ�������Ӱ����������ɫ����
// 		// ������������
// 		//m_pDCs[i].FillSolidRect(0, 0, w, h, RGB(255, 255, 255));
// 
// 		//����ǰ֡���Ƶ�HDC��
//  		m_pImage->SelectActiveFrame(&m_PageGuid, i);
// 		Graphics g(m_hDCs[i]);
// 		g.SetSmoothingMode(SmoothingModeHighQuality);
// 		g.DrawImage(m_pImage, 0, 0, m_nWidth, m_nHeight);
// 	}
	//::ReleaseDC(NULL, hDC);

	return S_OK;
}

SIZE CImageEx::GetImageSize()
{
	SIZE ImageSize;
	ImageSize.cx = m_nWidth;
	ImageSize.cy = m_nHeight;

	return ImageSize;
}

LPRECT CImageEx::GetImageRect()
{
	return &m_rcPos;
}

void CImageEx::ShowImage(HDC HdcDraw,LPRECT lpRect)
{
	// ��������������Ϊ di.prcBounds ָ���ľ���
	m_rcPos.top = lpRect->top;
	m_rcPos.left = lpRect->left;
	m_rcPos.right = lpRect->right;
	m_rcPos.bottom = lpRect->bottom;

	m_pImage->SelectActiveFrame(&m_PageGuid,m_iCurrentImage);
	Graphics g(HdcDraw);
	g.DrawImage(m_pImage,lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
}

void CImageEx::OnNextFrame()
{
	if (++m_iCurrentImage >= m_nFrmCount)
		m_iCurrentImage = 0;
}

UINT CImageEx::GetFrameDelay(UINT nFrame /*= 0*/)
{
	if (nFrame == 0)
		nFrame = m_iCurrentImage;

	UINT* lFrameDelay = (UINT*)m_pPropertyItem->value;
	return lFrameDelay[nFrame];
}

UINT CALLBACK CImageEx::TimerRoutine(LPVOID lParam,BOOLEAN TimerOrWaitFired)
{
	PIMAGEOLECTRL pImageOleCtrl = (PIMAGEOLECTRL)lParam;
	if (pImageOleCtrl != NULL)
	{
		pImageOleCtrl->pImageEx->OnNextFrame();
		//��ǰͼ���Ƿ�ɼ���������ɼ������Բ���ˢ��
		::InvalidateRect(pImageOleCtrl->hMainWnd,pImageOleCtrl->pImageEx->GetImageRect(),TRUE);
		ChangeTimerQueueTimer(pImageOleCtrl->pImageEx->m_hTimer,pImageOleCtrl->pImageEx->m_hQueueTimer,pImageOleCtrl->pImageEx->GetFrameDelay(0),pImageOleCtrl->pImageEx->GetFrameDelay(0));
	}
	return 0;
}