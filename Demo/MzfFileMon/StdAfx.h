#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include <atlstr.h>

#include <DrvControl/DrvControl.h>

#include <DuiLib/UIlib.h>
using namespace DuiLib;

#include "MacroDefine.h"
#include "FileMonlib.h"
#include "Utils/ControlBuilder.h"

#define	DUI_MSGTYPE_REFRESH		_T("refresh")
#define	DUI_MSGTYPE_LOADITEM	_T("loaditem")

//#include "RichText/IRichTextView.h"
//using namespace RichText;

//#ifndef __POSITION
//typedef struct __POSITION
//{
//}__POSITION,*POSITION;
//#endif
#define MIN min
#define MAX max