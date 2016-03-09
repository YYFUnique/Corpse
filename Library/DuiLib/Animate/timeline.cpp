#include "stdafx.h"
#include "timeline.h"
#include "movealgo.h"

IntTimeline::IntTimeline()
{
	m_nCurValue = 0;
	m_pOutRef = NULL;
}

IntTimeline::~IntTimeline()
{
}

void IntTimeline::Init()
{

}

void  IntTimeline::SetEaseParam(int from, int to, int t, EaseType eType)
{
	__super::_SetEaseParam((float)from, (float)to, (float)t, eType);
}
void  FloatTimeline::SetEaseParam(float from, float to, float t, EaseType eType)
{
	__super::_SetEaseParam(from, to, t, eType);
}

void  IntTimeline::SetLinerParam1(int from, int to, int t)
{
	__super::_SetLinerParam1((float)from, (float)to, (float)t);
}
void  FloatTimeline::SetLinerParam1(float from, float to, float t)
{
	__super::_SetLinerParam1(from, to, t);
}
void  IntTimeline::SetAccelerateParam1(int from, int to, int t, float Vo)
{
	__super::_SetAccelerateParam1((float)from, (float)to, (float)t, Vo);
}
void  FloatTimeline::SetAccelerateParam1(float from, float to, float t, float Vo)
{
	__super::_SetAccelerateParam1(from, to, t, Vo);
}
void  IntTimeline::OnTick(float fNewValue)
{
	m_nCurValue = round(fNewValue);
    if (m_pOutRef)
    {
        *m_pOutRef = m_nCurValue;
    }
}
void  FloatTimeline::OnTick(float fNewValue)
{
	m_fCurValue = fNewValue;
	if (m_pOutRef)
	{
		*m_pOutRef = m_fCurValue;
	}
}

void IntTimeline::GetCurrentValue(void* pOut)
{
	if (NULL == pOut)
		return;

	*(int*)pOut = m_nCurValue;
}

void FloatTimeline::GetCurrentValue(void* pOut)
{
	if (NULL == pOut)
		return;

	*(float*)pOut = m_fCurValue;
}


void IntTimeline::SetOutRef(int* pRef)
{
	m_pOutRef = pRef;
}

void FloatTimeline::SetOutRef(float* pRef)
{
	m_pOutRef = pRef;
}
//////////////////////////////////////////////////////////////////////////


FloatTimeline::FloatTimeline()
{
    m_fCurValue = 0;
    m_pOutRef = NULL;
}

FloatTimeline::~FloatTimeline()
{
}

void FloatTimeline::Init()
{

}
void  NoneAnimateTimeline::Init() 
{
	m_pMoveAlgorithm = new NoneMoveAlogorithm;
};