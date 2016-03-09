
#pragma once
#include <Windows.h>

/*class IUiFeatureKernel;*/


#ifndef IS_SAFE_HANDLE
#define IS_SAFE_HANDLE(hWnd)						(((hWnd)!=NULL)&&((hWnd)!=INVALID_HANDLE_VALUE))
#endif

#ifndef IS_INVALID_HANDLE
#define IS_INVALID_HANDLE(hWnd)						(((hWnd)==NULL)||((hWnd)==INVALID_HANDLE_VALUE))
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(pMem)							{if((pMem)!=NULL){delete((pMem));(pMem)=NULL;}};
#endif

#ifndef SAFE_DELETE_LIST
#define SAFE_DELETE_LIST(pMem)						{if((pMem)!= NULL) {delete[]((pMem));(pMem)=NULL;}};
#endif

class CDrawingBoard
{
public:
    CDrawingBoard();
    virtual ~CDrawingBoard();

	// 参数：
	// nWidth：宽度
	// nHeight：高度
	// nDefaultColor：默认填充颜色值
	// bReCreate：每次都重新创建，不管宽度和高度是否发生变化
	// bNoCreateInit：没有重新创建的情况下是否使用默认颜色值初始化整个数据空间
	// 返回值含义：是否创建了一个新的内存DC
	bool Create(int nWidth, int nHeight, bool bReCreate = false, bool bNoCreateInit = false);
	void Delete();
	HBITMAP& GetBmpHandle();
	HDC& GetSafeHdc();
	// 按照 BGRA 顺序排布的32位位图信息
	BYTE* GetBits();
	SIZE GetDcSize();

	// 从另一个内存DC克隆
	//bool Clone(CDrawingBoard& FromBoard, IUiFeatureKernel* pUiKernel);

	//// 绘制到另外一个内存DC上
	//bool AlphaBlendTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel, int nAlpha = 255);
	//bool AlphaBlendTo(HDC hDc, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel, int nAlpha = 255);

	//// 从一个DC进行复制，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	//bool StretchBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);
	//bool StretchBltFrom(HDC hDc, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);

	//// 绘制到另外一个内存DC上，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	//bool BitBltTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel);
	//bool BitBltTo(HDC hDc, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel);

	//// 从一个DC进行复制，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	//bool BitBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);
	//bool BitBltFrom(HDC hDc, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);

protected:
	HBITMAP m_hBmp;
	HBITMAP m_hOldBmp;
	HDC m_hDC;
	BYTE* m_pBits;
	SIZE m_DcSize;
};
