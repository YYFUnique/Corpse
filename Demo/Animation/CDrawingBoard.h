
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

	// ������
	// nWidth�����
	// nHeight���߶�
	// nDefaultColor��Ĭ�������ɫֵ
	// bReCreate��ÿ�ζ����´��������ܿ�Ⱥ͸߶��Ƿ����仯
	// bNoCreateInit��û�����´�����������Ƿ�ʹ��Ĭ����ɫֵ��ʼ���������ݿռ�
	// ����ֵ���壺�Ƿ񴴽���һ���µ��ڴ�DC
	bool Create(int nWidth, int nHeight, bool bReCreate = false, bool bNoCreateInit = false);
	void Delete();
	HBITMAP& GetBmpHandle();
	HDC& GetSafeHdc();
	// ���� BGRA ˳���Ų���32λλͼ��Ϣ
	BYTE* GetBits();
	SIZE GetDcSize();

	// ����һ���ڴ�DC��¡
	//bool Clone(CDrawingBoard& FromBoard, IUiFeatureKernel* pUiKernel);

	//// ���Ƶ�����һ���ڴ�DC��
	//bool AlphaBlendTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel, int nAlpha = 255);
	//bool AlphaBlendTo(HDC hDc, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel, int nAlpha = 255);

	//// ��һ��DC���и��ƣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
	//bool StretchBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);
	//bool StretchBltFrom(HDC hDc, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);

	//// ���Ƶ�����һ���ڴ�DC�ϣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
	//bool BitBltTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel);
	//bool BitBltTo(HDC hDc, RECT ToRct, RECT FromRct, IUiFeatureKernel* pUiKernel);

	//// ��һ��DC���и��ƣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
	//bool BitBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);
	//bool BitBltFrom(HDC hDc, RECT FromRct, RECT ToRct, IUiFeatureKernel* pUiKernel);

protected:
	HBITMAP m_hBmp;
	HBITMAP m_hOldBmp;
	HDC m_hDC;
	BYTE* m_pBits;
	SIZE m_DcSize;
};
