#pragma once

#include "Image3D/3dTransform.h"
using namespace IMAGE3D;

class C3DViewUI : public CVerticalLayoutUI
{
public:
	C3DViewUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoPaint(HDC hDC,const RECT& rcPaint);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void StartEffect(BOOL bStart);
	void Update();
	void SetRotateX(int nRotateX);
	void SetRotateY(int nRotateY);
	void SetRotateZ(int nRotateZ);
	void SetOffsetZ(int nOffsetZ);
	void SetBmpOrig(HBITMAP hBmpOrig);
	PARAM3DTRANSFORM& Get3dParam();

protected:
	HBITMAP			m_hBmpOrig;
	HBITMAP			m_hBmpTrans;
	BOOL				m_bPlay;
	HDC					m_hMemDC;
	CDuiString		m_strViewImage;
	PARAM3DTRANSFORM	m_3dParam;
};