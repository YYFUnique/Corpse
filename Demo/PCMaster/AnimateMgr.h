#pragma once

#include "UiFeatureEffect/Interface/IUIEffect.h"
#include "UiFeatureEffect/IEffect.h"
#include "UiFeatureEffect/Effect.h"
#include <algorithm>

#define ANIMATIONNUM	85

namespace DuiLib
{
	class CAnimateMgr : public IUIEffect
	{
	public:
		CAnimateMgr();
		~CAnimateMgr();	
	public:
		//IUIEffect
		BOOL AppendAnimation(AnimationParam &aParam);
		BOOL DependAnimation(WPARAM effectKey);
		BOOL ClearAllAnimation();
		BOOL Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin);

	private:
		// 计算动画过程
		void ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse);
		// 清理执行结束的动画
		void CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect);
	private:
		std::vector<InternalAnimationParam>				m_animationContainer;				// 动画参数容器
		std::vector<InternalAnimationParam>::iterator	m_itAnimation;						// 枚举器
		static const TCHAR *								m_animation;						// 动画名称，以\0分割,\0\0结束
		ULONG_PTR m_gdiplusToken;															// gdiplus 
	};
}