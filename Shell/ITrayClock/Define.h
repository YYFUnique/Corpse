#pragma once

#define WM_TCLOCK_RESVERED1		(WM_USER + 0x64)		// 未知保留中
#define WM_TCLOCK_LBUTTON		(WM_USER + 0x66)		// 任务栏时钟区域点击鼠标左键
#define WM_TCLOCK_TOOLTIP			(WM_USER + 0x67)		// 任务栏时钟显示ToolTip
#define WM_TCLOCK_HIGHLIGHT		(WM_USER + 0x68)		// 任务栏时钟背景高亮

typedef enum tagTCLOCK_HIGHLIGHT_WPARAM
{
	TCLOCK_DISABLE	= 1,
	TCLOCK_ENABLE		= 2,
	TCLOCK_UNKNOW = 3,
}TCLOCK_HIGHLIGHT_WPARAM;

typedef enum tagMENU_ITEM
{
	IDM_ITEM_TEST1		= 0,
	IDM_ITEM_TEST2		= 1,
	IDM_ITEM_SEP			=	2,
	IDM_ITEM_ENABLE		= 3,
	IDM_ITEM_DISABLE	=	4,
	IDM_ITEM_EXIT			=	5,
}MENU_ITEM;