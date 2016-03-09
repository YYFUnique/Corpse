#include "stdafx.h"
#include "AnimateMgr.h"

void ClearContainerData(InternalAnimationParam &pElem)
{
	// 删除拷贝的内存
	delete []pElem.bmpDataCopy;
	pElem.bmpDataCopy = NULL;
	// 删除特效类
	pElem.pEffect->ReleaseEffectParam();
	delete dynamic_cast<CEffect*>(pElem.pEffect);
	pElem.pEffect = NULL;
}

CAnimateMgr::CAnimateMgr()
{

}

CAnimateMgr::~CAnimateMgr()
{

}

BOOL CAnimateMgr::AppendAnimation(AnimationParam &aParam)
{
	if(aParam.animationEffect > ANIMATIONNUM+1 || aParam.animationEffect <= 1)
	{
		return FALSE;
	}

	InternalAnimationParam internalParam;
	memcpy(&internalParam.param,&aParam,sizeof(aParam));
	DWORD dataSize = internalParam.param.bmpSize.cx*internalParam.param.bmpSize.cy*4;
	internalParam.bmpDataCopy = new BYTE[dataSize];

	if(internalParam.bmpDataCopy == NULL)
	{
		return FALSE;
	}

	memcpy(internalParam.bmpDataCopy,aParam.pBmpData,dataSize);
	internalParam.frameNow = 0;
	internalParam.bLastFrame = FALSE;

	IEffect *pEffect = CEffectFactory::CreateEffect(aParam.animationEffect);
	internalParam.pEffect = pEffect;
	pEffect->InitEffectParam(&internalParam);

	m_animationContainer.push_back(internalParam);

	return TRUE;
}

BOOL CAnimateMgr::DependAnimation(WPARAM effectKey)
{
	BOOL bFind = FALSE;
	for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{
		if(m_itAnimation->param.effectKey == effectKey)
		{
			delete []m_itAnimation->bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(m_itAnimation->pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;
			m_animationContainer.erase(m_itAnimation);

			bFind = TRUE;
		}
	}

	return  bFind;
}

BOOL CAnimateMgr::ClearAllAnimation()
{
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	return TRUE;
}

BOOL CAnimateMgr::Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin)
{
	if(iDrawEffect == NULL)
	{
		return FALSE;
	}

	DWORD time_start = GetTickCount();
	DWORD delta_time = 0;

	// 队列非空，执行动画
	while (m_animationContainer.size() != 0)
	{
		ComputeAnimation(iDrawEffect,delta_time);

		iDrawEffect->OnUiEffectDraw();

		CleanFinishedAnimation(iDrawEffect);

		Sleep(10);

		delta_time = GetTickCount() - time_start;
	}

	return TRUE;
}


void CAnimateMgr::ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse)
{
	// 此循环过程中不能插入
	for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{	
		IEffect *pEffect = (IEffect *)m_itAnimation->pEffect;
		if((!m_itAnimation->frameNow))
		{
			// 第一帧
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
			// 通知绘制模块
			iDrawEffect->OnUiEffectBegin(m_itAnimation->param.effectKey, m_itAnimation->param.animationEffect);
		}
		else if(timeElapse / m_itAnimation->param.animationFrequency >= m_itAnimation->frameNow)
		{
			// 符合下一帧条件，重新计算
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
		}

	}
}

void CAnimateMgr::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
{
	// 绘制完成后删除完成的动画
	int sizeVec = m_animationContainer.size();
	for(int i = 0; i < sizeVec; i++)
	{
		if(m_animationContainer[i].bLastFrame)
		{
			iDrawEffect->OnUiEffectEnd(m_animationContainer[i].param.effectKey, m_animationContainer[i].param.animationEffect);
			delete []m_animationContainer[i].bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(m_animationContainer[i].pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;
			m_animationContainer.erase(m_animationContainer.begin()+i);
			sizeVec = m_animationContainer.size();
		}

	}
}

