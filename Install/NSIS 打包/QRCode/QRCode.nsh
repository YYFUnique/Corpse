
# ===================== �ⲿ����Լ��� =============================
!include "StrFunc.nsh"
!include "WordFunc.nsh"
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
!include "common.nsh"
!include "x64.nsh"
!include "MUI.nsh"
!include "WinVer.nsh" 
;!include "..\commonfunc.nsh"


!insertmacro MUI_LANGUAGE "SimpChinese"
# ===================== ��װ���汾 =============================
VIProductVersion             		"${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${EXE_NAME}"
VIAddVersionKey "FileDescription"   "${PRODUCT_NAME}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

!define INSTALL_PAGE_CONFIG 			0
;!define INSTALL_PAGE_LICENSE 			1
!define INSTALL_PAGE_PROCESSING 		1
!define INSTALL_PAGE_FINISH 			2
!define INSTALL_PAGE_UNISTCONFIG 		3
!define INSTALL_PAGE_UNISTPROCESSING 	4
!define INSTALL_PAGE_UNISTFINISH 		5

!define UNINSTALL_REG_PATH	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_PATHNAME}"

# �Զ���ҳ��
Page custom DUIPage
;Page instfiles  "" InstallShow
# ж�س�����ʾ����
UninstPage custom un.DUIPage

# ======================= DUILIB �Զ���ҳ�� =========================

Function DUIPage
	;InitPluginsDir   	
	;MessageBox MB_OK $OUTDIR
	
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\QRCode"
	
	SetOutPath $INSTDIR
	File /r "QRCode\*"
	;LsCIASkinEngine::InitTBCIASkinEngine "TTimeHelper" "TTimeHelper.xml" #ָ����Դ·������Դ�ļ�

	Call OnBtnInstall
	
	;ExecShell "open" "$INSTDIR\TTimeHelper.exe"
	;LsCIASkinEngine::ShowPage
	MessageBox MB_OK "��װ���"
	;Exec "$INSTDIR\${EXE_NAME}"
FunctionEnd

Function un.DUIPage
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\QRCode"
	;MessageBox MB_OK $INSTDIR
	;SetOutPath $INSTDIR
	
	#�˴���⵱ǰ�Ƿ��г����������У�����������У���ʾ��ж���ٰ�װ 
	;nsProcess::_FindProcess "${EXE_NAME}"
	;Pop $R0
	
	;${If} $R0 == 0
	;	MessageBox MB_OK "${EXE_NAME} �������У����˳�������!"
	;	goto InstallAbort
    ;${EndIf}
	
	;Processwork::KillProcess  
	
	DeleteRegKey HKLM "${UNINSTALL_REG_PATH}" 
	
	;Delete $INSTDIR
	;RMDir /s $INSTDIR
	RMDir /r /REBOOTOK $INSTDIR
	MessageBox MB_OK "ж�����"
	InstallAbort:
FunctionEnd

Section "none"
SectionEnd

Function OnBtnInstall
	WriteUninstaller "$INSTDIR\uninst.exe"
	
	# ���ж����Ϣ���������
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayIcon" "$INSTDIR\${EXE_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "Publisher" "${PRODUCT_PUBLISHER}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayVersion" "${PRODUCT_VERSION}"
FunctionEnd

;�������ԣ���������(http://www.reader8.cn/)ԭ�����ӣ�http://www.reader8.cn/jiaocheng/20120913/2118443.html