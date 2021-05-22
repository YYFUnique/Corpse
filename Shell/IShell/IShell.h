#pragma once

#ifdef LS_STATIC_LIB_CALL
#define SHELL_API 
#elif defined  SHELL_EXPORTS
#define SHELL_API __declspec(dllexport)
#else
#define SHELL_API __declspec(dllimport)
#endif

#include "Define.h"
#include "Utils.h"
#include "UIManager.h"
#include "WindowImplBase.h"