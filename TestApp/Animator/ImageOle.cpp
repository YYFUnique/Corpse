// ImageOle.cpp : CImageOle ��ʵ��
#include "stdafx.h"
#include "ImageOle.h"

// CImageOle

//
// ����IOleObject�ӿ�ָ����COleImage���е�ƫ����
//
DWORD CImageOle::GetIOleObjectOffset(void)
{
	const _ATL_INTMAP_ENTRY *pEntries = _GetEntries();
	while (pEntries->pFunc != NULL) {
		if (InlineIsEqualGUID(*(pEntries->piid), IID_IOleObject)) {
			return pEntries->dw;
		}
		pEntries++;
	}
	// ���������е�����
	ASSERT(FALSE);
	return 0;
}

// ��ʼ����̬��Աm_dwOleObjectOffset
DWORD CImageOle::m_dwOleObjectOffset = CImageOle::GetIOleObjectOffset();

// ����IOleObject�ӿ�ָ��õ�COleImage���ָ��
CImageOle * CImageOle::FromOleObject(IOleObject *lpOleObject)
{
	// ��ʵ���Ǽ򵥵���lpOleObject��ȥǰ����������ƫ����
	return (CImageOle *) ((UINT_PTR) lpOleObject - m_dwOleObjectOffset);
}

//
// ��ʼ����Ա
//
HRESULT CImageOle::FinalConstruct()
{
	m_pDCs = NULL;
	m_pBitmaps = NULL;
	m_ppOldBmps = NULL;
	m_pFrmIndexes = NULL;
	m_nFrmCount = 0;
	m_nTimeCount = 0;
	m_iPrevFrm = 0;
	m_iCurTime = 0;
	m_pContainer = 0;
	return S_OK;
}

// �ͷ��ڴ�ռ�
void CImageOle::FinalRelease()
{
	for (int i = 0; i < m_nFrmCount; i++) {
		m_pDCs[i].SelectObject(m_ppOldBmps[i]);
		m_pBitmaps[i].DeleteObject();
		m_pDCs[i].DeleteDC();
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

//
// ���ļ�����ͼ��
// 
// lpszPathName		-	�ļ�·��
// pContainer		-	��������ָ��
// nMaxWidth		-	����ͼ�������ȣ���������ȵ�ͼ��ᱻ���ţ�0��ʾ������
//
HRESULT CImageOle::LoadFromFile(BSTR lpszPathName, CRichEditCtrl *pContainer, UINT nMaxWidth)
{
	// �������ǰ���ع���ͼ��
	FinalRelease();

	// ������������ָ�룬���ڸ���֡ʱ�����������ڵ�InvalidateRect����
	m_pContainer = pContainer;

	// ����ͼ��
	Image *pImage = Image::FromFile(lpszPathName);
	if (pImage == NULL) {
		return E_FAIL;
	} else if (pImage->GetLastStatus() != Ok) {
		delete pImage;
		return E_FAIL;
	}

	// ��ȡ��֡������̬ͼ����JPG��PNG�ȵõ�����0��Ҫ��Ϊ1
	GUID pageGuid = FrameDimensionTime;
	m_nFrmCount = pImage->GetFrameCount(&pageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// �õ�ͼ��ߴ�
	UINT w = pImage->GetWidth();
	UINT h = pImage->GetHeight();

	// ����ͼ��
	if (nMaxWidth > 0 && w > nMaxWidth) {
		h = h * nMaxWidth / w;
		w = nMaxWidth;
	}

	// ת����HIMETRIC����0.01���׵�λ�ĳߴ�
	// ���ÿؼ��ߴ�
	SIZEL sizel;
	sizel.cx = w;
	sizel.cy = h;
	AtlPixelToHiMetric(&sizel, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	if (m_nFrmCount > 1) { // ��֡������1ʱ

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
		// new�������ڴ������delete
		delete [] (BYTE *) pItem;
	} else { // ֡��Ϊ1ʱ
		// ֻ�Ǳ��ڻ��ƣ�ҲΪm_pFrmIndexes����һ����Ԫ�Ŀռ䣬�����0������
		m_nTimeCount = 1;
		m_pFrmIndexes = new int[1];
		*m_pFrmIndexes = 0;
	}

	// ����ÿһ֡�Ļ���λͼ��DC
	m_pDCs = new CDC[m_nFrmCount];
	m_pBitmaps = new CBitmap[m_nFrmCount];
	m_ppOldBmps = new CBitmap *[m_nFrmCount];
	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	for (int i = 0; i < m_nFrmCount; i++) {
		m_pDCs[i].CreateCompatibleDC(pDC);
		m_pBitmaps[i].CreateCompatibleBitmap(pDC, w, h);
		m_ppOldBmps[i] = m_pDCs[i].SelectObject(&m_pBitmaps[i]);
		// ����InvalidateRectʱָ���˲��������������ʹ��͸���������л���
		// ��ô͸�����ֻ��ᱣ��ǰ��֡�����Ƶ����ݣ�������Ӱ����������ɫ����
		// ������������
		m_pDCs[i].FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

		// ���Ƶ�ǰ֡
		pImage->SelectActiveFrame(&pageGuid, i);

		Graphics g(m_pDCs[i]);
		g.SetSmoothingMode(SmoothingModeHighQuality);
		g.DrawImage(pImage, 0, 0, w, h);
	}
	::ReleaseDC(NULL, pDC->m_hDC);
	delete pImage;
	return S_OK;
}


//
// ���ƺ�����ÿһ�οؼ�������������ú����ᱻ���ã�
// ÿһ�ε����������ڵ�InvalidateRectʱ�ú���
// ͬ��Ҳ�ᱻ����
//
HRESULT CImageOle::OnDraw(ATL_DRAWINFO& di)
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

//
// ����֡����ʵ���������������׼ȷ����Ϊ����ÿһ�ε�����������������֡
//
void CImageOle::ChangeFrame()
{
	// ����Ҫ֡������1������������ָ�����ʱ
	if (m_nFrmCount > 1 && m_pContainer != NULL) {
		// ���ӵ�ǰʱ��
		++m_iCurTime;
		// �����ܵ��ӳ�ʱ��ʱ���ִ�0��ʼ
		if (m_iCurTime >= m_nTimeCount) {
			m_iCurTime = 0;
		}
		// ��ǰʱ���Ӧ��֡��������һ�λ��Ƶ�֡������ͬʱ����Ҫ�ػ�
		if (m_pFrmIndexes[m_iCurTime] != m_iPrevFrm) {
			m_pContainer->InvalidateRect(&m_rcPos, FALSE);
			// ������һ�λ��Ƶ�֡����Ϊ��ǰ��֡����
			m_iPrevFrm = m_pFrmIndexes[m_iCurTime];
		}
	}
}
