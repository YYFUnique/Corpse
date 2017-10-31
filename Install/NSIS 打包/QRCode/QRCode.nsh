
# ===================== 外部插件以及宏 =============================
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
# ===================== 安装包版本 =============================
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

# 自定义页面
Page custom DUIPage
;Page instfiles  "" InstallShow
# 卸载程序显示进度
UninstPage custom un.DUIPage

# ======================= DUILIB 自定义页面 =========================

Function DUIPage
	;InitPluginsDir   	
	;MessageBox MB_OK $OUTDIR
	
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\QRCode"
	
	SetOutPath $INSTDIR
	File /r "QRCode\*"
	;LsCIASkinEngine::InitTBCIASkinEngine "TTimeHelper" "TTimeHelper.xml" #指定资源路径及资源文件

	Call OnBtnInstall
	
	;ExecShell "open" "$INSTDIR\TTimeHelper.exe"
	;LsCIASkinEngine::ShowPage
	MessageBox MB_OK "安装完成"
	;Exec "$INSTDIR\${EXE_NAME}"
FunctionEnd

Function un.DUIPage
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\QRCode"
	;MessageBox MB_OK $INSTDIR
	;SetOutPath $INSTDIR
	
	#此处检测当前是否有程序正在运行，如果正在运行，提示先卸载再安装 
	;nsProcess::_FindProcess "${EXE_NAME}"
	;Pop $R0
	
	;${If} $R0 == 0
	;	MessageBox MB_OK "${EXE_NAME} 正在运行，请退出后重试!"
	;	goto InstallAbort
    ;${EndIf}
	
	;Processwork::KillProcess  
	
	DeleteRegKey HKLM "${UNINSTALL_REG_PATH}" 
	
	;Delete $INSTDIR
	;RMDir /s $INSTDIR
	RMDir /r /REBOOTOK $INSTDIR
	MessageBox MB_OK "卸载完成"
	InstallAbort:
FunctionEnd

Section "none"
SectionEnd

Function OnBtnInstall
	WriteUninstaller "$INSTDIR\uninst.exe"
	
	# 添加卸载信息到控制面板
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayIcon" "$INSTDIR\${EXE_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "Publisher" "${PRODUCT_PUBLISHER}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayVersion" "${PRODUCT_VERSION}"
FunctionEnd

;本文来自：读书人网(http://www.reader8.cn/)原文链接：http://www.reader8.cn/jiaocheng/20120913/2118443.html