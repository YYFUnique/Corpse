// UIEffect.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <time.h>
#include "UIFeatureEffect.h"

IUIEffect* GetAnimation(void)
{
	// 创建对象实例
	IUIEffect *pEffect =  dynamic_cast<IUIEffect*>(new CTimedEffect());
	return pEffect != NULL ? pEffect :NULL;
}

BOOL ReleaseAnimation(IUIEffect* &pUIEffect)
{
	// 释放，置空
	//CTimedEffect* pWndEffect =  dynamic_cast<CTimedEffect*>(pUIEffect);
	delete pUIEffect;
	pUIEffect = NULL;

	return TRUE;
}

DWORD GetSurportAnimationType(const TCHAR *& strAnimationType)
{
	return CTimedEffect::InitSurportAnimationType(strAnimationType);
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

CTimedEffect::CTimedEffect()
{	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	InitializeCriticalSection(&m_CriticalSection);
}

CTimedEffect::~CTimedEffect()
{
	// 清理资源
	EnterCriticalSection(&m_CriticalSection);
	for_each(m_animationContainer.begin(), m_animationContainer.end(), ClearContainerData);
		m_animationContainer.clear();

	//通知线程退出
	m_bExitThread = TRUE;
	if (m_hThreadEvent != NULL)
	{
		SetEvent(m_hThreadEvent);
		CloseHandle(m_hThreadEvent);
	}

	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (m_gdiplusToken != 0)
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	
	DeleteCriticalSection(&m_CriticalSection);
}

BOOL CTimedEffect::InitializeAnimation()
{
	m_bExitThread = FALSE;

	BOOL bSuccess = FALSE;

	do 
	{
		m_hThreadEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		if (m_hThreadEvent == NULL)
			break;

		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnimateThread, this, 0, NULL);
		if (m_hThread == NULL)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

DWORD CTimedEffect::InitSurportAnimationType(const TCHAR *& strAnimationType)
{
	strAnimationType = m_animation;
	return ANIMATIONNUM;
}

BOOL CTimedEffect::AddAnimation(AnimationParam &aParam)
{
	if (aParam.animationEffect > ANIMATIONNUM+1 || aParam.animationEffect <= 1)
		return FALSE;

	InternalAnimationParam internalParam;
	memcpy(&internalParam.param,&aParam,sizeof(aParam));

	SIZE ImageSize = internalParam.param.bmpSize;
	DWORD dwBMPSize = ImageSize.cx * ImageSize.cy * 4;
	internalParam.bmpDataCopy = new BYTE[dwBMPSize];

	if (internalParam.bmpDataCopy == NULL)
		return FALSE;

	memcpy(internalParam.bmpDataCopy, aParam.pBmpData, dwBMPSize);
	internalParam.frameNow = 0;
	internalParam.bLastFrame = FALSE;

	IEffect *pEffect = CEffectFactory::CreateEffect(aParam.animationEffect);
	internalParam.pEffect = pEffect;
	pEffect->InitEffectParam(&internalParam);
	
	EnterCriticalSection(&m_CriticalSection);
	m_animationContainer.push_back(internalParam);
	LeaveCriticalSection(&m_CriticalSection);

	return TRUE;
}

BOOL CTimedEffect::DeleteAnimation(WPARAM effectKey)
{
	BOOL bFind = FALSE;
	EnterCriticalSection(&m_CriticalSection);
	for (m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{
		if (m_itAnimation->param.effectKey == effectKey)
		{
			bFind = TRUE;
			delete []m_itAnimation->bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(m_itAnimation->pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;
			m_animationContainer.erase(m_itAnimation);

			break;
		}
	}

	LeaveCriticalSection(&m_CriticalSection);
	return  bFind;
}

BOOL CTimedEffect::ClearAllAnimation()
{
	EnterCriticalSection(&m_CriticalSection);
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
		m_animationContainer.clear();

	LeaveCriticalSection(&m_CriticalSection);

	return TRUE;
}

BOOL CTimedEffect::StartAnimation(IUIEffectCallBack *pDrawEffect,DWORD frameSpin)
{	
	if (pDrawEffect == NULL)
		return FALSE;

	m_pEffectCallback = pDrawEffect;

	if (m_hThreadEvent != NULL)
		SetEvent(m_hThreadEvent);

	return TRUE;
}

UINT CTimedEffect::AnimateThread(LPVOID lParam)
{
	CTimedEffect* pEffect = (CTimedEffect*)lParam;

	DWORD dwStart = clock();
	DWORD dwDelta = 0;

	while(TRUE)
	{
		if (pEffect->m_bExitThread)
			break;

		WaitForSingleObject(pEffect->m_hThreadEvent, INFINITE);

		EnterCriticalSection(&pEffect->m_CriticalSection);
		DWORD dwCount = pEffect->m_animationContainer.size();
		LeaveCriticalSection(&pEffect->m_CriticalSection);

		while (dwCount && pEffect->m_bExitThread == FALSE)
		{
			EnterCriticalSection(&pEffect->m_CriticalSection);
			pEffect->ComputeAnimation(pEffect->m_pEffectCallback, dwDelta);
			pEffect->m_pEffectCallback->OnUiEffectDraw();
			pEffect->CleanFinishedAnimation(pEffect->m_pEffectCallback);
			dwCount = pEffect->m_animationContainer.size();
			LeaveCriticalSection(&pEffect->m_CriticalSection);

			dwDelta = clock() - dwStart;

			Sleep(10);
		}	
	}

	return TRUE;
}

void CTimedEffect::ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse)
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

void CTimedEffect::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
{
	// 绘制完成后删除完成的动画
	int sizeVec = m_animationContainer.size();
	for (int i = 0; i < sizeVec; i++)
	{
		if (m_animationContainer[i].bLastFrame)
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

}

CAnimateMgr::~CAnimateMgr()
{
}

BOOL CAnimateMgr::AddAnimation(AnimationParam &aParam)
{
	return TRUE;
}

BOOL CAnimateMgr::DeleteAnimation(WPARAM effectKey)
{
	BOOL bFind = FALSE;

	return  bFind;
}

BOOL CAnimateMgr::ClearAllAnimation()
{
	return TRUE;
}

BOOL CAnimateMgr::RunAnimation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin)
{
	return TRUE;
}

IUIEffect* CAnimateMgr::CreateAnimateEffect()
{	
	//switch ()
	//{
	//}
	return m_pUIEffect;
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
