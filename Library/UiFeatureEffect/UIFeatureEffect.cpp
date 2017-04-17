// UIEffect.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <time.h>
#include "UIFeatureEffect.h"

IUIEffect* GetAnimation(void)
{
	// 创建对象实例
	IUIEffect *pEffect =  dynamic_cast<IUIEffect*>(new CWndEffect());
	return pEffect != NULL ? pEffect :NULL;
}

BOOL ReleaseAnimation(IUIEffect* &pUIEffect)
{
	// 释放，置空
	CWndEffect* pWndEffect =  dynamic_cast<CWndEffect*>(pUIEffect);
	delete pWndEffect;
	pUIEffect = NULL;

	return TRUE;
}

DWORD GetSurportAnimationType(const TCHAR *& strAnimationType)
{
	return CWndEffect::InitSurportAnimationType(strAnimationType);
}

IImageProcess* GetImageProcess(void)
{
	// 创建对象实例
	IImageProcess *pProcess =  dynamic_cast<IImageProcess*>(new CAlphaBlendImpl());
	return pProcess != NULL ? pProcess : NULL;
}

BOOL ReleaseImageProcess(IImageProcess* &pProcess)
{
	// 释放，置空
	IImageProcess* pImageProcess =  dynamic_cast<CAlphaBlendImpl*>(pProcess);
	delete pImageProcess;
	pImageProcess = NULL;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

CWndEffect::CWndEffect()
{	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CWndEffect::~CWndEffect()
{
	// 清理资源
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	if(m_gdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

DWORD CWndEffect::InitSurportAnimationType(const TCHAR *& strAnimationType)
{
	strAnimationType = m_animation;
	return ANIMATIONNUM;
}

BOOL CWndEffect::AppendAnimation(AnimationParam &aParam)
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

BOOL CWndEffect::DependAnimation(WPARAM effectKey)
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

BOOL CWndEffect::ClearAllAnimation()
{
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	return TRUE;
}

BOOL CWndEffect::Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin)
{
	if (iDrawEffect == NULL)
	{
		return FALSE;
	}

	m_pEffectCallback = iDrawEffect;

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AnimateworkThread,this,0,NULL);

	return TRUE;
}

UINT CWndEffect::AnimateworkThread(LPVOID lParam)
{
	CWndEffect* pEffect = (CWndEffect*)lParam;

	DWORD time_start = clock();
	DWORD delta_time = 0;

	// 队列非空，执行动画
	while (pEffect->m_animationContainer.size() != 0)
	{
		pEffect->ComputeAnimation(pEffect->m_pEffectCallback,delta_time);

		pEffect->m_pEffectCallback->OnUiEffectDraw();

		pEffect->CleanFinishedAnimation(pEffect->m_pEffectCallback);

		Sleep(10);

		delta_time = clock() - time_start;
	}

	return TRUE;
}

void CWndEffect::ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse)
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

void CWndEffect::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CAnimateMgr::CAnimateMgr()
{	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CAnimateMgr::~CAnimateMgr()
{
	// 清理资源
	/*for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	if(m_gdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}*/
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

	m_animationContainer.AddTail(internalParam);

	return TRUE;
}

BOOL CAnimateMgr::DependAnimation(WPARAM effectKey)
{
	BOOL bFind = FALSE;
	/*for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
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
	}*/

	return  bFind;
}

BOOL CAnimateMgr::ClearAllAnimation()
{
	/*for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();*/

	return TRUE;
}

BOOL CAnimateMgr::Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin)
{
	if(iDrawEffect == NULL)
	{
		return FALSE;
	}

	DWORD time_start = clock();
	DWORD delta_time = 0;

	DWORD dwFrame = 0;

	POSITION pos = m_animationContainer.GetHeadPosition();
	// 队列非空，执行动画
	while (pos)
	{
		//ComputeAnimation(iDrawEffect,delta_time);
		InternalAnimationParam& AnimationParam = m_animationContainer.GetNext(pos);
		if (dwFrame != 0)
			AnimationParam.frameNow = dwFrame;
		IEffect *pEffect = (IEffect *)AnimationParam.pEffect;
		if((!AnimationParam.frameNow))
		{
			// 第一帧
			AnimationParam.frameNow++;
			pEffect->ComputeOneFrame(&AnimationParam);
			// 通知绘制模块
			iDrawEffect->OnUiEffectBegin(AnimationParam.param.effectKey, AnimationParam.param.animationEffect);
		}
		else if(delta_time / AnimationParam.param.animationFrequency >= AnimationParam.frameNow)
		{
			// 符合下一帧条件，重新计算
			AnimationParam.frameNow++;
			pEffect->ComputeOneFrame(&AnimationParam);
		}

		if (AnimationParam.frameNow/2== 12)
			AnimationParam.bLastFrame = TRUE;

		iDrawEffect->OnUiEffectDraw();

		//CleanFinishedAnimation(iDrawEffect);
		if (AnimationParam.bLastFrame)
		{
			iDrawEffect->OnUiEffectEnd(AnimationParam.param.effectKey, AnimationParam.param.animationEffect);
			delete []AnimationParam.bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(AnimationParam.pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;

			m_animationContainer.RemoveHead();
		}

		Sleep(10);

		delta_time = clock() - time_start;
	}

	return TRUE;
}

void CAnimateMgr::ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse)
{
	// 此循环过程中不能插入
	//for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	//{	
	//	IEffect *pEffect = (IEffect *)m_itAnimation->pEffect;
	//	if((!m_itAnimation->frameNow))
	//	{
	//		// 第一帧
	//		m_itAnimation->frameNow++;
	//		pEffect->ComputeOneFrame(&*m_itAnimation);
	//		// 通知绘制模块
	//		iDrawEffect->OnUiEffectBegin(m_itAnimation->param.effectKey, m_itAnimation->param.animationEffect);
	//	}
	//	else if(timeElapse / m_itAnimation->param.animationFrequency >= m_itAnimation->frameNow)
	//	{
	//		// 符合下一帧条件，重新计算
	//		m_itAnimation->frameNow++;
	//		pEffect->ComputeOneFrame(&*m_itAnimation);
	//	}
	//}
}

void CAnimateMgr::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
{
	// 绘制完成后删除完成的动画
	/*int sizeVec = m_animationContainer.size();
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

	}*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CAlphaBlendImpl::AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha, int interpolationMode)
{
	return CAlphaBlend::AlphaBlend(desData, desStride, desLeft, desTop, desWidth, desHeight, srcData, srcStride, srcLeft, srcTop, srcWidth, srcHeight, sourceAlpha, interpolationMode);
}

BOOL CAlphaBlendImpl::ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode)
{
	return CScale::ImageScale(DataDes, desWidth, desHeight, DataSrc, srcStride, srcLeft, srcTop, srcWidth, srcHeight, interpolationMode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
