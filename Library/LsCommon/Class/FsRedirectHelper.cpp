#include "stdafx.h"
#include "FsRedirectHelper.h"
#include "../OsInfo.h"

//========================================================================================================
// ˵��: 1. ������Ϊ�˷���64λ����ϵͳ���ظ����йر��ض������õĲ�����֧��Ƕ��ʹ�� 
//       2. �����������ʱ�����������Ὺ���ض������ã������ض������õ���Ч�ڽ����ڸ���������������
//       3. bSet:TRUE �ر��ļ�ϵͳ�ض���
// ����: exp1: CFsRedirectHelper Helper(TRUE); // �ر��ļ�ϵͳ�ض���,�����������������������
//       exp2: CFsRedirectHelper Helper(FALSE); // δ�ر��ļ�ϵͳ�ض���
//             Helper.DisableFsRedirect();
//             Helper.RevertFsRedirect();
//========================================================================================================
char CFsRedirectHelper::bIs64 = -1;
BOOL CFsRedirectHelper::bInited = FALSE;
CFsRedirectHelper::Wow64DisableWow64FsRedirection CFsRedirectHelper::f_Wow64DisableWow64FsRedirection = NULL;
CFsRedirectHelper::Wow64RevertWow64FsRedirection CFsRedirectHelper::f_Wow64RevertWow64FsRedirection = NULL;  

CFsRedirectHelper::CFsRedirectHelper(BOOL bSet) : m_bSet(bSet), m_pOldValue(NULL)
{
	if(bIs64==-1)
	{
		bIs64 = IsWow64Process() ? 1 : 0; 
	}

	if(bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
	}
}

CFsRedirectHelper::~CFsRedirectHelper()
{
	RevertFsRedirect();
}

BOOL CFsRedirectHelper::DisableFsRedirect()
{
	if(!m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
		m_bSet = TRUE;
	}

	return (m_pOldValue != NULL) ? TRUE:FALSE;
}

void CFsRedirectHelper::RevertFsRedirect()
{
	if(m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64RevertWow64FsRedirection(&m_pOldValue);
		m_bSet = FALSE;
	}
}

BOOL CFsRedirectHelper::Init()
{
	if(!bInited)
	{
		bInited = TRUE;

		HINSTANCE hlibrary = GetModuleHandle(_T("Kernel32.dll"));
		if(hlibrary)
		{
			f_Wow64DisableWow64FsRedirection = (Wow64DisableWow64FsRedirection) GetProcAddress(hlibrary,"Wow64DisableWow64FsRedirection");
			f_Wow64RevertWow64FsRedirection = (Wow64RevertWow64FsRedirection) GetProcAddress(hlibrary,"Wow64RevertWow64FsRedirection");
		}
	}
	return bInited;
}
