#include "StdAfx.h"
#include "UIAnimation.h"
#include <algorithm>
namespace DuiLib
{
	CUIAnimation::CUIAnimation()
		: m_pOwner(NULL)
	{
	}

	BOOL CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, int nAnimationID /*= 0*/, BOOL bLoop/* = FALSE*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL != pData 
			|| nElapse <= 0
			|| nTotalFrame <= 0
			|| NULL == m_pOwner )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		CAnimationData* pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
		if( NULL == pAnimation ) return FALSE;
		
		if(m_pOwner->GetManager()->SetTimer(m_pOwner, nAnimationID, nElapse))
		{
			m_arAnimations.push_back(pAnimation);
			return TRUE;
		}
		return FALSE;
	}

	VOID CUIAnimation::StopAnimation(int nAnimationID /*= 0*/)
	{
		if (m_pOwner == NULL) return;

		if (nAnimationID  != 0)
		{
			CAnimationData* pData = GetAnimationDataByID(nAnimationID);
			if( NULL != pData )
			{
				m_pOwner->GetManager()->KillTimer( m_pOwner, nAnimationID );
				m_arAnimations.erase(remove(m_arAnimations.begin(), m_arAnimations.end(),pData),  m_arAnimations.end() );
				return;
			}
		}
		else
		{
			int nCount = m_arAnimations.size();
			for(int i=0; i<nCount; ++i)
				m_pOwner->GetManager()->KillTimer( m_pOwner, m_arAnimations[i]->m_nAnimationID );

			m_arAnimations.clear();
		}
	}

	BOOL CUIAnimation::IsAnimationRunning(int nAnimationID)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		return NULL != pData;
	}

	int CUIAnimation::GetCurrentFrame(int nAnimationID/* = 0*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData )
		{
			ASSERT(FALSE);
			return -1;
		}
		return pData->m_nCurFrame;
	}

	BOOL CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID/* = 0*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData)
		{
			ASSERT(FALSE);
			return FALSE;
		}

		if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
		{
			pData->m_nCurFrame = nFrame;
			return TRUE;
		}
		else
		{
			ASSERT(FALSE);
		}
		return FALSE;
	}

	VOID CUIAnimation::OnAnimationElapse(int nAnimationID)
	{
		if(m_pOwner == NULL) return;

		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData ) return;

		int nCurFrame = pData->m_nCurFrame;
		if(nCurFrame == 0)
		{
			OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
			pData->m_bFirstLoop = FALSE;
		}

		OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);

		if(nCurFrame >= pData->m_nTotalFrame)
		{
			OnAnimationStop(nAnimationID);
			if(pData->m_bLoop)
				pData->m_nCurFrame = 0;
			else
			{
				m_pOwner->GetManager()->KillTimer( m_pOwner, nAnimationID );
				m_arAnimations.erase(remove(m_arAnimations.begin(), m_arAnimations.end(),pData),  m_arAnimations.end() );
				pData = NULL;
			}
		}

		if( NULL != pData )
			++(pData->m_nCurFrame);
	}

	CAnimationData* CUIAnimation::GetAnimationDataByID(int nAnimationID)
	{
		CAnimationData* pRet = NULL;
		int nCount = m_arAnimations.size();
		for(int i=0; i<nCount; ++i)
		{
			if(m_arAnimations[i]->m_nAnimationID == nAnimationID)
			{
				pRet = m_arAnimations[i];
				break;
			}
		}

		return pRet;
	}

} // namespace DuiLib