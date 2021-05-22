#pragma once

// 桌面窗口自定义消息，用于“显示桌面”
#define DTM_RAISE                       (WM_USER + 83)
// 下列值是该消息号，对应的LPARAM参数
#define DTRF_RAISE		  0			// 显示桌面
#define DTRF_LOWER      1				// 隐藏桌面，显示应用
#define DTRF_QUERY       2			// 查询状态

typedef enum tagTOGGLEDESKTOP
{
	DESKTOP_PLUGIN_WORKING	= 0,	// 桌面消息插件是否在运行
	DESKTOP_PLUGIN_COUNT			=	1,	// 获取桌面插件引用计数
	DESKTOP_PLUGIN_ADDREF		=	2,	// 增加桌面插件引用计数
};

// 自定义“显示桌面”消息，用于转发“显示桌面”消息
#define TOGGLEDESKTOP					_T("ToggleDesktop")
// 自定义“显示桌面”消息，用于获取或设置插件工作状态
#define TOGGLEDESKTOPMSG			_T("GetToggleDesktopMsg")
// 自定义“卸载插件”消息，用于卸载注入进程中的插件
#define FREELIBRARYANDEXIT			_T("FreeLibraryAndExitThread")