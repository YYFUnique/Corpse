#pragma once

/********************************************************************************************/
/* software [in]：指定门锁系统，为固定值：30																			*/
/*	server [in]：字符指针，指定门锁系统数据库安装的服务器（SQL Server）名。							*/
/*	username [in]：字符指针，用户名，指定当前门锁系统的操作人员。											*/
/*	Port [in]：串口号，0-USB, 1-COM1，2-COM2，3-COM3，4-COM4依次类推。默认值是0	*/
/*	Encoder [in]：发行机类型，0-手动发行机，1-自动发行机，2-MSR206（磁卡）。默认值是0	*/
/*	TMEncoder [in]：TM发行机类型，1-DS9097E，5-DS9097U，默认值是5                               */
/* 示例： i := Init(30, '192.168.0.1','Interface',port,0,5) ;																*/
/*********************************************************************************************/

typedef int (WINAPI* FN_INIT)(int nSoftWare, char* lpszServer, char* lpszUserName, int nPort, int nEncoder, int nTMEncoder);

/************************************************************************/
/* 结束工作期                                                                     */
/* EndSession ;*/
/************************************************************************/
typedef int (WINAPI* FN_EndSession)(void);

/************************************************************************/
/* username [in]：字符指针，用户名，用于门锁系统记录操作人员。           */
/* ChangeUser('张三'); */
/************************************************************************/
typedef void (WINAPI* FN_ChangeUser)(char* lpszUserName);

/************************************************************************/
/* room [in]：房号，最大12字节字符串，必须是门锁系统设置的房号。
/* gate [in]：公共通道，字符串参数，其他为指定通道代码。例如：“010203”表示授权01、02、03三个通道，最大为15。
/* Stime [in]：起止时间，24字节字符串，格式yyyymmddhhnnyyyymmddhhnn，例如：“200012311230200101011230”表示2000年12月31日12时30分到2001年1月1日12时30分。起始时间为当前系统时间，截止时间必须大于起始时间。
/* Guestname [in]：客人姓名，最大30字节，可以为NULL。
/* Guestid [in]：客人ID，最大30字节，可以为NULL。
/* Overflag[in]：整形，是否覆盖当前卡上的内容。1-覆盖，当前卡不是新卡则自动注销（必须是客人卡）。0-不覆盖，不是新卡则返回。
/* openflag[in]：整形，门锁的开门方式。0：卡开门; 1: 卡或指纹开门；2：指纹开门；3：指纹和卡开门。通常选0
/* Cardno [out]：长整形指针，接收卡号（发卡后分配的唯一卡代码）。可以为NULL。
/* Breakfast[in]:整形，早餐券数量，0为无早餐，1为有效时间内的每天1份，2为有效时间内的每天2份。
/* track1 [in]：字符串指针，磁卡第1轨数据，如不写此轨可为NULL。
/* track2 [in]：字符串指针，磁卡第2轨数据，如不写此轨可为NULL。
/* Lift[in]: 字符串指针，电梯，“1”表示楼层代码为“B001”，“2”表示楼层代码为“B002”，以此类推。00表示按系统默认设置，99表示所有楼层。
/* Fingerflag[in]:整型,是否采集指纹. 1-采集,0-不采集                                                                     */
/************************************************************************/
typedef int (WINAPI* FN_NewKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lpszGuestName,
														char* lpszGuestId, int nOverFlag, int nOpenFlag, LONG* lCardNo, int nBreakFast, 
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);

/************************************************************************/
/* “复制”客人卡，新发行的客人卡与原来的客人卡可同时使用，且起始时间相同。                                                                     */
/* room [in]：房号，最大12字节字符串，必须是门锁系统设置的房号。
/* gate [in]：公共通道，字符串参数，其他为指定通道代码。例如：“010203”表示授权01、02、03三个通道，最大为15。
/* Stime [in]：起止时间，24字节字符串，格式yyyymmddhhnnyyyymmddhhnn，例如：“200012311230200101011230”表示2000年12月31日12时30分到2001年1月1日12时30分。起始时间为当前系统时间，截止时间必须大于起始时间。
/* Guestname [in]：客人姓名，最大30字节，可以为NULL。
/* Guestid [in]：客人ID，最大30字节，可以为NULL。
/* Overflag[in]：整形，是否覆盖当前卡上的内容。1-覆盖，当前卡不是新卡则自动注销（必须是客人卡）。0-不覆盖，不是新卡则返回。
/* openflag[in]：整形，门锁的开门方式。0：卡开门; 1: 卡或指纹开门；2：指纹开门；3：指纹和卡开门。通常选0
/* Cardno [out]：长整形指针，接收卡号（发卡后分配的唯一卡代码）。可以为NULL。
/* Breakfast[in]:整形，早餐券数量，0为无早餐，1为有效时间内的每天1份，2为有效时间内的每天2份。
/* track1 [in]：字符串指针，磁卡第1轨数据，如不写此轨可为NULL。
/* track2 [in]：字符串指针，磁卡第2轨数据，如不写此轨可为NULL。
/* Lift[in]: 字符串指针，电梯，“1”表示楼层代码为“B001”，“2”表示楼层代码为“B002”，以此类推。00表示按系统默认设置，99表示所有楼层。
/* Fingerflag[in]:整型,是否采集指纹. 1-采集,0-不采集
/************************************************************************/
typedef int (WINAPI* FN_DupKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lspzGuestName,
														char* lpszGuestId, int nOverFalg, int nOpenFlag, LONG* lCardNo, int nBreakFast,
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);

typedef int (WINAPI* FN_AddKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lspzGuestName,
														char* lpszGuestId, int nOverFalg, int nOpenFlag, LONG* lCardNo, int nBreakFast,
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);

/************************************************************************/
/* 读卡
/*	room [in]：房号，6字节字符串，必须是门锁系统设置的房号。
/*	gate [in]：公共通道，字符串参数，“00”表示按默认授权通道，“99”表示授权所有公共通道，其他为指定通道代码。例如：“010203”表示授权01、02、03三个通道，最大为15。
/*	Stime [in]：起止时间，24字节字符串，格式yyyymmddhhnnyyyymmddhhnn，例如：“200012311230200101011230”表示2000年12月31日12时30分到2001年1月1日12时30分。
/*	Guestname [in]：客人姓名，最大30字节，可以为NULL。
/*	Guestid [in]：客人ID，最大30字节，可以为NULL。
/*	Overflag[in]：整形，是否覆盖当前卡上的内容。1-覆盖，当前卡不是新卡则自动注销（必须是客人卡）。0-不覆盖，不是新卡则返回。
/*	openflag[in]：整形，门锁的开门方式。0：卡开门; 1: 卡或指纹开门；2：指纹开门；3：指纹和卡开门。通常选0
/*	Cardno [out]：长整形指针，接收卡号（发卡后分配的唯一卡代码）。可以为NULL。
/*	St [out]：整形指针，接收返回的卡状态，1-正常使用，3-正常注销，4-遗失注销，5-损毁注销，6-自动注销。可以为NULL。
/*	Breakfast[in]:整形，早餐券数量，0为无早餐，1为有效时间内的每天1份，2为有效时间内的每天2份。
/*	track1 [in]：字符串指针，磁卡第1轨数据，如不写此轨可为NULL。
/*	track2 [in]：字符串指针，磁卡第2轨数据，如不写此轨可为NULL。
/*	Lift[in]: 字符串指针，“1”表示楼层代码为“B001”，“2”表示楼层代码为“B002”，以此类推。*/
/************************************************************************/
typedef int (WINAPI* FN_ReadCard)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lpszGuestName,
															char* lpszGuestId, char* lpszTrack1, char* lpszTrack2, char* lpszLift, LONG* lpszCardNo, int* st, int* nOpenFlag, int* nBreakFast);

/************************************************************************/
/* 注销卡，同时更新数据库数据                                                                     */
/* cardno [in]：被注销卡的卡号, 通过读卡获得。
/* track1 [in]：字符串指针，磁卡第1轨数据，如不写此轨可为NULL。
/* track2 [in]：字符串指针，磁卡第2轨数据，如不写此轨可为NULL。
/* fpindex [out]：整形指针，接收返回的指纹号，可以为NULL。
/* 若fpindex大于0，表示持卡人的指纹在退房后，还可以开门。需要使用注销卡注销该房间的指纹。
/************************************************************************/
typedef int (WINAPI* FN_EraseCard)(LONG lCardNo, char* lpszTrack1, char* lpszTrack2, int* fpIndex);

/************************************************************************/
/* 读取卡片中数据。                                                                     */
/* cardtype [int]：整型，卡片类型，默认写为18。
/* start [int]：整型，设置读取卡片的起始扇区和块号，start = block | (section << 8)，其中block 为块号，section为扇区。
/* len [int]：整型，设置读取卡片数据的长度。三种值 ：16,32,48。
/* buff [out]：字节数组，设置卡片的密码，获取读取的数据，其中第一个字节为密码类型，0-A密码，1-B密码。2-7为该扇区密码，当函数执行成功时，buff为读取上来的数据。
/************************************************************************/
typedef int (WINAPI* FN_ReadCardData)(int nCardType, int nStart, int nLen, char* szBuff);

/************************************************************************/
/* 卡片写入数据。                                                                     */
/* cardtype [in]：卡片的类型,默认18。
/* start [in]：整型，设置写入卡片的起始扇区和块号，start = block | (section << 8)，其中block 为块号，section为扇区。
/* len [in]：整型，设置写入卡片数据的长度。。
/* buff [int]：字节数组，设置卡片的密码，和需要写入的数据，其中第一个字节为密码类型，0-A密码，1-B密码。2-7为该扇区密码,8位以后，为需要写入卡片的数据。
/************************************************************************/
typedef int (WINAPI* FN_WriteCardData)(int nCardType, int nStart, int nLen, char* szBuff);

/************************************************************************/
/* 读取卡片的ID。                                                                     */
/* CardId [out]：长整型指针，接收返回的卡标识，可以为NULL。
/************************************************************************/
typedef int (WINAPI* FN_ReadId)(LONG* lCardId);