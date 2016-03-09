#pragma once


// Required for VS 2008 (fails on XP and Win2000 without this fix)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <Windows.h>
#include <WinDef.h>
#include <atlstr.h>
#include <olectl.h>
