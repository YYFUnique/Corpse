#pragma once
#include "UISDK/Kernel/Src/Animate/ease/easing.h"
#include "UISDK/Kernel/Src/Animate/ease/beziertiming.h"

namespace UI
{
typedef float valueType;

// 动画时，动画算法

// ??
// 1. 如何判断运动结束
//    如果指定了时间，就按时间，否则按路程



class NoneMoveAlogorithm : public IMoveAlgorithm
{
public:
	virtual void  Reverse() {

	}
	virtual bool OnTick(int t, float* pCurrentValue){ 
		return false;
	}
};

class LinearMove : public IMoveAlgorithm
{
public:
    LinearMove()
    {
        m_from = m_to = m_t = 0;
        m_v = 0.0f;
        m_eFinishtType = CHECK_FINISH_BY_S;
        m_eSetParamType = SET_PARAM_NONE;
    }
    ~LinearMove()
    {

    }

    virtual void  SetParam1(valueType from, valueType to, valueType t)
    {
        m_eSetParamType = SET_PARAM_FROM_TO_T;

        m_from = from;
        m_to = to;
        m_t = t;

        UIASSERT(m_t != 0);

        if (0 == m_t)
            m_t = 1;

        m_v = (m_to - m_from)/(float)m_t;
        m_eFinishtType = CHECK_FINISH_BY_T;
    }
    virtual void  SetParam2(valueType from, valueType to, float v)
    {
        m_eSetParamType = SET_PARAM_FROM_TO_V;

        m_from = from;
        m_to = to;
        m_v = v;

        UIASSERT(m_from != m_to);
        UIASSERT(v != 0.0f);

        if (0.0f == v)
            m_v = 1.0f;

        m_t = (valueType)((m_to - m_from)/(float)m_v + 0.5);
        m_eFinishtType = CHECK_FINISH_BY_S;
    }
    virtual void  SetParam3(valueType from, float v, valueType t)
    {
        m_eSetParamType = SET_PARAM_FROM_V_T;

        m_from = from;
        m_to = 0;
        m_t = t;
        m_v = v;

        UIASSERT(v != 0.0f);
        UIASSERT(m_t != 0);

        m_to = m_from + (valueType)(m_t*m_v/* + 0.5*/);
        m_eFinishtType = CHECK_FINISH_BY_T;
    }

    virtual bool  OnTick(int t, valueType* pCurrentValue)
    {
        // #ifdef _DEBUG
        //     {
        //         static int s_tprev = 0;
        //         TCHAR szText[64] = _T("");
        //         _stprintf(szText, _T("%d\n"), t-s_tprev);
        //         s_tprev = t;
        //         ::OutputDebugString(szText);
        //     }
        // #endif

        bool bFinish = false;
        if (CHECK_FINISH_BY_T == m_eFinishtType)
        {
            if (t >= m_t)
            {
                bFinish = true;
                *pCurrentValue = m_to;
            }
            else
            {
                *pCurrentValue = m_from + (valueType)(m_v * t/* + 0.5*/);
            }
        }
        else if (CHECK_FINISH_BY_S == m_eFinishtType)
        {
            *pCurrentValue = m_from + (valueType)(m_v * t/* + 0.5*/);
            if ((m_to > m_from && *pCurrentValue >= m_to) ||
                (m_to < m_from && *pCurrentValue <= m_to))
            {
                bFinish = true;
                *pCurrentValue = m_to;
            }
        }

        return bFinish;
    }
    virtual void  Reverse()
    {
        switch (m_eSetParamType)
        {
        case SET_PARAM_FROM_TO_T:
            {
                SetParam1(m_to, m_from, m_t);
            }
            break;

        case SET_PARAM_FROM_TO_V:
            {
                SetParam2(m_to, m_from, -m_v);
            }
            break;

        case SET_PARAM_FROM_V_T:
            {
                SetParam3(m_to, -m_v, m_t);
            }
            break;
        }
    }

public:
    valueType  m_from;
    valueType  m_to;
    float m_v;
    valueType  m_t;

    enum CHECK_FINISH_TYPE 
    {
        CHECK_FINISH_BY_S,
        CHECK_FINISH_BY_T,
    };
    CHECK_FINISH_TYPE  m_eFinishtType;  

    enum SET_PARAM_TYPE
    {
        SET_PARAM_NONE,
        SET_PARAM_FROM_TO_T,
        SET_PARAM_FROM_TO_V,
        SET_PARAM_FROM_V_T,
    };
    SET_PARAM_TYPE  m_eSetParamType;
};

// Vt^2 - Vo^2 = 2as
// s = Vot + 1/2at^2
// a = (2s - 2Vot)/t^2
// 匀加速或匀减速运动
class AccelerateMove : public IMoveAlgorithm
{
public:
    AccelerateMove()
    {
        m_from = m_to = m_t = 0;
        m_vo = m_a = 0.0f;
        // m_vt = 0.0f;
        m_eFinishtType = CHECK_FINISH_BY_T;
    }
    ~AccelerateMove()
    {

    }

    virtual void SetParam1(valueType from, valueType to, valueType t, float Vo)
    {
        m_from = from;
        m_to = to;
        m_t = t;
        m_vo = Vo;
        m_eFinishtType = CHECK_FINISH_BY_T;

        UIASSERT (0 != t);
        if (0 == t)
            t = 1;

        valueType s = m_to - m_from;
        m_a = (float)((s*2) - ((t*2)*Vo)) / (t*t);
    }
    //        virtual void SetParam2(int from, int t, int a, int Vo, int Vt)
    //{
    //     m_from = from;
    //     m_t = t;
    //     m_vo = Vo;
    //     m_vt = Vt;
    //     m_a = a;
    // 
    //     float s = Vo * t + 0.5 * a * t * 5;
    //     m_to = m_from + s;
    // 
    //     m_eFinishtType = CHECK_FINISH_BY_T;
    //}

    virtual bool  OnTick(int t, valueType* pCurrentValue)
    {
        bool bFinish = false;
        if (CHECK_FINISH_BY_T == m_eFinishtType)
        {
            if (t >= m_t)
            {
                bFinish = true;
                *pCurrentValue = m_to;
            }
            else
            {
                *pCurrentValue = m_from + (valueType)(m_vo * t + 0.5f * m_a * t * t);
            }
        }
        else if (CHECK_FINISH_BY_S == m_eFinishtType)
        {
            *pCurrentValue = m_from + (valueType)(m_vo * t + 0.5f * m_a * t * t);
            if (*pCurrentValue >= m_to)
            {
                bFinish = true;
                *pCurrentValue = m_to;
            }
        }
        return bFinish;
    }
    virtual void  Reverse()
    {
        SetParam1(m_to, m_from, m_t, -m_vo);
    }

public:
    valueType    m_from;
    valueType    m_to;
    valueType    m_t;
    float  m_vo;
    //float  m_vt;
    float  m_a;

    enum CHECK_FINISH_TYPE 
    {
        CHECK_FINISH_BY_S,
        CHECK_FINISH_BY_T,
    };
    CHECK_FINISH_TYPE m_eFinishtType;  
};

class EasingMove : public IMoveAlgorithm
{
public:
    EasingMove()
    {
        m_from = m_to = m_t = m_s = 0;
        m_easetype = ease;
    }
    ~EasingMove()
    {

    }

    virtual void SetParam(valueType from, valueType to, valueType t, EaseType eType)
    {
        m_from = from;
        m_to = to;
        m_t = t;

        if (m_t == 0) // 避免除0
        {
            m_t = 1;
        }

        m_s = m_to-m_from;

        if (eType >= ease_end)
        {
            eType = ease;
        }
        m_easetype = eType;
    }

    virtual bool  OnTick(int t, valueType* pCurrentValue)
    {
        if (t >= m_t)
        {
            *pCurrentValue = m_to;
            return true;
        }

        double dtPercent = (double)t/(double)m_t;  // 时间比例
        double dsPercent = 0;


        // 计算已走的路程比例
        if (m_easetype > ease_bezier_support)
        {
            dsPercent = Easing(m_easetype, dtPercent);
        }
        else
        {
            dsPercent = WebCore::solveCubicBezierFunction(
                g_BezierArgs[m_easetype].p1x,
                g_BezierArgs[m_easetype].p1y,
                g_BezierArgs[m_easetype].p2x,
                g_BezierArgs[m_easetype].p2y,
                (double)dtPercent, (double)m_t
                );
        }
        *pCurrentValue = valueType(m_from + m_s * dsPercent);
        return false;
    }
    virtual void  Reverse()
    {
        SetParam(m_to, m_from, m_t, m_easetype);
    }

public:
    valueType    m_from;
    valueType    m_to;
    valueType    m_t;
    valueType    m_s;  // m_to - m_from;

    EaseType     m_easetype;
};
}