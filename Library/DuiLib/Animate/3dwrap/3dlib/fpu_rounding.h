#ifndef __FPU_ROUNDING_H_
#define __FPU_ROUNDING_H_

// http://technet.microsoft.com/zh-cn/library/e9b52ceh(v=vs.110).aspx
//////////////////////////////////////////////////////////////////////////
// http://www.musicdsp.org/showone.php?id=246
/*
Float to integer conversion
References : Posted by Peter Schoffhauzer
			 --------------------------------------------------------------------------------
Notes : 
The fld x/fistp i instructions can be used for fast conversion of 
floating point numbers to integers. By default, the number is rounded 
to the nearest integer. However, sometimes that's not what we need.

Bits 12 and 11 if the FPU control word determine the way the FPU 
rounds numbers. The four rounding states are:

00 = round to nearest (this is the default)
01 = round down (towards -infinity)
10 = round up(towards +infinity)
11 = truncate up(towards zero)

The status word is loaded/stored using the fldcw/fstcw instructions. 
After setting the rounding mode as desired, the float2int() function 
will use that rounding mode until the control mode is reset. The ceil() 
and floor() functions set the rounding mode for every instruction, 
which is very slow. Therefore, it is a lot faster to set the rounding 
mode (down or up) before processing a block, and use float2int() instead.

References: SIMPLY FPU by Raymond Filiatreault
http://www.website.masmforum.com/tutorials/fptute/index.html

MSVC (and probably other compilers too) has functions defined in <float.h> 
for changing the FPU control word: _control87/_controlfp. The equivalent 
instructions are:

_controlfp(_RC_CHOP, _MCW_RC); // set rounding mode to truncate
_controlfp(_RC_UP _MCW_RC); // set rounding mode to up (ceil)
_controlfp(_RC_DOWN, _MCW_RC); // set rounding mode to down (floor)
_controlfp(_RC_NEAR, _MCW_RC); // set rounding mode to near (default)

Note that the FPU rounding mode affects other calculations too, so the 
same code may give different results.

Alternatively, single precision floating point numbers can be truncated or 
rounded to integers by using SSE instructions cvtss2si, cvttss2si, cvtps2pi 
or cvttps2pi, where SSE instructions are available (which means probably on 
all modern CPUs). These are not discussed here in detail, but I provided the 
function truncateSSE which always truncates a single precision floating point 
number to integer, regardless of the current rounding mode. 

(Also I think the SSE rounding mode can differ from the rounding mode set 
in the FPU control word, but I haven't tested it so far.)
*/
//////////////////////////////////////////////////////////////////////////

static short control_word;
static short control_word2;

// ������ת������ ע�⣺Ĭ�ϵ�fistp����������ģʽ����Ҫͨ������_control87(_MCW_RC, _RC_DOWN);���е���
// ע������ඨ��ɺ�ÿ�к��治�ܼӣ�����ÿһ�ж��ü�һ��__asm
#define FLOAT_TO_INT(f,i) __asm \
{ \
	__asm fld   f \
	__asm fistp i \
}


// round a float to int using the actual rounding mode 
inline int float2int(float x) {
    int i;
    __asm {
        fld x
        fistp i
    }
    return i;
}

// set rounding mode to nearest
inline void set_round2near() {
    __asm      {
        fstcw    control_word        // store fpu control word 
        mov     dx, word ptr [control_word]
        and        dx,0xf9ff            // round towards nearest (default)
        mov        control_word2, dx
        fldcw    control_word2        // load modfied control word 
    }
}

// set rounding mode to round down
inline void set_round_down() {
    __asm  {
        fstcw    control_word        // store fpu control word 
        mov     dx, word ptr [control_word]
        or        dx,0x0400            // round towards -inf
        and        dx,0xf7ff            
        mov        control_word2, dx
        fldcw    control_word2        // load modfied control word 
    }
}

// set rounding mode to round up
inline void set_round_up() {
    __asm  {
        fstcw    control_word        // store fpu control word 
        mov     dx, word ptr [control_word]
        or        dx,0x0800            // round towards +inf
        and        dx,0xfbff            
        mov        control_word2, dx
        fldcw    control_word2        // load modfied control word 
    }
}

// set rounding mode to truncate
inline void set_truncate() {
    __asm  {
        fstcw    control_word        // store fpu control word 
        mov     dx, word ptr [control_word]
        or        dx,0x0c00            // rounding: truncate
        mov        control_word2, dx
        fldcw    control_word2        // load modfied control word 
    }
}

// restore original rounding mode
inline void restore_cw() {
    __asm fldcw    control_word
}

// truncate to integer using SSE
inline long truncateSSE(float x) {
    __asm cvttss2si eax,x
}

#endif
