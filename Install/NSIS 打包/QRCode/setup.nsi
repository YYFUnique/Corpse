# ====================== �Զ���� ��Ʒ��Ϣ==============================
!define PRODUCT_NAME           		"��ά�빤��"			#ע���װ����ʾ����
!define PRODUCT_PATHNAME			"QRCodeAssist"			#ע���װ������
;!define PRODUCT_PATHNAME           "SGLW"     				#��װж�����õ���KEY
!define INSTALL_APPEND_PATH         "QRCode"     			#��װ·��׷�ӵ����� 
!define INSTALL_DEFALT_SETUPPATH    "$PROGRAMFILES"       	#Ĭ�����ɵİ�װ·�� 
!define EXE_NAME               		"QRTool.exe"			#����������
!define PRODUCT_VERSION        		"1.0.0.0"				#�ļ��汾
!define PRODUCT_PUBLISHER      		"�Ĵ�������Ϣ�������޹�˾"
!define PRODUCT_LEGAL          		"Copyright (C) 2017 �Ĵ�������Ϣ�������޹�˾"
!define INSTALL_OUTPUT_NAME    		"��ά�����ɹ���.exe"

# ====================== �Զ���� ��װ��Ϣ==============================
!define INSTALL_ICO 				"QRCode.ico"
!define UNINSTALL_NAME				"uninst.exe"
# ====================== �ļ�����ű����� ==============================

!include "QRCode.nsh"

# ====================== NSIS���� ======================================

# ���Vista��win7 ��UAC����Ȩ������.
# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

#SetCompressor zlib

; ��װ������.
Name "${PRODUCT_NAME}"

# ��װ�����ļ���.
OutFile "Output\${INSTALL_OUTPUT_NAME}"

;InstallDir "1"

# ��װ��ж�س���ͼ��
Icon              "${INSTALL_ICO}"
UninstallIcon     "uninst.ico"