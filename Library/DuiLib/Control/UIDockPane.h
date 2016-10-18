#pragma once

namespace DuiLib
{
	class CDockItemUI : public CButtonUI
	{
	public:
		CDockItemUI();
		~CDockItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		/*
		 *	@brief 重写DoEvent来修改对MOUSEMOVE和MOUSELEAVE消息的响应
		 */
		void DoEvent(TEventUI& event);

		void SetPos(RECT rc);

	private:
	};

	typedef enum tagDOCKPANE_POSITION
	{
		DOCKPANE_POSITION_LEFT			= 0,				//位置左
		DOCKPANE_POSITION_TOP			= 1,				//位置上
		DOCKPANE_POSITION_RIGHT		= 2,				//位置右
		DOCKPANE_POSITION_BOTTOM   = 3,				//位置下
	}DOCKPANE_POSITION;

	class CDockPaneUI : public CHorizontalLayoutUI
	{
	public:
		CDockPaneUI(void);
		~CDockPaneUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		/*
		 *	@brief 只添加CDockItem控件
		 */
		bool Add(CControlUI* pControl) ;

		/*
		 *	@brief 重写SetPos函数，让内部子控件默认底端排列而不是顶端排列，并且去掉不必要的代码
		 */
		void SetPos(RECT rc) ;

		/*
		 *	@brief 为SetItemSize、SetItemMaxRate、SetChangeRange增加对应的属性
		 */
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) ;
		void DoEvent(TEventUI& event) ;
		
		/*
		 *	@param sz:设置/获取每一个子控件的大小，默认 60x60
		 */
		void SetItemSize(SIZE sz);
		SIZE GetItemSize() const;

		/*
		 *	@param dRate:设置/获取子控件最大变化比例，默认 2.0
		 */
		void SetItemMaxRate(double dRate);
		double GetItemMaxRate() const;

		/*
		 *	@param nNum:设置/获取子控件缩放影响的范围，单位是控件的个数，默认3，则影响最大子控件的左右3个控件大小
		 */
		void SetChangeRange(int nNum);
		int	 GetChangeRange() const;


		/*
		 *	@param szPositon:设置/获取子控件的位置，默认为bottom
		 */
		void SetPositon(DOCKPANE_POSITION DockPanePosition);
		DOCKPANE_POSITION GetPositon() const;

		/*
		 *  @brief 子控件用来通知DockPane的接口，通知我们鼠标进入或者离开某个子控件
		 *	@param pItem:当前鼠标所在的子控件的指针
		 *	@param dRate:当前子控件中鼠标的x位置占整个控件的的宽度的比例
		 */
		void NotifyEnterItem(CDockItemUI* pItem, double dXRate, double dYRate);
		void NotifyLeaveItem(CDockItemUI* pItem);

	private:
		SIZE	m_szItemSize ;
		double	m_dbMaxRate;
		int		m_nChangeRange;
		DOCKPANE_POSITION m_DockPane;
	};
}
