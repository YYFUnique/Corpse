#pragma once

namespace DuiLib
{		
	class UILIB_API CFrameUI : public CLabelUI
	{
	public:
		CFrameUI();
		~CFrameUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void DoEvent(TEventUI& event);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	
		//执行帧序列动画
		void Run();
		//设置动画周期
		void SetPeriod(DWORD dwPeroid);
		//设置动画总共帧数
		void SetFrameCount(DWORD dwFrame);
		//设置帧序列动画图片
		void SetFrameImage(LPCTSTR lpszFrameImg);
	protected:
		DWORD				m_dwPeroid;					//一个周期时间，单位毫秒
		DWORD				m_dwTotal;						//每个周期需要多少帧
		DWORD				m_dwFrame;					//当前处于哪一帧
		POINT					m_ptTopLeft;					//左上角在帧序图片的坐标
		CDuiString			m_strFrameImg;
	};
}