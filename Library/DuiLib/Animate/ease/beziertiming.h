#pragma once

//
// 三次方贝赛尔曲线，用于实现平滑动画效果 （缓动）
// http://www.w3.org/TR/css3-transitions/#transition-timing-function-property
// http://easings.net/zh-cn
// http://yiminghe.iteye.com/blog/1762706
//
//
//  B(t) = P0*(1-t)^3 + 3*P1*t(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3 t属于[0,1]区间
// css3 所用的 cubic bezier 已经限定死 p0 = (0,0) , p3= (1,1)
//
// TODO: 疑问
//
//  1. webkit的这个实现仍然没有看明白是怎么回事
//     输入一个t，那计算当中所使用的x,y分别代表什么意思？不应该就只是 from-to 随着 t 而变化吗。这个二维坐标到底是什么意思？
//
//  2. 求导 + 迭代法 + 二分法 的应用
//     求导的意思是明白了，但作用是什么？其它两个算法也没看明白
//


struct BezierArgs
{
    double p1x;
    double p1y;
    double p2x;
    double p2y;
};

const BezierArgs  g_BezierArgs[] = 
{
    // ease-cubic
    {0.25, 0.1, 0.25, 1.0},      // ease
    {0.0, 0.0, 1.0, 1.0},        // linear 
    {0.42, 0, 1.0, 1.0},         // ease_in
    {0, 0, 0.58, 1.0},           // ease_out
    {0.42, 0, 0.58, 1.0},        // ease_in_out

    // sine
    {0.47, 0, 0.745, 0.715},     // ease_in_sine
    {0.39, 0.575, 0.565, 1},     // ease_out_sine
    {0.445, 0.05, 0.55, 0.95},   // ease_in_out_sine

    // quad

    {0.55, 0.085, 0.68, 0.5},    // ease_in_quad
    {0.25, 0.46, 0.45, 0.94},    // ease_out_quad
    {0.455, 0.03, 0.515, 0.955}, // ease_in_out_quad

    // quart
    {0.895, 0.03, 0.685, 0.22},  // ease_in_quart
    {0.165, 0.84, 0.44, 1},      // ease_out_quart
    {0.77, 0, 0.175, 1},         // ease_in_out_quart

    // quint
    {0.755, 0.05, 0.855, 0.06},  // ease_in_quint
    {0.23, 1, 0.32, 1},          // ease_out_quint
    {0.86, 0, 0.07, 1},          // ease_in_out_quint

    // expo
    {0.95, 0.05, 0.795, 0.035},  // ease_in_expo
    {0.19, 1, 0.22, 1},          // ease_out_expo
    {1, 0, 0, 1},                // ease_in_out_expo

    // circ
    {0.6, 0.04, 0.98, 0.335},    // ease_in_circ
    {0.075, 0.82, 0.165, 1},     // ease_out_circ
    {0.785, 0.135, 0.15, 0.86},  // ease_in_out_circ

    // back
    {0.6, -0.28, 0.735, 0.045},  // ease_in_back
    {0.175, 0.885, 0.32, 1.275}, // ease_out_back
    {0.68, -0.55, 0.265, 1.55},  // ease_in_out_back
};


/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
 
#ifndef UnitBezier_h
#define UnitBezier_h

#include <math.h>

namespace WebCore 
{
    struct UnitBezier
    {
        UnitBezier(double p1x, double p1y, double p2x, double p2y)
        {
            // Calculate the polynomial coefficients, implicit first and last control points are (0,0) and (1,1).
            cx = 3.0 * p1x;
            bx = 3.0 * (p2x - p1x) - cx;
            ax = 1.0 - cx -bx;
             
            cy = 3.0 * p1y;
            by = 3.0 * (p2y - p1y) - cy;
            ay = 1.0 - cy - by;
        }
        
        double sampleCurveX(double t)
        {
            // `ax t^3 + bx t^2 + cx t' expanded using Horner's rule.
            return ((ax * t + bx) * t + cx) * t;
        }
        
        double sampleCurveY(double t)
        {
            return ((ay * t + by) * t + cy) * t;
        }
        
        double sampleCurveDerivativeX(double t)
        {
            return (3.0 * ax * t + 2.0 * bx) * t + cx;
        }
        
        // Given an x value, find a parametric value it came from.
        double solveCurveX(double x, double epsilon)
        {
            double t0;
            double t1;
            double t2;
            double x2;
            double d2;
            int i;

            // First try a few iterations of Newton's method -- normally very fast.
            for (t2 = x, i = 0; i < 8; i++) {
                x2 = sampleCurveX(t2) - x;
                if (fabs (x2) < epsilon)
                    return t2;
                d2 = sampleCurveDerivativeX(t2);
                if (fabs(d2) < 1e-6)
                    break;
                t2 = t2 - x2 / d2;
            }

            // Fall back to the bisection method for reliability.
            t0 = 0.0;
            t1 = 1.0;
            t2 = x;

            if (t2 < t0)
                return t0;
            if (t2 > t1)
                return t1;

            while (t0 < t1) {
                x2 = sampleCurveX(t2);
                if (fabs(x2 - x) < epsilon)
                    return t2;
                if (x > x2)
                    t0 = t2;
                else
                    t1 = t2;
                t2 = (t1 - t0) * .5 + t0;
            }

            // Failure.
            return t2;
        }

        double solve(double x, double epsilon)
        {
            return sampleCurveY(solveCurveX(x, epsilon));
        }
        
    private:
        double ax;
        double bx;
        double cx;
        
        double ay;
        double by;
        double cy;
    };


    // The epsilon value we pass to UnitBezier::solve given that the animation is going to run over |dur| seconds. The longer the
    // animation, the more precision we need in the timing function result to avoid ugly discontinuities.
    static inline double solveEpsilon(double duration)
    {
        return 1.0 / (200.0 * duration);
    }

    static inline double solveCubicBezierFunction(double p1x, double p1y, double p2x, double p2y, double t, double duration)
    {
        // Convert from input time to parametric value in curve, then from
        // that to output time.
        UnitBezier bezier(p1x, p1y, p2x, p2y);
        return bezier.solve(t, solveEpsilon(duration));
    }
}
#endif


// 测试代码
#if 0
double dFrom = 0.0;
double dTo = 100.0;
double duration = 1000;

int pfs = 30;

int nTime = 0;
int nTimeStart = GetTickCount();
double timepercent = 0;

double dprev = 0;
while (1)
{
    nTime = GetTickCount() - nTimeStart;
    if (nTime > duration)
    {
        cout << dTo << endl;
        break;
    }

    timepercent = nTime / duration;
    double dPercent = solveCubicBezierFunction(0.68, -0.55, 0.265, 1.55, timepercent, duration);
    double dCur = dFrom + (dTo-dFrom)*dPercent;

    cout << dCur << "   (" << (dCur-dprev) << ")" << endl;
    dprev = dCur;
    Sleep(pfs);
}
#endif