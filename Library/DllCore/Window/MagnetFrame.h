#pragma once

#include "../Utils/DuiList.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef enum tagATTCH_MODE
{
	ATTCH_MODE_NULL		= 0,
	ATTCH_MODE_LEFT		=	1,
	ATTCH_MODE_TOP			=	2,
	ATTCH_MODE_RIGHT		=	3,
	ATTCH_MODE_BOTTOM= 4,
}ATTCH_MODE;

typedef enum tagATTCH_ALIGN
{
	ATTCH_ALIGN_NULL		= 0,
	ATTCH_ALIGN_LEFT		=	1,
	ATTCH_ALIGN_TOP			=	2,
	ATTCH_ALIGN_RIGHT		= 3,
	ATTCH_ALIGN_BOTTOM= 4,
}ATTCH_ALIGN;

typedef struct tagATTCH_INFO
{
	int		nDist;
	ATTCH_MODE AttchMode;
	ATTCH_ALIGN	AttchAlign;
}ATTCH_INFO,*PATTCH_INFO;

typedef struct tagWND_DATA
{
	HWND				hWnd;
	WNDPROC		pProc;
	ATTCH_INFO	AttchInfo;
	BOOL				bNewPos;
	BOOL				bSubWnd;
}WND_DATA,*PWND_DATA;

typedef CDuiList<WND_DATA,WND_DATA&> CWndDataMap;

class DLL_API CMagnetFrame
{
protected:
	CMagnetFrame();
	~CMagnetFrame();

public:
	/************************************************************************/
	/* ��  ��  ����Initialize						                                                           */
	/* �������ܣ���ʼ������ģ�����ʹ��ģ��ǰ������ã������ܲ�����  */
	/* �����������																						   */
	/* ��  �� ֵ�����󴴽��ɹ�����TRUE�����򷵻�FALSE								   */
	/************************************************************************/
	static BOOL Initialize();

	static CMagnetFrame* GetInstance();
	/************************************************************************/
	/* ��  ��  ����SetMainWnd				                                                           */
	/* �������ܣ���������������																	   */
	/* ���������hWnd �����ھ��																   */
	/* ��  �� ֵ����																						   */
	/************************************************************************/
	void SetMainWnd(HWND hWnd);

	/************************************************************************/
	/* ��  ��  ����AddSubWnd		                                                                  */
	/* �������ܣ����һ�������Ӵ���																   */
	/* ���������hWnd ��Ҫ�Ƴ����Ӵ���														   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	BOOL AddSubWnd(HWND hWnd, ATTCH_MODE AttchMode = ATTCH_MODE_NULL, ATTCH_ALIGN AttchAlign = ATTCH_ALIGN_NULL, int nDist = 0);

	/************************************************************************/
	/* ��  ��  ����RemoveSubWnd                                                                  */
	/* �������ܣ��Ƴ�һ�������Ӵ���																   */
	/* ���������hWnd ��Ҫ�Ƴ����Ӵ���														   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	BOOL RemoveSubWnd(HWND hWnd);

protected:
	BOOL SubclassWnd(HWND hWnd, WND_DATA* pWndData);
	BOOL UnsubclassWnd(HWND hWnd);

	ATTCH_INFO CalcAttchMode(HWND hSubWnd, LPRECT rcWnd);
	void UpdateSubWndPos(HWND hSubWnd, const ATTCH_INFO* pAttchInfo);

protected:
	int CalcDistance(int nV1, int nV2);
	BOOL IsNeighbour(int nV1, int nV2);
	static LRESULT CALLBACK MagnetWndProc(HWND hWnd, UINT uMsg, WPARAM wParma, LPARAM lParam);

protected:
	HWND m_hMainWnd;		//����������
	static CMagnetFrame* m_pMagnet;
	CWndDataMap m_SubWndProc;
};