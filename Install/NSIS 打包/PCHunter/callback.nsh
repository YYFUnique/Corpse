# ============================== 回调函数 ====================================

# 函数名以“.”开头的一般作为回调函数保留.
# 函数名以“un.”开头的函数将会被创建在卸载程序里，因此，普通安装区段和函数不能调用卸载函数，而卸载区段和卸载函数也不能调用普通函数。

# 安装过程
Section "Dummy Section" SecDummy
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\${INSTALL_APPEND_PATH}"
	;设置安装目录
	SetOutPath "$INSTDIR"
	;设置覆盖标记
	SetOverWrite on
	;复制要发布的安装文件  
	File /r "APP\*.*"
	SetOverWrite on
	SetRebootFlag false
	;Call BuildShortCut
  
	; 生成卸载程序
	WriteUninstaller "$INSTDIR\${UNINSTALL_NAME}"
	
	# 添加卸载信息到控制面板
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "UninstallString" "$INSTDIR\${UNINSTALL_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayIcon" "$INSTDIR\${EXE_NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "Publisher" "${PRODUCT_PUBLISHER}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd

/* Function CreateMutex
	;检查安装互斥
	ReCheck:
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${MyMutex_Install}") i .R1 ?e'
	Pop $R0
	System::Call 'kernel32::CloseHandle(i R1) i.s'
	;检查卸载互斥：
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${MyMutex_UnInstall}") i .R3 ?e'
	Pop $R2
	System::Call 'kernel32::CloseHandle(i R3) i.s'
	;判断安装/卸载互斥的存在
	${If} $R0 != 0
		MessageBox MB_RetryCancel|MB_ICONEXCLAMATION "安装程序已经运行！" IdRetry ReCheck
		Quit
	${ElseIf} $R2 != 0
		MessageBox MB_RetryCancel|MB_ICONEXCLAMATION "卸载程序已经运行！" IdRetry ReCheck
		Quit
	${Else}
		;创建安装互斥：
		System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${MyMutex_Install}") i .R1 ?e'
		Pop $R0
		StrCmp $R0 0 +2
		Quit
	${EndIf}
FunctionEnd */

; 安装时检测当前用户权限
Function CheckAdmin
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1

	UserInfo::GetOriginalAccountType
	Pop $2
	StrCmp $1 "Admin" 0 +3
		;MessageBox MB_OK 'User "$0" is in the Administrators group${REALMSG}'
		Goto done
	StrCmp $1 "Power" 0 +3
		MessageBox MB_OK '用户 "$0" 为 User 用户组,不具有管理员权限.安装程序自动退出!'
		Abort
	StrCmp $1 "User" 0 +3
		MessageBox MB_OK '用户 "$0" 为 User 用户组,不具有管理员权限.安装程序自动退出!'
		Abort
	StrCmp $1 "Guest" 0 +3
		MessageBox MB_OK '用户 "$0" 为 Guest 用户组,不具有管理员权限.安装程序自动退出!' 
		Abort
		MessageBox MB_OK  '检查用户权限时发生未知错误!' 
		Abort
	Win9x:
	MessageBox MB_OK '错误! 该动态库不能在 Windows 9x 下运行!'
	done:
FunctionEnd

Function .onInit
	Call CheckAdmin
	${FindProcess} "${EXE_NAME}" $0
	${if} $0 == 0
		;MessageBox MB_ICONQUESTION|MB_YESNO "${EXE_NAME} 正在运行，是否强制退出！" /SD IDYES IDYES +2 IDNO +1
		MessageBox MB_ICONQUESTION|MB_YESNO "    ${EXE_NAME} 正在运行，是否强制继续安装？" /SD IDYES IDYES +2 IDNO +1
		Abort
		
		${KillProcess} "${EXE_NAME}" $0
	${endif}
	
	;SectionGetSize ${SecDummy} $1
FunctionEnd

;Function .onGUIEnd
; 	RMDir /r $360Safetemp\${PRODUCT_NAME_EN}Temp
;	IfFileExists $360Safetemp\${PRODUCT_NAME_EN}Temp 0 +2
;  	RMDir /r /REBOOTOK $360Safetemp\${PRODUCT_NAME_EN}Temp
;FunctionEnd

# 安装成功以后.
Function .onInstSuccess
	;MessageBox MB_ICONQUESTION|MB_YESNO "onInstSuccess" /SD IDYES IDYES +2 IDNO +1
	nsTBCIASkinEngine::GetCheckStatus "OptionRun"
	Pop $0
	${if} $0 == 1
		Call RunApp
	${endif}
FunctionEnd

# 在安装失败后用户点击“取消”按钮时被调用.
Function .onInstFailed
    MessageBox MB_ICONQUESTION|MB_YESNO "安装成功！" /SD IDYES IDYES +2 IDNO +1
FunctionEnd


# 卸载操作开始前.
Function un.CreateUI
	MessageBox MB_ICONQUESTION|MB_YESNO "您确定要卸载${PRODUCT_NAME}吗?" /SD IDYES IDYES +2 IDNO +1
	Abort
		
	${FindProcess} "${EXE_NAME}" $0
	${if} $0 == 0
		MessageBox MB_ICONQUESTION|MB_YESNO "${EXE_NAME} 正在运行，是否强制卸载?" /SD IDYES IDYES +2 IDNO +1
		Abort
		
		${KillProcess} "${EXE_NAME}" $0
	${endif} 
FunctionEnd
/* Function un.onInit
    ;MessageBox MB_ICONQUESTION|MB_YESNO "您确定要卸载${PRODUCT_NAME}吗?" /SD IDYES IDYES +2 IDNO +1
	MessageBox MB_ICONQUESTION|MB_YESNO "您确定要卸载${PRODUCT_NAME}吗?" /SD IDYES IDYES +2 IDNO +1
	Abort
		
	${FindProcess} "${EXE_NAME}" $0
	${if} $0 == 0
		MessageBox MB_ICONQUESTION|MB_YESNO "${EXE_NAME} 正在运行，是否强制卸载?" /SD IDYES IDYES +2 IDNO +1
		Abort
		
		${KillProcess} "${EXE_NAME}" $0
	${endif} 
FunctionEnd */

# 卸载成功以后.
Function un.onUninstSuccess
	
FunctionEnd

# 安装成功以后启动应用程序
Function RunApp
	Exec "$INSTDIR\${EXE_NAME}"
FunctionEnd