#ifndef IEFFECT_H
#define IEFFECT_H

#ifndef _WINDEF_
#include <WinDef.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
struct AnimationParam
{
	WPARAM				effectKey;					// ����ID
	HBITMAP				hBitmap;						// ����ͼƬ��Դ
	HDC						hdc;								// ����ͼƬDC
	SIZE						bmpSize;						// ����ͼƬ�ߴ�
	BYTE*					pBmpData;					// ����ͼƬ�ڴ��ַ
	DWORD				animationEffect;			// ��������
	DWORD				animationFrequency;	// ����֡ʱ��
	BOOL					bShow;							// ��������������뿪��������
};

class _declspec(novtable) IUIEffectCallBack
{
public:
	// ÿһ �� ������ʼʱ�ص�
	virtual void OnUiEffectBegin(WPARAM effectKey, DWORD animaType) = 0;
	// ÿһ �� ��������ʱ�ص�
	virtual void OnUiEffectEnd(WPARAM effectKey, DWORD animaType) = 0;
	// ÿһ ֡ ��������ʱ�ص�
	virtual void OnUiEffectDraw() = 0;
};

class _declspec(novtable) IUIEffect
{
public:
	// ��ʼ����������
	virtual BOOL InitializeAnimation() = 0;
	// ����һ������
	virtual BOOL AddAnimation(AnimationParam &aParam) = 0;
	// ɾ��һ������
	virtual BOOL DeleteAnimation(WPARAM effectKey) = 0;
	// ɾ�����ж���
	virtual BOOL ClearAllAnimation() = 0;
	// ִ�����ж���
	virtual BOOL StartAnimation(IUIEffectCallBack *pDrawEffect,DWORD dwFrameSpin) = 0;
};

// ��������
// ��ö�����ʵ��
IUIEffect* GetAnimation(void);
// ɾ��GetAnimation������õĶ�����ʵ��
BOOL ReleaseAnimation(IUIEffect* &pUIEffect);
// �����֧�ֵĶ������ͣ�\0Ϊ�ָ�����������������
// �ַ����е�˳����Ƕ������������ֵ��1...N��
DWORD GetSurportAnimationType(const TCHAR* &strType);

typedef IUIEffect* (*GETANIMATION)();
typedef BOOL (*RELEASEANIMATION)(IUIEffect* &);
typedef DWORD (*GETSURPORTANIMATIONTYPE)(const char* &);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class _declspec(novtable) IImageProcess
{
public:
	// alpha ���
	virtual BOOL AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha = 255, int interpolationMode = 0) = 0;
	// ͼƬ����
	virtual BOOL ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode = 0) = 0;
};

// ��������
// ���ͼ����ʵ��
IImageProcess* GetImageProcess(void);
// ɾ��GetImageProcess������õ�ͼ����ʵ��
BOOL ReleaseImageProcess(IImageProcess* &pProcess);

typedef IImageProcess* (*GETIMAGEPORCESS)();
typedef BOOL (*RELEASEIMAGEPROCESS)(IImageProcess* &);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
