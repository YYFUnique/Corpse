#ifndef UITrayIcon_h__
#define UITrayIcon_h__

#pragma once
#include <ShellAPI.h>

namespace DuiLib
{
	class UILIB_API CUITrayPos
	{
	public:
		CUITrayPos();
		virtual ~CUITrayPos();

		void OnMouseMove();
		BOOL IsMouseHover();
		void SetTrackMouse(BOOL bTrackMouse);
		static UINT CALLBACK TrackMousePt(PVOID pvClass);
	protected:
		virtual VOID OnMouseHover() = 0;
		virtual VOID OnMouseLeave() = 0;
	private:
		POINT					m_ptMouse;
		HANDLE				m_hThread;
		HANDLE				m_hExitEvent;
		BOOL					m_bTrackMouse;
		CRITICAL_SECTION	m_cs;
	};

	class UILIB_API CUITrayIcon : public CUITrayPos
	{
	public:
		CUITrayIcon(void);
		~CUITrayIcon(void);

	public:
		void CreateTrayIcon( HWND _RecvHwnd,UINT _IconIDResource,LPCTSTR _ToolTipText = NULL,UINT _Message = NULL/*(UIEVENT_TRAYICON)*/);
		void DeleteTrayIcon();
		bool SetTooltipText(LPCTSTR _ToolTipText);
		bool SetTooltipText(UINT _IDResource);
		CDuiString GetTooltipText() const;

		bool SetIcon(HICON _Hicon);
		bool SetIcon(LPCTSTR _IconFile);
		bool SetIcon(UINT _IDResource);
		HICON GetIcon() const;
		void SetHideIcon();
		void SetShowIcon();
		void RemoveIcon();
		bool StartTwinkling();
		void StopTwinkling();

		bool Enabled(){return m_bEnabled;};
		bool IsVisible(){return !m_bVisible;};
		bool IsTwinkling(){return m_bTwinkling;};

		/*void OnTimer(IDuiTimer* pTimer);*/
	protected:
		//当鼠标进入系统托盘图标区域，调用该函数，同时向
		//主窗口托盘消息发送鼠标进入托盘区域消息
		virtual void OnMouseHover();
		//当鼠标离开系统托盘图标区域，调用该函数，同时向
		//主窗口托盘消息发送鼠标离开托盘区域消息，如果需要继续保持有效可以设置SetTrackMouse(TRUE)
		virtual void OnMouseLeave();
	private:
		bool m_bEnabled;
		bool m_bVisible;
		bool m_bTwinkling;
		HWND m_hWnd;
		UINT m_uID;
		UINT m_uMessage;
		HICON m_hIcon;
		NOTIFYICONDATA	m_trayData;
	};
}
#endif // UITrayIcon_h__

