#include "stdafx.h"
#include "OSBootMenu.h"
#include "../CommonFunc.h"
#include "../Class/Register.h"
#include "../OsInfo.h"
#include "../Class/Process.h"
#include "../TextTools.h"
#include "../FileTools.h"
#include <Shlwapi.h>

#define	BOOT_MENU_DIRECTORY_NAME					_T("BootMenu")
#define	BOOT_MENU_OS_NAME								_T("operating systems")
#define	BOOT_MENU_BOOT_LOAD							_T("boot loader")
#define	BOOT_MENU_DEFAULT_OS_NAME				_T("default")
#define   BOOT_MENU_DEFAULT_MANAGER_GUID		_T("{9dea862c-5cdd-4e70-acc1-f32b344d4795}")	
#define   BOOT_MENT_DEFAULT_KEY_NAME				_T("23000003")
#define	BOOT_MENU_OS_LIST_KEY							_T("24000001")
#define	BOOT_MENU_OS_LIST_VALUE_NAME			_T("Element")
#define	BOOT_MENU_REG_BAK_VALUE_NAME			_T("ElementBak")

#define	BOOT_MENU_BCD_KEY_NAME						_T("BCD00000000")

COSBootMenu::COSBootMenu()
{
	m_bIsVista = OsIsVistaOrLater();
}

COSBootMenu::~COSBootMenu()
{

}

BOOL COSBootMenu::ChangeOsBCDMenu()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_bIsVista != FALSE)
		{
			if (ModifyVistaBootMenu() == FALSE)
				break;
		}
		else
		{
			if (BackupNtBootMenu() == FALSE)
			{
				SetErrorTitle(_T("����NTϵͳBoot.iniʧ��"));
				WriteErrorInfoToLogFile(GetThreadErrorInfoString());
			}

			if (ModifyNtBootMenu() == FALSE)
			{
				SetErrorTitle(_T("�޸�NTϵͳboot.iniʧ��"));
				WriteErrorInfoToLogFile(GetThreadErrorInfoString());
			}

			if (ModifyBootMenuByReg() == FALSE)
			{
				SetErrorTitle(_T("��NTϵͳ���޸�BCDע�����Ϣʧ��"));
				WriteErrorInfoToLogFile(GetThreadErrorInfoString());
			}
		}

		bSuccess = TRUE;
	} while (FALSE);
	return bSuccess;
}

BOOL COSBootMenu::ModifyVistaBootMenu()
{
	BOOL bSuccess = FALSE;
	do 
	{
		//��ȡ����ϵͳGUID�б�
		CStringArray strOsListArray;
		if (GetOsListGUIDString(strOsListArray) == FALSE)
			break;

		if (strOsListArray.GetSize() == 1)
		{
			bSuccess = TRUE;	//��������˵�ֻ��1�˵���Ѿ��������
			break;
		}

		CString strOutput;
		LPCTSTR lpszBcdEditPath = _T("C:\\Windows\\System32\\bcdedit.exe");
		if (RedirectionOutput(lpszBcdEditPath,NULL,1000,strOutput) == FALSE)
			break;

		CString strDefaultOsGuid;
		GetDefaultOsGuid(strDefaultOsGuid);

		CString strResveredGuid;
		LPCTSTR lpszFilter[] = {_T("{default}"),_T("{ntldr}")};	//legacy ??
		LPCTSTR lpszFilterValue[] = {strDefaultOsGuid,_T("{466f5a88-0af2-4f76-9038-095b170dc21c}")};
		for (int i=0;i<_countof(lpszFilter);++i)
			strOutput.Replace(lpszFilter[i],lpszFilterValue[i]);

		for (int i=0;i<strOsListArray.GetSize();++i)
		{
			CString& strOsGuid = strOsListArray.ElementAt(i);
			int nIndex = strOutput.Find(strOsGuid);
			if (nIndex == -1)
			{
				strResveredGuid = strOsGuid;
				break;
			}
		}

		if (strResveredGuid.IsEmpty() != FALSE)
		{
			WriteErrorInfoToLogFile(_T("�޸�ϵͳ�����˵�û���ҵ���ǰϵͳ��GUID"));
			break;
		}

		bSuccess = DisableOtherBootMenu(strResveredGuid);
	} while (FALSE);

	return bSuccess;
}

BOOL COSBootMenu::BackupNtBootMenu()
{
	BOOL bSuccess = FALSE;
	do 
	{
		TCHAR szBootMenuPath[MAX_PATH];
		GetCommonAppDataFilePath(BOOT_MENU_DIRECTORY_NAME,_T(""),szBootMenuPath);

		TCHAR szBackBootMenuName[MAX_PATH];
		PathCombine(szBackBootMenuName,szBootMenuPath,_T("boot.ini"));

		//������ھͲ���Ҫ������
		if (PathFileExists(szBackBootMenuName))
		{
			bSuccess = TRUE;
			break;
		}

		TCHAR szCurrentOsPath[MAX_PATH],szCurrentBootPath[MAX_PATH];
		GetSystemWindowsDirectory(szCurrentOsPath,_countof(szCurrentOsPath));
		//��ȡ��Ŀ¼
		PathStripToRoot(szCurrentOsPath);

		PathCombine(szCurrentBootPath,szCurrentOsPath,_T("boot.ini"));
		if (CopyFile(szCurrentBootPath,szBackBootMenuName,FALSE) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("���������˵�������Ŀ¼ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL COSBootMenu::ModifyNtBootMenu()
{
	BOOL bSuccess = FALSE;
	do 
	{
		TCHAR szCurrentOsPath[MAX_PATH],szCurrentBootPath[MAX_PATH];
		GetSystemWindowsDirectory(szCurrentOsPath,_countof(szCurrentOsPath));
		PathStripToRoot(szCurrentOsPath);

		PathCombine(szCurrentBootPath,szCurrentOsPath,_T("boot.ini"));
		DWORD dwFileAttri = GetFileAttributes(szCurrentBootPath);
		dwFileAttri &= ~FILE_ATTRIBUTE_READONLY;
		//ȥ��boot.ini�ļ���ֻ������
		SetFileAttributes(szCurrentBootPath,dwFileAttri);

		TCHAR szOSKeyName[1024];
		GetPrivateProfileString(BOOT_MENU_OS_NAME,NULL,NULL,szOSKeyName,_countof(szOSKeyName),szCurrentBootPath);
		LPCTSTR lpszOSKeyName = szOSKeyName;
		CString strDriverLetter;
		CString strOsPathDriverLetter(szCurrentOsPath);
		while(*lpszOSKeyName)
		{
			ConvertBootMenuStringToDriverLetter(lpszOSKeyName,strDriverLetter);
			if (strDriverLetter.CompareNoCase(strOsPathDriverLetter.TrimRight(_T("\\"))) != 0)
				WritePrivateProfileString(BOOT_MENU_OS_NAME,lpszOSKeyName,NULL,szCurrentBootPath);
			else
				WritePrivateProfileString(BOOT_MENU_BOOT_LOAD,BOOT_MENU_DEFAULT_OS_NAME,lpszOSKeyName,szCurrentBootPath);
			lpszOSKeyName += _tcslen(lpszOSKeyName) + 1;
		}

		dwFileAttri |= FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(szCurrentBootPath,dwFileAttri);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL COSBootMenu::ModifyBootMenuByReg()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (LoadVistaBCDMenu() == FALSE)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("����BCDע���ʧ��"));
			break;
		}

		LPCTSTR lpszNtldrGUID = _T("{466f5a88-0af2-4f76-9038-095b170dc21c}");
		bSuccess = DisableOtherBootMenu(lpszNtldrGUID);
		RegUnLoadKey(HKEY_LOCAL_MACHINE , BOOT_MENU_BCD_KEY_NAME);
	} while (FALSE);	

	return bSuccess;
}

BOOL COSBootMenu::GetOsListGUIDString(CStringArray& strOsListGuid)
{
	HKEY hKey = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		RunFunctionAsSystemUser();
		CString strOsListRegPath;
		strOsListRegPath.Format(_T("%s\\%s\\Elements\\%s"),_T("BCD00000000\\Objects"),BOOT_MENU_DEFAULT_MANAGER_GUID,BOOT_MENU_OS_LIST_KEY);
		DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,strOsListRegPath,NULL,KEY_READ,&hKey);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б���ע���ʧ��"));
			break;
		}

		TCHAR szOsList[1024*10];
		DWORD dwSize = _countof(szOsList);
		dwRet = RegQueryValueEx(hKey,BOOT_MENU_OS_LIST_VALUE_NAME,NULL,NULL,(LPBYTE)szOsList,&dwSize);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б�ʧ��"));
			break;
		}

		AnalyseDbNullTerminatedText(szOsList,strOsListGuid);
		bSuccess = TRUE;
	} while (FALSE);

	if (hKey)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL COSBootMenu::GetDefaultOsGuid(CString& strDefaultOsGuid)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		RunFunctionAsSystemUser();
		CString strOsListRegPath;
		strOsListRegPath.Format(_T("%s\\%s\\Elements\\%s"),_T("BCD00000000\\Objects"),BOOT_MENU_DEFAULT_MANAGER_GUID,BOOT_MENT_DEFAULT_KEY_NAME);
		DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,strOsListRegPath,NULL,KEY_READ|KEY_WRITE,&hKey);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б���ע���ʧ��"));
			break;
		}

		TCHAR szOsList[1024*10];
		DWORD dwSize = _countof(szOsList);
		dwRet = RegQueryValueEx(hKey,BOOT_MENU_OS_LIST_VALUE_NAME,NULL,NULL,(LPBYTE)szOsList,&dwSize);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б�ʧ��"));
			break;
		}

		strDefaultOsGuid = CString(szOsList,dwSize);

		bSuccess = TRUE;
	} while (FALSE);

	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL COSBootMenu::DisableOtherBootMenu(LPCTSTR lpszResveredKey)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		RunFunctionAsSystemUser();
		CString strOsListRegPath;
		strOsListRegPath.Format(_T("%s\\%s\\Elements\\%s"),_T("BCD00000000\\Objects"),BOOT_MENU_DEFAULT_MANAGER_GUID,BOOT_MENU_OS_LIST_KEY);
		DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,strOsListRegPath,NULL,KEY_READ|KEY_WRITE,&hKey);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б���ע���ʧ��"));
			break;
		}

		//����Ƿ��Ѿ��б��ݣ������������Ҫ�ٴα��ݣ������ľ���洢��ֵ
		//���سɹ���ʾ�Ѿ��иü�ֵ
		CString strOsListBak;
		if (LsRegQueryValue(hKey,BOOT_MENU_REG_BAK_VALUE_NAME,strOsListBak) == TRUE)
			break;

		TCHAR szOsList[1024*10];
		DWORD dwSize = _countof(szOsList);
		dwRet = RegQueryValueEx(hKey,BOOT_MENU_OS_LIST_VALUE_NAME,NULL,NULL,(LPBYTE)szOsList,&dwSize);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("��ȡ����ϵͳ�б�ʧ��"));
			break;
		}

		dwRet = RegSetValueEx(hKey,BOOT_MENU_REG_BAK_VALUE_NAME,NULL,REG_MULTI_SZ, (LPBYTE)szOsList,dwSize);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("���ݲ���ϵͳ�����˵��б�ʧ��"));
			break;
		}

		dwRet = RegSetValueEx(hKey,BOOT_MENU_OS_LIST_VALUE_NAME,NULL,REG_MULTI_SZ, (LPBYTE)lpszResveredKey,(_tcslen(lpszResveredKey)+1)*sizeof(TCHAR));
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwRet,_T("�޸Ĳ���ϵͳ�����˵��б�ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hKey != NULL)
		RegCloseKey(hKey);
	return bSuccess;
}

BOOL COSBootMenu::LoadVistaBCDMenu()
{
	BOOL bSuccess = FALSE;
	do 
	{
		HKEY hKey = NULL;
		DWORD dwRet = RegOpenKey(HKEY_LOCAL_MACHINE,BOOT_MENU_BCD_KEY_NAME,&hKey);
		if (dwRet == ERROR_SUCCESS)
		{	//����Ѿ�ע������Ѿ�������BCD�Ͳ��ü�����
			RegCloseKey(hKey);
			bSuccess = TRUE;
			break;
		}

		if (AdjustProcessAuthority(SE_RESTORE_NAME) == FALSE)
		{
			SetErrorTitle(_T("���ý��̾���%s��Ȩʧ��"),SE_RESTORE_NAME);
			break;
		}

		if (AdjustProcessAuthority(SE_BACKUP_NAME) == FALSE)
		{
			SetErrorTitle(_T("���ý��̾���%s��Ȩʧ��"),SE_BACKUP_NAME);
			break;
		}

		DWORD dwLoadKey = ERROR_FILE_NOT_FOUND;
		TCHAR szBCDLoaderPath[MAX_PATH];
		for (DWORD dwIndex = 1;dwIndex<=32;++dwIndex)
		{
			_stprintf_s(szBCDLoaderPath,_countof(szBCDLoaderPath),_T("\\\\?\\GLOBALROOT\\Device\\HarddiskVolume%d\\boot\\BCD"),dwIndex);
			dwLoadKey = RegLoadKey(HKEY_LOCAL_MACHINE,BOOT_MENU_BCD_KEY_NAME,szBCDLoaderPath);
			//����ɹ����ؾͲ���Ҫ������
			if (dwLoadKey == ERROR_SUCCESS)
				break;
		}

		if (dwLoadKey == ERROR_SUCCESS)
			bSuccess = TRUE;

	} while (FALSE);

	return bSuccess;
}

BOOL COSBootMenu::ConvertBootMenuStringToDriverLetter(LPCTSTR lpszOSKeyName,CString& strDriverLetter)
{
	CString strOSKeyName(lpszOSKeyName);
	UINT nPartitionLeft = strOSKeyName.ReverseFind(_T('('));
	UINT nPartitionRight = strOSKeyName.ReverseFind(_T(')'));

	CString strPartition = strOSKeyName.Mid(nPartitionLeft+1,nPartitionRight-nPartitionLeft-1);

	strDriverLetter.Format(_T("\\Device\\HarddiskVolume%s"),strPartition);
	if (DosPathToNtPath(strDriverLetter,strDriverLetter) == FALSE)
		return FALSE;

	return TRUE;
}