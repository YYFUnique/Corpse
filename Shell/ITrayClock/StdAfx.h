#pragma once

#include <atlstr.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <windows.h>
#include <crtdbg.h> 
#include <vssym32.h>
#include "Define.h"
#include "ThemeHelper.h"
#include "UILabel.h"
#include "IMenuHandler.h"
#include "IShell/IShell.h"

using namespace IShell;

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif