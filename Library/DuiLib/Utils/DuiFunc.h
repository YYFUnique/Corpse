#pragma once

namespace DuiLib
{
	class CWindowWnd;
	class UILIB_API CDuiLibMgr
	{
		/*friend CDuiLibMrg& GetDuiLibMgr();*/
		public:
			CDuiLibMgr();

			void SetMainApp(CWindowWnd* pMainWnd);
			CWindowWnd* GetMainApp();
			void SetMainWndHandle(HWND hMainWnd);
			HWND GetMainWndHandle();
		protected:
			
			HWND m_hMainWnd;
			CWindowWnd* m_pMainWnd;
	};

	UILIB_API CDuiLibMgr& GetDuiLibMgr();
	UILIB_API CWindowWnd* GetMainApp();
}