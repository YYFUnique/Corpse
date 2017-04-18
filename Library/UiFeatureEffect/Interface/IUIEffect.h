#ifndef IEFFECT_H
#define IEFFECT_H

#ifndef _WINDEF_
#include <WinDef.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 动画参数
struct AnimationParam
{
	WPARAM				effectKey;					// 动画ID
	HBITMAP				hBitmap;						// 动画图片资源
	HDC						hdc;								// 动画图片DC
	SIZE						bmpSize;						// 动画图片尺寸
	BYTE*					pBmpData;					// 动画图片内存地址
	DWORD				animationEffect;			// 动画种类
	DWORD				animationFrequency;	// 动画帧时间
	BOOL					bShow;							// 进入可视区域还是离开可视区域
};

class _declspec(novtable) IUIEffectCallBack
{
public:
	// 每一 个 动画开始时回调
	virtual void OnUiEffectBegin(WPARAM effectKey, DWORD animaType) = 0;
	// 每一 个 动画结束时回调
	virtual void OnUiEffectEnd(WPARAM effectKey, DWORD animaType) = 0;
	// 每一 帧 动画绘制时回调
	virtual void OnUiEffectDraw() = 0;
};

class _declspec(novtable) IUIEffect
{
public:
	// 初始化动画引擎
	virtual BOOL InitializeAnimation() = 0;
	// 加入一个动画
	virtual BOOL AddAnimation(AnimationParam &aParam) = 0;
	// 删除一个动画
	virtual BOOL DeleteAnimation(WPARAM effectKey) = 0;
	// 删除所有动画
	virtual BOOL ClearAllAnimation() = 0;
	// 执行所有动画
	virtual BOOL StartAnimation(IUIEffectCallBack *pDrawEffect,DWORD dwFrameSpin) = 0;
};

// 导出函数
// 获得动画类实例
IUIEffect* GetAnimation(void);
// 删除GetAnimation函数获得的动画类实例
BOOL ReleaseAnimation(IUIEffect* &pUIEffect);
// 获得所支持的动画类型（\0为分隔符），动画名称在
// 字符串中的顺序就是动画种类的输入值（1...N）
DWORD GetSurportAnimationType(const TCHAR* &strType);

typedef IUIEffect* (*GETANIMATION)();
typedef BOOL (*RELEASEANIMATION)(IUIEffect* &);
typedef DWORD (*GETSURPORTANIMATIONTYPE)(const char* &);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class _declspec(novtable) IImageProcess
{
public:
	// alpha 混合
	virtual BOOL AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha = 255, int interpolationMode = 0) = 0;
	// 图片缩放
	virtual BOOL ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode = 0) = 0;
};

// 导出函数
// 获得图像类实例
IImageProcess* GetImageProcess(void);
// 删除GetImageProcess函数获得的图像类实例
BOOL ReleaseImageProcess(IImageProcess* &pProcess);

typedef IImageProcess* (*GETIMAGEPORCESS)();
typedef BOOL (*RELEASEIMAGEPROCESS)(IImageProcess* &);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
