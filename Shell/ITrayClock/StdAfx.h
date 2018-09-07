#pragma once

#include <atlstr.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <windows.h>

#include "Define.h"
#include "UILabel.h"
#include "IMenuHandler.h"
#include "UIManager.h"
#include "WinImplBase.h"

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif