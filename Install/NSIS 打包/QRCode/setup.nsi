# ====================== 自定义宏 产品信息==============================
!define PRODUCT_NAME           		"二维码工具"			#注册表安装项显示名称
!define PRODUCT_PATHNAME			"QRCodeAssist"			#注册表安装键名称
;!define PRODUCT_PATHNAME           "SGLW"     				#安装卸载项用到的KEY
!define INSTALL_APPEND_PATH         "QRCode"     			#安装路径追加的名称 
!define INSTALL_DEFALT_SETUPPATH    "$PROGRAMFILES"       	#默认生成的安装路径 
!define EXE_NAME               		"QRTool.exe"			#主进程名称
!define PRODUCT_VERSION        		"1.0.0.0"				#文件版本
!define PRODUCT_PUBLISHER      		"四川龙信信息技术有限公司"
!define PRODUCT_LEGAL          		"Copyright (C) 2017 四川龙信信息技术有限公司"
!define INSTALL_OUTPUT_NAME    		"二维码生成工具.exe"

# ====================== 自定义宏 安装信息==============================
!define INSTALL_ICO 				"QRCode.ico"
!define UNINSTALL_NAME				"uninst.exe"
# ====================== 文件打包脚本内容 ==============================

!include "QRCode.nsh"

# ====================== NSIS属性 ======================================

# 针对Vista和win7 的UAC进行权限请求.
# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

#SetCompressor zlib

; 安装包名字.
Name "${PRODUCT_NAME}"

# 安装程序文件名.
OutFile "Output\${INSTALL_OUTPUT_NAME}"

;InstallDir "1"

# 安装和卸载程序图标
Icon              "${INSTALL_ICO}"
UninstallIcon     "uninst.ico"