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

	// 获取总帧数，静态图像，如JPG、PNG等得到的是0，要改为1
	m_PageGuid = FrameDimensionTime;
	m_nFrmCount = m_pImage->GetFrameCount(&m_PageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// 得到图像尺寸
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();

	//缩放图像
// 	if (nMaxWidth > 0 && uWidth > nMaxWidth) {
// 		uHeight = uHeight * nMaxWidth / uWidth;
// 		uWidth = nMaxWidth;
// 	}

	// 转化成HIMETRIC，即0.01毫米单位的尺寸
	// 设置控件尺寸

	m_iCurrentImage = 0;
	 // 总帧数超过1时
	if (m_nFrmCount > 1)
	{
		// 得到各帧的延迟时间
		int nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pPropertyItem = (PropertyItem *) new BYTE[nSize];
		m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
		LONG *pDelays =(LONG *) m_pPropertyItem->value;

		// 计算总的延迟时间有几个100毫秒
		for (int i = 0; i < m_nFrmCount; i++) {
			// 得到的延迟时间单位是10毫秒，乘以10转换成毫秒
			// 再使用四舍五入的方法得到有几个100毫秒。
			// 例如这个值是35，那么就是35个10毫秒，乘以10得350毫秒，再四舍五入，得到4
			if (pDelays[i] < 200)
				pDelays[i] = 200;
		}		
		m_hTimer = CreateTimerQueue();

		CreateTimerQueueTimer(&m_hQueueTimer,m_hTimer,(WAITORTIMERCALLBACK)TimerRoutine,
			m_pImageOleCtrl,GetFrameDelay(m_iCurrentImage),GetFrameDelay(m_iCurrentImage),WT_EXECUTEDEFAULT);
		// 再得到每个100毫秒对应的帧索引
		// 例如第1帧200毫秒，第2帧300毫秒，那么
		// m_pFrmIndexes[0] = 0
		// m_pFrmIndexes[1] = 0
		// m_pFrmIndexes[2] = 1
		// m_pFrmIndexes[3] = 1
		// m_pFrmIndexes[4] = 1
		// 这样就可通过这个数组得到当前这100毫秒应该绘制哪一帧
// 		m_pFrmIndexes = new int[m_nTimeCount];
// 		for (int i = 0, j = 0; i < m_nFrmCount; i++) {
// 			for (int k = 0; k < pDelays[i]; k++) {
// 				m_pFrmIndexes[j++] = i;
// 			}
// 		}
		// new出来的内存别忘了delete
		/*delete [] (BYTE *) m_pPropertyItem;*/
	}

	// 创建每一帧的缓存位图的DC
// 	m_hDCs = new HDC[m_nFrmCount];
// 	m_phBitmaps = new HBITMAP[m_nFrmCount];
// 	m_phOldBitmaps = new HBITMAP[m_nFrmCount];
// 	HDC hDC = ::GetDC(m_hWndContainer);
// 	for (int i = 0; i < m_nFrmCount; i++)
// 	{
// 		m_hDCs[i] = CreateCompatibleDC(hDC);
// 		m_phBitmaps[i] = CreateCompatibleBitmap(hDC, m_nWidth, m_nHeight);
// 		m_phOldBitmaps[i] = (HBITMAP)SelectObject(m_hDCs[i],&m_phBitmaps[i]);
// 		// 由于InvalidateRect时指定了不擦除背景，如果使用透明背景进行绘制
// 		// 那么透明部分还会保留前面帧所绘制的内容，出现重影，所以填充白色背景
// 		// 来解决这个问题
// 		//m_pDCs[i].FillSolidRect(0, 0, w, h, RGB(255, 255, 255));
// 
// 		//将当前帧绘制到HDC中
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
	// 将剪辑区域设置为 di.prcBounds 指定的矩形
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
		//当前图像是否可见，如果不可见，可以不用刷新
		::InvalidateRect(pImageOleCtrl->hMainWnd,pImageOleCtrl->pImageEx->GetImageRect(),TRUE);
		ChangeTimerQueueTimer(pImageOleCtrl->pImageEx->m_hTimer,pImageOleCtrl->pImageEx->m_hQueueTimer,pImageOleCtrl->pImageEx->GetFrameDelay(0),pImageOleCtrl->pImageEx->GetFrameDelay(0));
	}
	return 0;
}