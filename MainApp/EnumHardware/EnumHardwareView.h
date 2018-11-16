
// EnumHardwareView.h : CEnumHardwareView 类的接口
//


#pragma once
#include "LsCommon/Class/Hardware.h"
typedef struct tgaDevName 
{
	CString strDevName;
	GUID DevGuid;
	UINT  DevIcon;
}DEVNAME,*PDEVNAME;

typedef CList<DEVNAME,DEVNAME&>CDevNameList;

//设备状态
#define DEV_DISABLE				0 //设备处于禁用状态
#define DEV_ENABLE				1 //设备处于启用状态
#define DEV_PROBLEM			2 //设备存在问题
#define DEV_NOT_EXIST		3 //设备不存在

//设备操作
#define DEV_OP_NONE				0 //设备无操作
#define DEV_OP_DISABLE			1 //禁用设备
#define DEV_OP_ENABLE			2 //启用设备
#define DEV_OP_RESET			3 //重启设备
#define	DEV_OP_REVERSE			4 //设备状态取反

#define MICROSOFT_BLUETOOTH		1//微软默认蓝牙
#define THIRD_BLUETOOTH			2//第三方蓝牙

//外设管理类
typedef struct _Outer_Dev_Manager
{
	CString strDevClassName;	//规则中的外设名称
	CString strDevClassDisplayName;	//这类设备的显示名称
	GUID	DevChangeGuid;		//注册硬件变化通知消息时用的GUID
	GUID	DevClassGuid;	//枚举硬件时枚举硬件时用的GUID
	DWORD	dwSubClass;		//子类别,目前主要用于区分串并口
	DWORD	dwOperator;		//操作类型
}OUTER_DEV_MANAGER,*POUTER_DEV_MANAGER;
typedef CArray<OUTER_DEV_MANAGER,OUTER_DEV_MANAGER&> COuterDevManagerArray;

class CEnumHardwareView : public CTreeView
{
protected: // 仅从序列化创建
	CEnumHardwareView();
	DECLARE_DYNCREATE(CEnumHardwareView)

// 属性
public:
	CEnumHardwareDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CEnumHardwareView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bShowAll;
	CDevInfoList m_DevInfoList;
	CDevNameList m_DevNameList;
	CImageList m_ImgList;
// 生成的消息映射函数
protected:
	afx_msg LRESULT OnShowHardware(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHdr,LPARAM* lResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowall();
	afx_msg void OnDonotShowHide();
	afx_msg void OnHardwareScanf();
	afx_msg void OnHardwareProperty();
	afx_msg void OnHardwareEnable();
	afx_msg void OnHardwareDisable();

protected:
	BOOL SetDeviceStatus(OUTER_DEV_MANAGER& DeviceRuleManage);
	DWORD GetPhysicalDiskIndex(const DEV_INFO& DeviceInfo);
	BOOL GetDevInfoFromClassGuid(IN GUID DevClassGuid, OUT CDevInfoList& DevInfoList,BOOL bAllDevice);
};

#ifndef _DEBUG  // EnumHardwareView.cpp 中的调试版本
inline CEnumHardwareDoc* CEnumHardwareView::GetDocument() const
   { return reinterpret_cast<CEnumHardwareDoc*>(m_pDocument); }
#endif

