#pragma once

typedef enum tagWIZARD_ID
{
	WIZARD_ID_TASK		=	0,
	WIZARD_ID_NET			=	1,
}WIZARD_ID;

typedef struct tagNTCHDR
{	
	WIZARD_ID	nWizardId;			// 顶部导航名称
	CString			strTabFrom;			// 来自那个标签项
	CString			strTabTo;				// 切换到标签项的名称
	CString			strData;				// 用Json构造的数据
}NTCHDR,*PNTCHDR;

typedef const PNTCHDR PCNTCHDR;