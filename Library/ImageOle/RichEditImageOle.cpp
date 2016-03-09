// RichEditImageOle.cpp : CRichEditImageOle 的实现
#include "stdafx.h"
#include "RichEditImageOle.h"

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
//
// 计算IOleObject接口指针在COleImage类中的偏移量
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

// 初始化静态成员m_dwOleObjectOffset
DWORD CRichEditImageOle::m_dwOleObjectOffset = CRichEditImageOle::GetIOleObjectOffset();

// 根据IOleObject接口指针得到COleImage类的指针
CRichEditImageOle * CRichEditImageOle::FromOleObject(IOleObject *lpOleObject)
{
	// 其实就是简单地用lpOleObject减去前面计算出来的偏移量
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
	// 将剪辑区域设置为 di.prcBounds 指定的矩形

	// 保存绘制矩形区域，前面保存了容器窗口指针，这里保存了要绘制的矩形区域
	// 在更换帧时就可直接调用m_pContainer->InvalidateRect(&m_rcPos, FALSE)了
	m_rcPos = rc;
	if (m_nFrmCount > 0) {
		BitBlt(di.hdcDraw, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_pDCs[m_pFrmIndexes[m_iCurTime]], 0, 0, SRCCOPY);
	}
	return S_OK;
}

STDMETHODIMP CRichEditImageOle::LoadFileFromPath(BSTR lpszFilePath, UINT nMaxWidth)
{
	// 先清除以前加载过的图像
	FinalRelease();

	// 加载图像
	Image *pImage = Image::FromFile(lpszFilePath);
	if (pImage == NULL)
		return E_FAIL;
	else if (pImage->GetLastStatus() != Ok) {
		delete pImage;
		return E_FAIL;
	}

	// 获取总帧数，静态图像，如JPG、PNG等得到的是0，要改为1
	GUID pageGuid;
	m_nFrmCount = pImage->GetFrameCount(&pageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// 得到图像尺寸
	UINT nWidth = pImage->GetWidth();
	UINT nHeight = pImage->GetHeight();

	// 缩放图像
	if (nMaxWidth > 0 && nWidth > nMaxWidth) {
		nHeight = nHeight * nMaxWidth / nWidth;
		nWidth = nMaxWidth;
	}

	// 转化成HIMETRIC，即0.01毫米单位的尺寸
	// 设置控件尺寸
	SIZEL sizel;
	sizel.cx = nWidth;
	sizel.cy = nHeight;
	AtlPixelToHiMetric(&sizel, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	if (m_nFrmCount > 1) 
	{ // 总帧数超过1时
		// 得到各帧的延迟时间
		int nSize = pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *pItem = (PropertyItem *) new BYTE[nSize];
		pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pItem);
		LONG *pDelays =(LONG *) pItem->value;

		// 计算总的延迟时间有几个100毫秒
		for (int i = 0; i < m_nFrmCount; i++) {
			// 得到的延迟时间单位是10毫秒，乘以10转换成毫秒
			// 再使用四舍五入的方法得到有几个100毫秒。
			// 例如这个值是35，那么就是35个10毫秒，乘以10得350毫秒，再四舍五入，得到4
			if (pDelays[i] == 0)
				pDelays[i] = 10;
			pDelays[i] = ((pDelays[i] * 10) + (MIN_FRM_DELAY / 2)) / MIN_FRM_DELAY;
			m_nTimeCount += pDelays[i];
		}

		// 再得到每个100毫秒对应的帧索引
		// 例如第1帧200毫秒，第2帧300毫秒，那么
		// m_pFrmIndexes[0] = 0
		// m_pFrmIndexes[1] = 0
		// m_pFrmIndexes[2] = 1
		// m_pFrmIndexes[3] = 1
		// m_pFrmIndexes[4] = 1
		// 这样就可通过这个数组得到当前这100毫秒应该绘制哪一帧
		m_pFrmIndexes = new int[m_nTimeCount];
		for (int i = 0, j = 0; i < m_nFrmCount; i++) {
			for (int k = 0; k < pDelays[i]; k++) {
				m_pFrmIndexes[j++] = i;
			}
		}
		
		delete [] (BYTE *) pItem;		// new出来的内存别忘了delete
	} else { // 帧数为1时
		// 只是便于绘制，也为m_pFrmIndexes分配一个单元的空间，里面存0就行了
		m_nTimeCount = 1;
		m_pFrmIndexes = new int[1];
		*m_pFrmIndexes = 0;
	}

	// 创建每一帧的缓存位图的DC
	m_pDCs = new HDC[m_nFrmCount];
	m_pBitmaps = new HBITMAP[m_nFrmCount];
	m_ppOldBmps = new HBITMAP*[m_nFrmCount];
	HDC pDC = ::GetDC(NULL);
	for (int i = 0; i < m_nFrmCount; i++) {
		m_pDCs[i] = CreateCompatibleDC(pDC);
		m_pBitmaps[i] = CreateCompatibleBitmap(pDC, nWidth, nHeight);
		m_ppOldBmps[i] = (HBITMAP*)SelectObject(m_pDCs[i],&m_pBitmaps[i]);
		// 由于InvalidateRect时指定了不擦除背景，如果使用透明背景进行绘制
		// 那么透明部分还会保留前面帧所绘制的内容，出现重影，所以填充白色背景
		// 来解决这个问题
		//FillSolidRect(m_pDCs[i],0, 0, nWidth, nHeight, RGB(255, 255, 255));

		// 绘制当前帧
		pImage->SelectActiveFrame(&pageGuid, i);
		Graphics GraphicImage(m_pDCs[i]);
		GraphicImage.SetSmoothingMode(SmoothingModeHighQuality);
		GraphicImage.DrawImage(pImage, 0, 0, nWidth, nHeight);
	}
	::ReleaseDC(NULL, pDC);
	delete pImage;

	return S_OK;
}
