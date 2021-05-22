#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "DuiLib/UIlib.h"
using namespace DuiLib;

#include "Define.h"
#include "Control/UIFileList.h"
#include "Utils/IMarkup.h"
#include "Utils/ProjectFileManager.h"
#include "Core/ProjectFileMarkup.h"
#include "Core/ControlBuilder.h"
#include "Core/ProjectFileBuilder.h"
#include "Utils/ProjectFileInfo.h"
#include "Wnd/AboutDialog.h"
#include "Utils/History.h"
//#ifndef __POSITION
//typedef struct __POSITION
//{
//}__POSITION,*POSITION;
//#endif

#define MAX max
#define MIN min