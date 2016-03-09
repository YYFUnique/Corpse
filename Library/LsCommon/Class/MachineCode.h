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

// 定义新机器码的长度
const DWORD MACHINECODE_SIZE = 16;
class LS_COMMON_API CMachineCode  
{
public:
	CMachineCode();

	virtual ~CMachineCode();

	//==================================================================
	//函 数 名: GetMachineCode
	//功    能: 获取旧机器码
	//输入参数: [out] dwOldMachineCode,旧机器码
	//返 回 值: 获取旧机器码成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCode(DWORD& dwOldMachineCode);

	//==================================================================
	//函 数 名: GetMachineCodeEx
	//功    能: 获取新机器码(16字节)
	//输入参数: [out] pbyMachineCode,获取的新机器码(缓冲区长度必须大于16字节)
	//返 回 值: 获取新机器码成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeEx(BYTE* pbyMachineCode);

	//==================================================================
	//函 数 名: GetMachineCodeEx
	//功    能: 获取新机器码(32个字符)
	//输入参数: [out] strMachineCodeEx,获取的新机器码字符串
	//返 回 值: 获取新机器码成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeEx(CString& strMachineCodeEx);

	//==================================================================
	//函 数 名: GetMachineCodeByIOControl
	//功    能: 通过IO方式获取机器码
	//输入参数: [out] dwMachineCode, 获取到的旧机器码
	//			[out] pbyMachineCode, 获取到的新机器码
	//返 回 值: 获取成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeByIOControl(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//函 数 名: GetMachineCodeByAsScsiDrive
	//功    能: 通过获取磁盘序列号(SCSI方式获取),生成机器码(IO管理员权限)
	//输入参数: [out] dwMachineCode, 获取到的旧机器码
	//			[out] pbyMachineCode, 获取到的新机器码
	//返 回 值: 机器码生成成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeByAsScsiDrive(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//函 数 名: GetMachineCodeByZeroRights
	//功    能: 通过磁盘序列号(IO方式获取)生成主机机器码(无权限要求只要能打开磁盘)
	//输入参数: [out] dwMachineCode, 获取到的旧机器码
	//			[out] pbyMachineCode, 获取到的新机器码
	//返 回 值: 机器码生成成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeByZeroRights(DWORD& dwMachineCode,BYTE *pbyMachineCode);

	//==================================================================
	//函 数 名: GetMachineCodeByNetAdaptID
	//功    能: 通过网卡适配器ID获取机器码
	//输入参数: [out] dwMachineCode, 获取到的旧机器码
	//			[out] pbyMachineCode, 获取到的新机器码
	//返 回 值: 获取机器码成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetMachineCodeByNetAdaptID(DWORD& dwMachineCode,BYTE *pbyMachineCode);
protected:
	BOOL IsRunInVPC();		// 检测是否在VPC虚拟机中运行
	BOOL IsRunInVmWare();	// 检测是否在VMware虚拟集中运行
	BOOL IsRunInVM();		// 检测是否运行在虚拟机中运行
	
//==================================================================
// 函 数 名: GetNewHwCodeFromReg
// 功    能: 获取存放HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NewHwCode该位置的
//			 自定义新机器码
// 输入参数: [out] pbyNewMachineCode, 获取到的新机器码
// 返 回 值: 获取成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetNewHwCodeFromReg(PBYTE pbyNewMachineCode);
	
//==================================================================
// 函 数 名: GetOldHwCodeFromReg
// 功    能: 获取存放HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\OldHwCode该位置的
//			 自定义旧机器码
// 输入参数: [out] dwOldMachineCode, 获取到的旧机器码
// 返 回 值: 获取成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetOldHwCodeFromReg(DWORD &dwOldMachineCode);
	
//==================================================================
//函 数 名: GetDiskInfoWithZeroRights
//功    能: 通过IO方式获取磁盘信息(无权限要求只要能打开磁盘)
//输入参数: [out] chSerialNumber, 磁盘序列号
//			[out] chModelNumber, 磁盘硬件号
//			[out] dwSerialLen, 磁盘序列号长度
//			[out] dwModelLen, 磁盘硬件号长度
//返 回 值: 函数调用成功返回TURE,否则返回FALSE
//==================================================================
	BOOL GetDiskInfoWithZeroRights(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
	
	//==================================================================
	//函 数 名: GetHardDiskSerialNumByZeroRights
	//功    能: 通过IO方式获取磁盘信息(无权限要求只要能打开磁盘)
	//输入参数:dwHardIndex硬盘序号
	//			[out] chSerialNumber, 磁盘序列号
	//			[out] chModelNumber, 磁盘硬件号
	//			[out] dwSerialLen, 磁盘序列号长度
	//			[out] dwModelLen, 磁盘硬件号长度
	//返 回 值: 函数调用成功返回TURE,否则返回FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByZeroRights(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

//==================================================================
//函 数 名: GetDiskInfoAsScsiDrive
//功    能: 通过SCSI方式,获取磁盘信息
//输入参数: [out] chSerialNumber, 磁盘序列号
//			[out] chModelNumber, 磁盘硬件号
//			[out] dwSerialLen, 磁盘序列号长度
//			[out] dwModelLen, 磁盘硬件号长度
//返 回 值: 获取磁盘信息成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetDiskInfoAsScsiDrive(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
	
	//==================================================================
	//函 数 名: GetHardDiskSerialNumByScsiDrive
	//功    能: 通过SCSI方式,获取磁盘信息序列号
	//输入参数: dwHardIndex硬盘编号
	//			[out] chSerialNumber, 磁盘序列号
	//			[out] chModelNumber, 磁盘硬件号
	//			[out] dwSerialLen, 磁盘序列号长度
	//			[out] dwModelLen, 磁盘硬件号长度
	//返 回 值: 获取磁盘信息成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByScsiDrive(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

//==================================================================
//函 数 名: CalculateDiskId
//功    能: 通过磁盘序列号计算机器码
//输入参数: [out] dwHwId, 计算得到的机器码
//			[in]  pDiskInfo, 磁盘序列号
//			[in]  dwInfoLen, 磁盘序列号长度
//返 回 值: 无返回值
//==================================================================
	void CalculateDiskId(DWORD& dwHwId,CHAR pDiskInfo[],DWORD dwInfoLen);
	
//==================================================================
//函 数 名: GetDiskSerialNumAndModelNumByIoControl
//功    能: 通过IO获取硬盘序列号和型号
//输入参数: [out] chSerialNumber, 磁盘序列号
//			[out] chModelNumber, 磁盘型号
//			[out] dwSerialLen, 磁盘序列号字符串长度
//			[out] dwModelLen, 磁盘型号字符串长度
//返 回 值: 获取磁盘序列号和型号成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetDiskSerialNumAndModelNumByIoControl(CHAR chSerialNumber[],char chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);

	//==================================================================
	//函 数 名: GetHardDiskSerialNumByIoControl
	//功    能: 获取指定硬盘的序列号
	//输入参数: dwHardIndex 硬盘编号
	//			[out] chSerialNumber, 磁盘序列号
	//			[out] chModelNumber, 磁盘型号
	//			[out] dwSerialLen, 磁盘序列号字符串长度
	//			[out] dwModelLen, 磁盘型号字符串长度
	//返 回 值: 获取磁盘序列号和型号成功返回TRUE,否则返回FALSE
	//==================================================================
	BOOL GetHardDiskSerialNumByIoControl(DWORD dwHardIndex,CHAR chSerialNumber[],char chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen);
//==================================================================
//函 数 名: CalculateMachineCodeWithIO
//功    能: 通过磁盘序列号计算机器码
//输入参数: [out] dwMachineCode, 计算得到的机器码
//			[in]  chNumber, 磁盘序列号
//			[in]  iLen, 磁盘序列号长度
//返 回 值: 无返回值
//==================================================================
	void CalculateMachineCodeWithIO(DWORD& dwMachineCode,CHAR chNumber[],int iLen);
	
//==================================================================
//函 数 名: RndAuthAlg
//功    能: 自定义加密算法
//输入参数: [in] dwNum, 传入参数
//返 回 值: 返回加密后参数
//==================================================================
	DWORD RndAuthAlg(DWORD dwNum);
	
//==================================================================
//函 数 名: GetNetAdaptID
//功    能: 获取网卡适配器信息(多网卡情况,网卡适配器信息链表首节点)
//输入参数: [out]  byAddress, 适配器信息缓存
//			[out] 适配器缓存大小
//返 回 值: 获取网卡适配器信息成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetNetAdaptID(BYTE byAddress[],DWORD& dwAddressLen);
	
//==================================================================
//函 数 名: CalculateNetAdaptId
//功    能: 通过网卡适配器ID计算机器码
//输入参数: [out] dwMachineCode, 计算得到的机器码
//			[in]  pAddress, 网卡适配器ID缓存地址
//			[in]  dwAddressLen, 网卡适配器ID缓存长度
//返 回 值: 无返回值
//==================================================================
	void CalculateNetAdaptId(DWORD& dwMachineCode,BYTE* pAddress,DWORD dwAddressLen);


	static DWORD m_dwOldMachineCode;
	static BYTE m_byNewMachineCode[MACHINECODE_SIZE];

//==================================================================
//函 数 名: ArrayIsEmpty
//功    能: 判断数组是否为空(所有元素均不为0)
//输入参数: [in] byArray, 判断数组
//			[in] iArrayLen, 判断数组长度
//返 回 值: 数组不为空(数组所有元素均不为0)则返回TRUE,否则返回FALSE
//==================================================================
	BOOL ArrayIsEmpty(PBYTE byArray,int iArrayLen);
//==================================================================
//函 数 名: GetLastSaveHwExId
//功    能: 从注册表中读取上次生成的随机扩展机器码
//输入参数: [out] pbyMachineCodeCode, 随机扩展机器码
//返 回 值: 获取成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetLastSaveHwExId(BYTE* pbyMachineCodeCode);
	
//==================================================================
//函 数 名: GenerateHwExId
//功    能: 产生随机扩展机器码,并写入注册表
//输入参数: 无输入参数
//返 回 值: 生成随机扩展机器码成功返回TURE,否则返回FALSE
//==================================================================
	BOOL GenerateHwExId();		
	
//==================================================================
//函 数 名: GetLastSaveHwId
//功    能: 从注册表中读取上次生成的随机机器码
//输入参数: [out] dwMachineCode,随机机器码
//返 回 值: 读取随机机器码成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetLastSaveHwId(DWORD& dwMachineCode);
	
//==================================================================
//函 数 名: GenerateHwId
//功    能: 生成随机机器码,并写入注册表
//输入参数: [in]  dwMachineCode, 参数在函数中无实际作用
//返 回 值: 生成随机机器码成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GenerateHwId(DWORD& dwMachineCode);

	
//==================================================================
//函 数 名: CalculateMachineCodeEx
//功    能: 通过MD5算法计算机新器码
//输入参数: [in]  dwType, 生成机器码的方式,权限
//						IO_MANAGER_TYPE, IO管理员权限
//						SCSI_TYPE, 通过磁盘SCSI方式
//						ZERO_IO_TYPE,通过磁盘IO(0权限)
//						NET_ID_TYPE,通过网卡适配器ID
//						GUID_TYPE, 随机数形式
//			[in]  pbyHardwardId1, 传入第一个硬件ID
//			[in]  dwHardwardIdLen1, 传入第一个硬件ID长度
//			[in]  pbyHardwareId2, 传入第二个硬件ID
//			[in]  dwHardwardIdLen2, 传入第二个硬件ID长度
//			[out] byMachineCodeEx, 传出生成的新机器码
//返 回 值: 无返回值
//==================================================================
	void CalculateMachineCodeEx(DWORD dwType,const void* pbyHardwardId1,DWORD dwHardwardIdLen1,BYTE* pbyHardwareId2,DWORD dwHardwardIdLen2,BYTE byMachineCodeEx[16]);

};
