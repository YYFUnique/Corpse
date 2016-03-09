#include "stdafx.h"
#include "AnimateMgr.h"

void ClearContainerData(InternalAnimationParam &pElem)
{
	// ɾ���������ڴ�
	delete []pElem.bmpDataCopy;
	pElem.bmpDataCopy = NULL;
	// ɾ����Ч��
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

	// ���зǿգ�ִ�ж���
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
	// ��ѭ�������в��ܲ���
	for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{	
		IEffect *pEffect = (IEffect *)m_itAnimation->pEffect;
		if((!m_itAnimation->frameNow))
		{
			// ��һ֡
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
			// ֪ͨ����ģ��
			iDrawEffect->OnUiEffectBegin(m_itAnimation->param.effectKey, m_itAnimation->param.animationEffect);
		}
		else if(timeElapse / m_itAnimation->param.animationFrequency >= m_itAnimation->frameNow)
		{
			// ������һ֡���������¼���
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
		}

	}
}

void CAnimateMgr::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
{
	// ������ɺ�ɾ����ɵĶ���
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

