// RichEditImageOle.cpp : CRichEditImageOle ��ʵ��
#include "stdafx.h"
#include "RichEditImageOle.h"

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
//
// ����IOleObject�ӿ�ָ����COleImage���е�ƫ����
//
DWORD CRichEditImageOle::GetIOleObjectOffset(void)
{
	const _ATL_INTMAP_ENTRY *pEntries = _GetEntries();
	while (pEntries->pFunc != NULL) {
		if (InlineIsEqualGUID(*(pEntries->piid), IID_IOleObject)) {
			return pEntries->dw;
		}
		pEntries++;
	}

	return 0;
}

// ��ʼ����̬��Աm_dwOleObjectOffset
DWORD CRichEditImageOle::m_dwOleObjectOffset = CRichEditImageOle::GetIOleObjectOffset();

// ����IOleObject�ӿ�ָ��õ�COleImage���ָ��
CRichEditImageOle * CRichEditImageOle::FromOleObject(IOleObject *lpOleObject)
{
	// ��ʵ���Ǽ򵥵���lpOleObject��ȥǰ����������ƫ����
	return (CRichEditImageOle *) ((UINT_PTR) lpOleObject - m_dwOleObjectOffset);
}

// CRichEditImageOle
HRESULT CRichEditImageOle::FinalConstruct()
{
	m_pDCs = NULL;
	m_pBitmaps = NULL;
	m_ppOldBmps = NULL;
	m_pFrmIndexes = NULL;
	m_nFrmCount = 0;
	m_nTimeCount = 0;
	m_iPrevFrm = 0;
	m_iCurTime = 0;
	return S_OK;
}

void CRichEditImageOle::FinalRelease()
{
	for (int i = 0; i < m_nFrmCount; i++) {
		SelectObject(m_pDCs[i],m_ppOldBmps[i]);
		DeleteObject(m_pBitmaps[i]);
		DeleteDC(m_pDCs[i]);
	}
	if (m_pDCs != NULL) {
		delete [] m_pDCs;
	}
	if (m_pBitmaps != NULL) {
		delete [] m_pBitmaps;
	}
	if (m_ppOldBmps != NULL) {
		delete [] m_ppOldBmps;
	}
	if (m_pFrmIndexes != NULL) {
		delete [] m_pFrmIndexes;
	}
	FinalConstruct();
}

HRESULT CRichEditImageOle::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	// ��������������Ϊ di.prcBounds ָ���ľ���

	// ������ƾ�������ǰ�汣������������ָ�룬���ﱣ����Ҫ���Ƶľ�������
	// �ڸ���֡ʱ�Ϳ�ֱ�ӵ���m_pContainer->InvalidateRect(&m_rcPos, FALSE)��
	m_rcPos = rc;
	if (m_nFrmCount > 0) {
		BitBlt(di.hdcDraw, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_pDCs[m_pFrmIndexes[m_iCurTime]], 0, 0, SRCCOPY);
	}
	return S_OK;
}

STDMETHODIMP CRichEditImageOle::LoadFileFromPath(BSTR lpszFilePath, UINT nMaxWidth)
{
	// �������ǰ���ع���ͼ��
	FinalRelease();

	// ����ͼ��
	Image *pImage = Image::FromFile(lpszFilePath);
	if (pImage == NULL)
		return E_FAIL;
	else if (pImage->GetLastStatus() != Ok) {
		delete pImage;
		return E_FAIL;
	}

	// ��ȡ��֡������̬ͼ����JPG��PNG�ȵõ�����0��Ҫ��Ϊ1
	GUID pageGuid;
	m_nFrmCount = pImage->GetFrameCount(&pageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// �õ�ͼ��ߴ�
	UINT nWidth = pImage->GetWidth();
	UINT nHeight = pImage->GetHeight();

	// ����ͼ��
	if (nMaxWidth > 0 && nWidth > nMaxWidth) {
		nHeight = nHeight * nMaxWidth / nWidth;
		nWidth = nMaxWidth;
	}

	// ת����HIMETRIC����0.01���׵�λ�ĳߴ�
	// ���ÿؼ��ߴ�
	SIZEL sizel;
	sizel.cx = nWidth;
	sizel.cy = nHeight;
	AtlPixelToHiMetric(&sizel, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	if (m_nFrmCount > 1) 
	{ // ��֡������1ʱ
		// �õ���֡���ӳ�ʱ��
		int nSize = pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *pItem = (PropertyItem *) new BYTE[nSize];
		pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pItem);
		LONG *pDelays =(LONG *) pItem->value;

		// �����ܵ��ӳ�ʱ���м���100����
		for (int i = 0; i < m_nFrmCount; i++) {
			// �õ����ӳ�ʱ�䵥λ��10���룬����10ת���ɺ���
			// ��ʹ����������ķ����õ��м���100���롣
			// �������ֵ��35����ô����35��10���룬����10��350���룬���������룬�õ�4
			if (pDelays[i] == 0)
				pDelays[i] = 10;
			pDelays[i] = ((pDelays[i] * 10) + (MIN_FRM_DELAY / 2)) / MIN_FRM_DELAY;
			m_nTimeCount += pDelays[i];
		}

		// �ٵõ�ÿ��100�����Ӧ��֡����
		// �����1֡200���룬��2֡300���룬��ô
		// m_pFrmIndexes[0] = 0
		// m_pFrmIndexes[1] = 0
		// m_pFrmIndexes[2] = 1
		// m_pFrmIndexes[3] = 1
		// m_pFrmIndexes[4] = 1
		// �����Ϳ�ͨ���������õ���ǰ��100����Ӧ�û�����һ֡
		m_pFrmIndexes = new int[m_nTimeCount];
		for (int i = 0, j = 0; i < m_nFrmCount; i++) {
			for (int k = 0; k < pDelays[i]; k++) {
				m_pFrmIndexes[j++] = i;
			}
		}
		
		delete [] (BYTE *) pItem;		// new�������ڴ������delete
	} else { // ֡��Ϊ1ʱ
		// ֻ�Ǳ��ڻ��ƣ�ҲΪm_pFrmIndexes����һ����Ԫ�Ŀռ䣬�����0������
		m_nTimeCount = 1;
		m_pFrmIndexes = new int[1];
		*m_pFrmIndexes = 0;
	}

	// ����ÿһ֡�Ļ���λͼ��DC
	m_pDCs = new HDC[m_nFrmCount];
	m_pBitmaps = new HBITMAP[m_nFrmCount];
	m_ppOldBmps = new HBITMAP*[m_nFrmCount];
	HDC pDC = ::GetDC(NULL);
	for (int i = 0; i < m_nFrmCount; i++) {
		m_pDCs[i] = CreateCompatibleDC(pDC);
		m_pBitmaps[i] = CreateCompatibleBitmap(pDC, nWidth, nHeight);
		m_ppOldBmps[i] = (HBITMAP*)SelectObject(m_pDCs[i],&m_pBitmaps[i]);
		// ����InvalidateRectʱָ���˲��������������ʹ��͸���������л���
		// ��ô͸�����ֻ��ᱣ��ǰ��֡�����Ƶ����ݣ�������Ӱ����������ɫ����
		// ������������
		//FillSolidRect(m_pDCs[i],0, 0, nWidth, nHeight, RGB(255, 255, 255));

		// ���Ƶ�ǰ֡
		pImage->SelectActiveFrame(&pageGuid, i);
		Graphics GraphicImage(m_pDCs[i]);
		GraphicImage.SetSmoothingMode(SmoothingModeHighQuality);
		GraphicImage.DrawImage(pImage, 0, 0, nWidth, nHeight);
	}
	::ReleaseDC(NULL, pDC);
	delete pImage;

	return S_OK;
}
