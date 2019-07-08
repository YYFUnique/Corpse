#ifndef __UDV_RETURNCODE_H__
#define __UDV_RETURNCODE_H__


#define UDVReturnOK							0
#define UDVError							1

#define UDVErrorcDriveLetterNotFound		19
#define UDVErrorInputParameter				20	//Input parameter error
#define UDVErrorGetDeviceInfo				21	//get drive info error
#define UDVErrorGetDeviceString				22	//get drive info error ( string, firmware verison.. )
#define UDVErrorGetCapacity					23	//Capacity error
#define UDVErrorNoHidden					24	//No hidden area
#define UDVErrorIndexOutOfCompass			25	//wPageIndex is out of compass 
#define UDVErrorCountOutOfCompass			26	//wPageCount is out of compass
#define UDVErrorAccessingOutOfCompass		27	//Accessing is out of compass
#define UDVErrorReadHiddenArea				28	//Error in reading hidden area
#define UDVErrorWriteDataToBuff				29	//Error in writing data to the buf
#define UDVErrorIncorrectHiddenAreaPassword	30	//Incorrect hidden area password.
#define UDVErrorHiddenNOA_OutOfMAX			31	//Current NOA is equal to Max NOA.
#define UDVErrorWriteHiddenArea				32	//Error in writing hidden area
#define UDVErrorReadDataFromBuff			33	//Error in reading data from the buf
#define UDVErrorDevice						34	//Device Error
#define UDVErrorCDAlreadyLocked				35	//CD is already locked.
#define UDVErrorCDLock						36	//CD lock error.
#define UDVErrorVerifyCDKey					37	//Verify CD key error.
#define UDVErrorSecuritySessionNoOpened		38	//CD Owner¡¦s Security Session is not opened.
#define UDVErrorCDUnlock					39	//CD unlock error.
#define UDVErrorOpenSecuritySession			40	//Open secure session error.
#define UDVErrorCDNoLocked					41	//CD is not locked yet.
#define UDVErrorTerminateSecureSession		42	//Error Terminate Secure Session
#define UDVErrorOutOfCompassAccessCD		43	//out of compass for access CD
#define UDVErrorWriteCDBuffer				44	//Write CD buffer error
#define UDVErrorReadCDBuffer				45	//Read CD buffer error
#define UDVErrorReadBuffer					46	//Read buffer error
#define UDVErrorWriteBuffer					47//46	//Write buffer error
#define UDVErrorPasswordExisted				49	//Password was existed
#define UDVErrorDeviceBeSeized				50	//The device had been used by another program.
#define UDVErrorLogout						51	//Logout error.
#define UDVErrorSetPassword					52	//Set password error.
#define UDVErrorPasswordNoExisted			53	//Password was not existed
#define UDVErrorAlreadyLogin				54	//Partition is already login.
#define UDVErrorAlreadyLogout				55	//Partition is already logout.
#define UDVErrorWrongPassword				56	//Wrong password.
#define UDVErrorExceededMaxNOA				57	//Exceeded max NOA(Number of Attempts).
#define UDVErrorLogin						58	//Login error.
#define UDVErrorChangePassword				59	//Change Password error.
#define UDVErrorUnblockPassword				60	//Unblock Password error.
#define UDVErrorEncryptBuffer				61	//Encrypt buffer error
#define UDVErrorDecryptBuffer				62	//Decrypt buffer error
#define UDVErrorLoginDeviceSecureMechanism	70	//Login device secure mechanism error.
#define UDVErrorLogoutDeviceSecureMechanism	71	//Logout device secure mechanism error.
#define UDVErrorPasswordNoExisted_for_DeviceSecureMechanism	72	//Password was not existed for device secure mechanism.
#define UDVErrorWrongPassword_for_DeviceSecureMechanism		73	//Wrong password for device secure mechanism.
#define UDVErrorVeerSecuritySessionType		74	//veer secure session type error.
#define UDVErrorNoInitSDK					75	//SDK had not been initiated yet.
#define UDVErrorAlreadyInitSDK				76	//SDK had been initiated already.
#define UDVErrorAlreadyCloseSDK				77	//SDK had been closed.
#define UDVErrorSetDevicePartition			78	//set device partition error.
#define UDVErrorWrong_SDK_Password			79	//Wrong SDK password.
#define UDVErrorWrong_HostPublicKey			80	//Host's Public Key does not match the CD Owner's Public Key.
#define UDVErrorErasePartitionData			81	//Erase partition data error.
#define UDVErrorNoKeyFromSmartCard			82	//Receive key from smart card error.
#define UDVErrorSetDeviceInfo				83	//set device information error.
#define UDVErrorPartitionIsWriteProtected	84	//The partition is write-protected.
#define UDVErrorHardwareProblem				85	//Hardware problem during the operation (the status of accessed page is unknown).
#define UDVImageExistsInDeviceCD			86	//CD_ROM imag had been existed in CDROM Disk
#define UDVErrorOpenFile					87	//Open file error
#define UDVInvalidFileSize					88	//Invalid file size
#define UDVErrorReadFile					89	//Read file error
#define UDVErrorWriteFile					90	//Write file error
#define UDVErrorNoMBR							91	//No MBR in partition
#define UDVErrorNoFAT							92	//No FAT in partition
#define UDVError_DeviceUnready				93	//The device is unready.
#define UDVError_FunctionNotSupported		100	//The function is not supported.
#define UDVError_VolumeUnready				101 //The volume is not ready yet
#define UDVErrorUser                        102

#endif
