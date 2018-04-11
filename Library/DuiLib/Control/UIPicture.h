/************************************************************************/
//图片可以显示多层图标实现图片的叠加 我这里只是设置了 背景图和前景图两种 有需求的可以自己添加
/************************************************************************/
#pragma once
namespace DuiLib
{
	class UILIB_API CPictureUI : public CControlUI
	{
	public:
		CPictureUI(void);
		~CPictureUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetIcon(HICON hIcon);

		void SetForeImage( LPCTSTR pStrImage );
		void PaintStatusImage(HDC hDC);

	protected:
		HICON			m_hIcon;
		CDuiString m_strResourcePath;
	};
}