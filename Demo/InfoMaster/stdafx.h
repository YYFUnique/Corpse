#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include <atlstr.h>
#include "DuiLib/UIlib.h"
using namespace DuiLib;

#define WM_ENCRYPT_PROGRESS_POSITION			(WM_USER+0x1000)		//�ͻ��������ļ��������
#define WM_ENCRYPT_SUCCESS								(WM_USER+0x1001)		//�ͻ��������ļ�����ɹ�
