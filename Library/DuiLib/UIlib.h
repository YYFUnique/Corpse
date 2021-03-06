// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted provided that the 
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials 
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef LS_STATIC_LIB_CALL
#define UILIB_API 
#elif defined  UILIB_EXPORTS
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API __declspec(dllimport)
#endif


#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <shellapi.h>
#include <atlstr.h>
#ifdef RENDER_GDIPLUS
#include "Utils/GraphicsRoundRectPath.h"
#endif

#include "Utils/Utils.h"
#include "Utils/UIDelegate.h"
#include "Utils/UIDropDrag.h"
#include "Utils/DPI.h"
#include "Core/UIDefine.h"
#include "Core/UIManager.h"
#include "Core/UIBase.h"
#include "Core/UIMarkup.h"
#include "Core/UIDataBuilder.h"
#include "Utils/DuiFunc.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UIDlgBuilder.h"
#include "Core/ImageAttribute.h"
#include "Core/UIRender.h"

#include "Utils/EditTextHost.h"
#include "Utils/WinTextHost.h"
#include "Utils/WinImplBase.h"
#include "Utils/UITrayIcon.h"
#include "Effect/UIAnimation.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"
#include "Layout/UIGroupLayout.h"
#include "Layout/UIAnimationTabLayout.h"
#include "Layout/UISplitLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"
#include "Control/UIChartView.h"

#include "Control/UILabel.h"
#include "Control/RollTextUI.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/EditUI2.h"

#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"
#include "Control/UICalendar.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"
#include "Control/UISliderRangeUI.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
//#include "Control/SMRichEdit.h"
#include "Control/UIDateTime.h"
#include "Control/RichEditEx.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIWkeBrowser.h"
#include "Control/UIWinHex.h"
#include "Control/UIFlash.h"
/*#include "Control/UIPicture.h"*/
/*#include "Control/UICrack.h"*/
#include "Control/UIMenu.h"
#include "Control/UIColorPalette.h"
#include "Control/UIIPAddress.h"
#include "Control/UIDockPane.h"
#include "Control/UIPage.h"
#include "Control/UIRectTracker.h"

#include "Control/UIPicture.h"
#include "Control/UIFrame.h"