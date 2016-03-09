// ImageOle.cpp : CImageOle 的实现
#include "stdafx.h"
#include "ImageOle.h"

// CImageOle

//
// 计算IOleObject接口指针在COleImage类中的偏移量
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
	// 不可能运行到这里
	ASSERT(FALSE);
	return 0;
}

// 初始化静态成员m_dwOleObjectOffset
DWORD CImageOle::m_dwOleObjectOffset = CImageOle::GetIOleObjectOffset();

// 根据IOleObject接口指针得到COleImage类的指针
CImageOle * CImageOle::FromOleObject(IOleObject *lpOleObject)
{
	// 其实就是简单地用lpOleObject减去前面计算出来的偏移量
	return (CImageOle *) ((UINT_PTR) lpOleObject - m_dwOleObjectOffset);
}

//
// 初始化成员
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

// 释放内存空间
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
// 从文件加载图像
// 
// lpszPathName		-	文件路径
// pContainer		-	容器窗口指针
// nMaxWidth		-	插入图像的最大宽度，超过最大宽度的图像会被缩放，0表示不缩放
//
HRESULT CImageOle::LoadFromFile(BSTR lpszPathName, CRichEditCtrl *pContainer, UINT nMaxWidth)
{
	// 先清除以前加载过的图像
	FinalRelease();

	// 保存容器窗口指针，便于更换帧时调用容器窗口的InvalidateRect函数
	m_pContainer = pContainer;

	// 加载图像
	Image *pImage = Image::FromFile(lpszPathName);
	if (pImage == NULL) {
		return E_FAIL;
	} else if (pImage->GetLastStatus() != Ok) {
		delete pImage;
		return E_FAIL;
	}

	// 获取总帧数，静态图像，如JPG、PNG等得到的是0，要改为1
	GUID pageGuid = FrameDimensionTime;
	m_nFrmCount = pImage->GetFrameCount(&pageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// 得到图像尺寸
	UINT w = pImage->GetWidth();
	UINT h = pImage->GetHeight();

	// 缩放图像
	if (nMaxWidth > 0 && w > nMaxWidth) {
		h = h * nMaxWidth / w;
		w = nMaxWidth;
	}

	// 转化成HIMETRIC，即0.01毫米单位的尺寸
	// 设置控件尺寸
	SIZEL sizel;
	sizel.cx = w;
	sizel.cy = h;
	AtlPixelToHiMetric(&sizel, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	if (m_nFrmCount > 1) { // 总帧数超过1时

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
		// new出来的内存别忘了delete
		delete [] (BYTE *) pItem;
	} else { // 帧数为1时
		// 只是便于绘制，也为m_pFrmIndexes分配一个单元的空间，里面存0就行了
		m_nTimeCount = 1;
		m_pFrmIndexes = new int[1];
		*m_pFrmIndexes = 0;
	}

	// 创建每一帧的缓存位图的DC
	m_pDCs = new CDC[m_nFrmCount];
	m_pBitmaps = new CBitmap[m_nFrmCount];
	m_ppOldBmps = new CBitmap *[m_nFrmCount];
	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	for (int i = 0; i < m_nFrmCount; i++) {
		m_pDCs[i].CreateCompatibleDC(pDC);
		m_pBitmaps[i].CreateCompatibleBitmap(pDC, w, h);
		m_ppOldBmps[i] = m_pDCs[i].SelectObject(&m_pBitmaps[i]);
		// 由于InvalidateRect时指定了不擦除背景，如果使用透明背景进行绘制
		// 那么透明部分还会保留前面帧所绘制的内容，出现重影，所以填充白色背景
		// 来解决这个问题
		m_pDCs[i].FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

		// 绘制当前帧
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
// 绘制函数，每一次控件进入可视区，该函数会被调用，
// 每一次调用容器窗口的InvalidateRect时该函数
// 同样也会被调用
//
HRESULT CImageOle::OnDraw(ATL_DRAWINFO& di)
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

//
// 更换帧，其实这个函数名命名不准确，因为不是每一次调用这个函数都会更换帧
//
void CImageOle::ChangeFrame()
{
	// 必须要帧数大于1，且容器窗口指针存在时
	if (m_nFrmCount > 1 && m_pContainer != NULL) {
		// 增加当前时间
		++m_iCurTime;
		// 超过总的延迟时间时，又从0开始
		if (m_iCurTime >= m_nTimeCount) {
			m_iCurTime = 0;
		}
		// 当前时间对应的帧索引与上一次绘制的帧索引不同时才需要重绘
		if (m_pFrmIndexes[m_iCurTime] != m_iPrevFrm) {
			m_pContainer->InvalidateRect(&m_rcPos, FALSE);
			// 更新上一次绘制的帧索引为当前的帧索引
			m_iPrevFrm = m_pFrmIndexes[m_iCurTime];
		}
	}
}
