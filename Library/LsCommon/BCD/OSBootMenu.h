#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include <WbemIdl.h>

class LS_COMMON_API COSBootMenu
{
public:
	COSBootMenu();
	~COSBootMenu();
public:
	//�޸������˵�
	BOOL ChangeOsBCDMenu();

	//�޸�Vistaϵͳ���Ժ�ϵͳ�������˵�
	BOOL ModifyVistaBootMenu();

	//ͨ��ע����޸�ϵͳ�°汾�������˵�
	BOOL ModifyBootMenuByReg();

	//����NTϵͳ�����˵�boot.ini
	BOOL BackupNtBootMenu();

	//�޸�NTϵͳ�����˵���boot.ini
	BOOL ModifyNtBootMenu();
protected:
	//��ȡ����ϵͳ�����˵���GUID�ַ���
	BOOL GetOsListGUIDString(CStringArray& strOsListGuid);

	//��ȡ�����˵�Ĭ�ϲ���ϵͳGUID�ַ���
	BOOL GetDefaultOsGuid(CString& strDefaultOsGuid);

	BOOL DisableOtherBootMenu(LPCTSTR lpszResveredGuid);

	BOOL LoadVistaBCDMenu();

	BOOL ConvertBootMenuStringToDriverLetter(LPCTSTR lpszOSKeyName,CString& strDriverLetter);
protected:
	BOOL m_bIsVista;
};