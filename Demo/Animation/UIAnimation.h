#pragma once
#include "UiFeatureEffect/Interface/IUIEffect.h"
#include "CDrawingBoard.h"

class CAnimationUI : public WindowImplBase,public IUIEffectCallBack
{
public:
	CAnimationUI();
	~CAnimationUI();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	virtual void OnUiEffectBegin(WPARAM effectKey, DWORD animaType);
	virtual void OnUiEffectEnd(WPARAM effectKey, DWORD animaType);
	virtual void OnUiEffectDraw();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	LRESULT OnShow(WPARAM wParam,LPARAM lParam);
	virtual void OnClick(TNotifyUI& msg);
	void OnShowActiveX(TNotifyUI& msg);
private:
	CDrawingBoard* m_pDraw;
	IUIEffect* m_pEffect;
	UINT m_nCount;
};