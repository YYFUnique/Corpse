# ====================== �Զ���� ��Ʒ��Ϣ==============================
!define PRODUCT_NAME           		"ʱ������"
!define PRODUCT_PATHNAME			"TTimeHelperAssist"
;!define PRODUCT_PATHNAME           	"SGLW"     				#��װж�����õ���KEY
!define INSTALL_APPEND_PATH         "SGLW"     				#��װ·��׷�ӵ����� 
!define INSTALL_DEFALT_SETUPPATH    "$PROGRAMFILES"       	#Ĭ�����ɵİ�װ·�� 
!define EXE_NAME               		"TTimeHelper.exe"
!define PRODUCT_VERSION        		"1.0.0.0"
!define PRODUCT_PUBLISHER      		"�Ĵ�������Ϣ�������޹�˾"
!define PRODUCT_LEGAL          		"Copyright (C) 2017 �Ĵ�������Ϣ�������޹�˾"
!define INSTALL_OUTPUT_NAME    		"ʱ������.exe"

# ====================== �Զ���� ��װ��Ϣ==============================
!define INSTALL_ICO 				"TTimeHelper.ico"
!define UNINSTALL_NAME				"uninst.exe"
# ====================== �ļ�����ű����� ==============================

!include "ttimehelper.nsh"

# ====================== NSIS���� ======================================

# ���Vista��win7 ��UAC����Ȩ������.
# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

#SetCompressor zlib

; ��װ������.
Name "${PRODUCT_NAME}"

# ��װ�����ļ���.
;IfFileExists
;CreateDirectory "..\Output\"
OutFile "..\Output\${INSTALL_OUTPUT_NAME}"

;InstallDir "1"

# ��װ��ж�س���ͼ��
Icon              "${INSTALL_ICO}"
UninstallIcon     "uninst.ico"