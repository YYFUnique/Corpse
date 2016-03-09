#include "stdafx.h"
#include "ImageEx.h"

#define     MIN_FRM_DELAY			100

CImageEx::CImageEx()
:m_bstrFileName(NULL)
,m_PageGuid(GUID_NULL)
,m_iCurrentImage(0)
,m_pImage(NULL)
,m_lprcPos(NULL)
,m_hDCs(NULL)
,m_phBitmaps(NULL)
,m_phOldBitmaps(NULL)
{
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
}

HRESULT CImageEx::LoadFromFile(BSTR bstrFileName)
{
	m_bstrFileName = bstr_t(bstrFileName);
	
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
		PropertyItem *pItem = (PropertyItem *) new BYTE[nSize];
		m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pItem);
		LONG *pDelays =(LONG *) pItem->value;

		// �����ܵ��ӳ�ʱ���м���100����
		for (int i = 0; i < m_nFrmCount; i++) {
			// �õ����ӳ�ʱ�䵥λ��10���룬����10ת���ɺ���
			// ��ʹ����������ķ����õ��м���100���롣
			// �������ֵ��35����ô����35��10���룬����10��350���룬���������룬�õ�4
			if (pDelays[i] == 0)
				pDelays[i] = 10;
			pDelays[i] = ((pDelays[i] * 10) + (MIN_FRM_DELAY / 2)) / MIN_FRM_DELAY;
			//m_nTimeCount += pDelays[i];
		}

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
		delete [] (BYTE *) pItem;
	}

	// ����ÿһ֡�Ļ���λͼ��DC
	m_hDCs = new HDC[m_nFrmCount];
	m_phBitmaps = new HBITMAP[m_nFrmCount];
	m_phOldBitmaps = new HBITMAP[m_nFrmCount];
	HDC hDC = ::GetDC(NULL);
	for (int i = 0; i < m_nFrmCount; i++)
	{
		m_hDCs[i] = CreateCompatibleDC(hDC);
		m_phBitmaps[i] = CreateCompatibleBitmap(hDC, m_nWidth, m_nHeight);
		m_phOldBitmaps[i] = (HBITMAP)SelectObject(m_hDCs[i],&m_phBitmaps[i]);
		// ����InvalidateRectʱָ���˲��������������ʹ��͸���������л���
		// ��ô͸�����ֻ��ᱣ��ǰ��֡�����Ƶ����ݣ�������Ӱ����������ɫ����
		// ������������
		//m_pDCs[i].FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

		//����ǰ֡���Ƶ�HDC��
 		m_pImage->SelectActiveFrame(&m_PageGuid, i);
		Graphics g(m_hDCs[i]);
		g.SetSmoothingMode(SmoothingModeHighQuality);
		g.DrawImage(m_pImage, 0, 0, m_nWidth, m_nHeight);
	}
	::ReleaseDC(NULL, hDC);

	return S_OK;
}

SIZE CImageEx::GetImageSize()
{
	SIZE ImageSize;
	ImageSize.cx = m_nWidth;
	ImageSize.cy = m_nHeight;

	return ImageSize;
}

HRESULT CImageEx::GetFileName(BSTR& lpszFileName)
{
	lpszFileName = m_bstrFileName;
	return S_OK;
}

void CImageEx::ShowImage(HDC HdcDraw,LPRECT lpRect)
{
	// ��������������Ϊ di.prcBounds ָ���ľ���

	// ������ƾ�������ǰ�汣������������ָ�룬���ﱣ����Ҫ���Ƶľ�������
	// �ڸ���֡ʱ�Ϳ�ֱ�ӵ���m_pContainer->InvalidateRect(&m_rcPos, FALSE)��
	m_lprcPos = lpRect;
	if (m_nFrmCount > 0) {
		BitBlt(HdcDraw, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
			m_hDCs[m_iCurrentImage], 0, 0, SRCCOPY);
	}
}

void CImageEx::ChangeFrame()
{

}