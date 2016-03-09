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
		// ���㶯������
		void ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse);
		// ����ִ�н����Ķ���
		void CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect);
	private:
		std::vector<InternalAnimationParam>				m_animationContainer;				// ������������
		std::vector<InternalAnimationParam>::iterator	m_itAnimation;						// ö����
		static const TCHAR *								m_animation;						// �������ƣ���\0�ָ�,\0\0����
		ULONG_PTR m_gdiplusToken;															// gdiplus 
	};
}