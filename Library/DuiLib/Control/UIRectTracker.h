#pragma once

namespace DuiLib
{
	#define		DIRECTE_WNES			0	//西北-东南
	#define		DIRECTE_ENWS			1	//东北-西南
	#define		DIRECTE_ESWN			2	//东南-西北
	#define		DIRECTE_WSEN			3	//西南-东北
	#define		DIRECTE_NS				4	//正北-正南
	#define		DIRECTE_EW				5	//正东-正西
	#define		DIRECTE_SN				6	//正南-正北
	#define		DIRECTE_WE				7	//正西-正东
	#define		DIRECTE_ALL				8	//四面八方
	#define		DIRECTE_NOTHINE		9	//未定义

	typedef enum StyleFlags
	{
		solidLine = 1, dottedLine = 2, hatchedBorder = 4,
		resizeInside = 8, resizeOutside = 16, hatchInside = 32,
	}StyleFlags;

	typedef enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
		hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
	}TrackerHit;

	class UILIB_API CRectTrackerUI : public CLabelUI
	{
		public:
			CRectTrackerUI();

			LPCTSTR GetClass() const;
			LPVOID GetInterface(LPCTSTR pstrName);
			UINT GetControlFlags() const;
			void DoInit();

			void PaintBorder(HDC hDC);
			void DoEvent(TEventUI& event);
			void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		public:
			int NormalizeHit(int nHandle) const;
			int	GetHandleSize(LPCRECT lpRect = NULL) const;
			void GetTrueRect(LPRECT lpTrueRect) const;
			void GetHandleRect(int nHandle, LPRECT pHandleRect) const;
	
			int HitTest(POINT ptCursor) const;
			int HitTestHandles(POINT pt) const;
			BOOL SetCursor(HWND hWnd, UINT nHitTest);
			
			BOOL TrackHandle(int nHandle, HWND hWnd);
			BOOL Track(HWND hWnd, POINT pt, BOOL bAllowInvert);
			BOOL TrackRubberBand(HWND hWnd, POINT pt, BOOL bAllowInvert);
			void	GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py);

			// Overridables
			virtual UINT GetHandleMask() const;
			virtual void AdjustRect(int nHandle, LPRECT lpRect);
			virtual void DrawTrackerRect(LPCRECT lpRect, HDC hDC);
			virtual void OnChangedRect(const RECT& rectOld);
		protected:
			int m_nStyle;
			BOOL m_bErase;
			int m_nHandleSize;
			BOOL m_bFinalErase;
			BOOL m_bAllowInvert;

			CDuiRect m_rect;
			CDuiRect m_rectLast;
			CDuiSize m_sizeMin;
			CDuiSize m_sizeLast;
			static HCURSOR m_hCursors[10];
	};
}
