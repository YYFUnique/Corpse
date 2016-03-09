// MachineCode.h: interface for the CMachineCode class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <WinIoCtl.h>

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

// �����»�����ĳ���
const DWORD MACHINECODE_SIZE = 16;
class LS_COMMON_API CMachineCode  
{
public:
	CMachineCode();

	virtual ~CMachineCode();

	//==================================================================
	//�� �� ��: GetMachineCode
	//��    ��: ��ȡ�ɻ�����
	//�������: [out] dwOldMachineCode,�ɻ�����
	//�� �� ֵ: ��ȡ�ɻ�����ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCode(DWORD& dwOldMachineCode);

	//==================================================================
	//�� �� ��: GetMachineCodeEx
	//��    ��: ��ȡ�»�����(16�ֽ�)
	//�������: [out] pbyMachineCode,��ȡ���»�����(���������ȱ������16�ֽ�)
	//�� �� ֵ: ��ȡ�»�����ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeEx(BYTE* pbyMachineCode);

	//==================================================================
	//�� �� ��: GetMachineCodeEx
	//��    ��: ��ȡ�»�����(32���ַ�)
	//�������: [out] strMachineCodeEx,��ȡ���»������ַ���
	//�� �� ֵ: ��ȡ�»�����ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeEx(CString& strMachineCodeEx);

	//==================================================================
	//�� �� ��: GetMachineCodeByIOControl
	//��    ��: ͨ��IO��ʽ��ȡ������
	//�������: [out] dwMachineCode, ��ȡ���ľɻ�����
	//			[out] pbyMachineCode, ��ȡ�����»�����
	//�� �� ֵ: ��ȡ�ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeByIOControl(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//�� �� ��: GetMachineCodeByAsScsiDrive
	//��    ��: ͨ����ȡ�������к�(SCSI��ʽ��ȡ),���ɻ�����(IO����ԱȨ��)
	//�������: [out] dwMachineCode, ��ȡ���ľɻ�����
	//			[out] pbyMachineCode, ��ȡ�����»�����
	//�� �� ֵ: ���������ɳɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeByAsScsiDrive(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//�� �� ��: GetMachineCodeByZeroRights
	//��    ��: ͨ���������к�(IO��ʽ��ȡ)��������������(��Ȩ��Ҫ��ֻҪ�ܴ򿪴���)
	//�������: [out] dwMachineCode, ��ȡ���ľɻ�����
	//			[out] pbyMachineCode, ��ȡ�����»�����
	//�� �� ֵ: ���������ɳɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeByZeroRights(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//�� �� ��: GetMachineCodeByNetAdaptID
	//��    ��: ͨ������������ID��ȡ������
	//�������: [out] dwMachineCode, ��ȡ���ľɻ�����
	//			[out] pbyMachineCode, ��ȡ�����»�����
	//�� �� ֵ: ��ȡ������ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetMachineCodeByNetAdaptID(DWORD& dwMachineCode,BYTE *pbyMachineCode);
protected:
	BOOL IsRunInVPC();		// ����Ƿ���VPC�����������
	BOOL IsRunInVmWare();	// ����Ƿ���VMware���⼯������
	BOOL IsRunInVM();		// ����Ƿ������������������
	
//==================================================================
// �� �� ��: GetNewHwCodeFromReg
// ��    ��: ��ȡ���HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NewHwCode��λ�õ�
//			 �Զ����»�����
// �������: [out] pbyNewMachineCode, ��ȡ�����»�����
// �� �� ֵ: ��ȡ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetNewHwCodeFromReg(PBYTE pbyNewMachineCode);
	
//==================================================================
// �� �� ��: GetOldHwCodeFromReg
// ��    ��: ��ȡ���HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\OldHwCode��λ�õ�
//			 �Զ���ɻ�����
// �������: [out] dwOldMachineCode, ��ȡ���ľɻ�����
// �� �� ֵ: ��ȡ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetOldHwCodeFromReg(DWORD &dwOldMachineCode);
	
//==================================================================
//�� �� ��: GetDiskInfoWithZeroRights
//��    ��: ͨ��IO��ʽ��ȡ������Ϣ(��Ȩ��Ҫ��ֻҪ�ܴ򿪴���)
//�������: [out] chSerialNumber, �������к�
//			[out] chModelNumber, ����Ӳ����
//			[out] dwSerialLen, �������кų���
//			[out] dwModelLen, ����Ӳ���ų���
//�� �� ֵ: �������óɹ�����TURE,���򷵻�FALSE
//==================================================================
	BOOL GetDiskInfoWithZeroRights(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
	
	//==================================================================
	//�� �� ��: GetHardDiskSerialNumByZeroRights
	//��    ��: ͨ��IO��ʽ��ȡ������Ϣ(��Ȩ��Ҫ��ֻҪ�ܴ򿪴���)
	//�������:dwHardIndexӲ�����
	//			[out] chSerialNumber, �������к�
	//			[out] chModelNumber, ����Ӳ����
	//			[out] dwSerialLen, �������кų���
	//			[out] dwModelLen, ����Ӳ���ų���
	//�� �� ֵ: �������óɹ�����TURE,���򷵻�FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByZeroRights(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

//==================================================================
//�� �� ��: GetDiskInfoAsScsiDrive
//��    ��: ͨ��SCSI��ʽ,��ȡ������Ϣ
//�������: [out] chSerialNumber, �������к�
//			[out] chModelNumber, ����Ӳ����
//			[out] dwSerialLen, �������кų���
//			[out] dwModelLen, ����Ӳ���ų���
//�� �� ֵ: ��ȡ������Ϣ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetDiskInfoAsScsiDrive(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
	
	//==================================================================
	//�� �� ��: GetHardDiskSerialNumByScsiDrive
	//��    ��: ͨ��SCSI��ʽ,��ȡ������Ϣ���к�
	//�������: dwHardIndexӲ�̱��
	//			[out] chSerialNumber, �������к�
	//			[out] chModelNumber, ����Ӳ����
	//			[out] dwSerialLen, �������кų���
	//			[out] dwModelLen, ����Ӳ���ų���
	//�� �� ֵ: ��ȡ������Ϣ�ɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByScsiDrive(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

//==================================================================
//�� �� ��: CalculateDiskId
//��    ��: ͨ���������кż��������
//�������: [out] dwHwId, ����õ��Ļ�����
//			[in]  pDiskInfo, �������к�
//			[in]  dwInfoLen, �������кų���
//�� �� ֵ: �޷���ֵ
//==================================================================
	void CalculateDiskId(DWORD& dwHwId,CHAR pDiskInfo[],DWORD dwInfoLen);
	
//==================================================================
//�� �� ��: GetDiskSerialNumAndModelNumByIoControl
//��    ��: ͨ��IO��ȡӲ�����кź��ͺ�
//�������: [out] chSerialNumber, �������к�
//			[out] chModelNumber, �����ͺ�
//			[out] dwSerialLen, �������к��ַ�������
//			[out] dwModelLen, �����ͺ��ַ�������
//�� �� ֵ: ��ȡ�������кź��ͺųɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetDiskSerialNumAndModelNumByIoControl(CHAR chSerialNumber[],char chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

	//==================================================================
	//�� �� ��: GetHardDiskSerialNumByIoControl
	//��    ��: ��ȡָ��Ӳ�̵����к�
	//�������: dwHardIndex Ӳ�̱��
	//			[out] chSerialNumber, �������к�
	//			[out] chModelNumber, �����ͺ�
	//			[out] dwSerialLen, �������к��ַ�������
	//			[out] dwModelLen, �����ͺ��ַ�������
	//�� �� ֵ: ��ȡ�������кź��ͺųɹ�����TRUE,���򷵻�FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByIoControl(DWORD dwHardIndex,CHAR chSerialNumber[],char chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
//==================================================================
//�� �� ��: CalculateMachineCodeWithIO
//��    ��: ͨ���������кż��������
//�������: [out] dwMachineCode, ����õ��Ļ�����
//			[in]  chNumber, �������к�
//			[in]  iLen, �������кų���
//�� �� ֵ: �޷���ֵ
//==================================================================
	void CalculateMachineCodeWithIO(DWORD& dwMachineCode,CHAR chNumber[],int iLen);
	
//==================================================================
//�� �� ��: RndAuthAlg
//��    ��: �Զ�������㷨
//�������: [in] dwNum, �������
//�� �� ֵ: ���ؼ��ܺ����
//==================================================================
	DWORD RndAuthAlg(DWORD dwNum);
	
//==================================================================
//�� �� ��: GetNetAdaptID
//��    ��: ��ȡ������������Ϣ(���������,������������Ϣ�����׽ڵ�)
//�������: [out]  byAddress, ��������Ϣ����
//			[out] �����������С
//�� �� ֵ: ��ȡ������������Ϣ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetNetAdaptID(BYTE byAddress[],DWORD& dwAddressLen);
	
//==================================================================
//�� �� ��: CalculateNetAdaptId
//��    ��: ͨ������������ID���������
//�������: [out] dwMachineCode, ����õ��Ļ�����
//			[in]  pAddress, ����������ID�����ַ
//			[in]  dwAddressLen, ����������ID���泤��
//�� �� ֵ: �޷���ֵ
//==================================================================
	void CalculateNetAdaptId(DWORD& dwMachineCode,BYTE* pAddress,DWORD dwAddressLen);


	static DWORD m_dwOldMachineCode;
	static BYTE m_byNewMachineCode[MACHINECODE_SIZE];

//==================================================================
//�� �� ��: ArrayIsEmpty
//��    ��: �ж������Ƿ�Ϊ��(����Ԫ�ؾ���Ϊ0)
//�������: [in] byArray, �ж�����
//			[in] iArrayLen, �ж����鳤��
//�� �� ֵ: ���鲻Ϊ��(��������Ԫ�ؾ���Ϊ0)�򷵻�TRUE,���򷵻�FALSE
//==================================================================
	BOOL ArrayIsEmpty(PBYTE byArray,int iArrayLen);
//==================================================================
//�� �� ��: GetLastSaveHwExId
//��    ��: ��ע����ж�ȡ�ϴ����ɵ������չ������
//�������: [out] pbyMachineCodeCode, �����չ������
//�� �� ֵ: ��ȡ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetLastSaveHwExId(BYTE* pbyMachineCodeCode);
	
//==================================================================
//�� �� ��: GenerateHwExId
//��    ��: ���������չ������,��д��ע���
//�������: ���������
//�� �� ֵ: ���������չ������ɹ�����TURE,���򷵻�FALSE
//==================================================================
	BOOL GenerateHwExId();		
	
//==================================================================
//�� �� ��: GetLastSaveHwId
//��    ��: ��ע����ж�ȡ�ϴ����ɵ����������
//�������: [out] dwMachineCode,���������
//�� �� ֵ: ��ȡ���������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetLastSaveHwId(DWORD& dwMachineCode);
	
//==================================================================
//�� �� ��: GenerateHwId
//��    ��: �������������,��д��ע���
//�������: [in]  dwMachineCode, �����ں�������ʵ������
//�� �� ֵ: �������������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GenerateHwId(DWORD& dwMachineCode);

	
//==================================================================
//�� �� ��: CalculateMachineCodeEx
//��    ��: ͨ��MD5�㷨�����������
//�������: [in]  dwType, ���ɻ�����ķ�ʽ,Ȩ��
//						IO_MANAGER_TYPE, IO����ԱȨ��
//						SCSI_TYPE, ͨ������SCSI��ʽ
//						ZERO_IO_TYPE,ͨ������IO(0Ȩ��)
//						NET_ID_TYPE,ͨ������������ID
//						GUID_TYPE, �������ʽ
//			[in]  pbyHardwardId1, �����һ��Ӳ��ID
//			[in]  dwHardwardIdLen1, �����һ��Ӳ��ID����
//			[in]  pbyHardwareId2, ����ڶ���Ӳ��ID
//			[in]  dwHardwardIdLen2, ����ڶ���Ӳ��ID����
//			[out] byMachineCodeEx, �������ɵ��»�����
//�� �� ֵ: �޷���ֵ
//==================================================================
	void CalculateMachineCodeEx(DWORD dwType,const void* pbyHardwardId1,DWORD dwHardwardIdLen1,BYTE* pbyHardwareId2,DWORD dwHardwardIdLen2,BYTE byMachineCodeEx[16]);

};
