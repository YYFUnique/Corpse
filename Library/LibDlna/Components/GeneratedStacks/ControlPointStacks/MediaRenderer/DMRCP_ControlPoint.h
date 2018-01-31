/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2002 - 2005 Intel Corporation.  All rights reserved.
 * 
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors.  Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors.  The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and
 * licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.  No part of the Material
 * may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 * 
 * $Workfile: DMRCP_ControlPoint.h
 * $Revision: #1.0.2244.29880
 * $Author:   Intel Corporation, Intel Device Builder
 * $Date:     Wednesday, March 01, 2006
 *
 *
 *
 */
#ifndef __DMRCP_ControlPoint__
#define __DMRCP_ControlPoint__

#include "UPnPControlPointStructs.h"

/*! \file DMRCP_ControlPoint.h 
	\brief MicroStack APIs for Control Point Implementation
*/

/*! \defgroup ControlPoint Control Point Module
	\{
*/


/* Complex Type Parsers */


/* Complex Type Serializers */




/*! \defgroup CPReferenceCounter Reference Counter Methods
	\ingroup ControlPoint
	\brief Reference Counting for the UPnPDevice and UPnPService objects.
	\para
	Whenever a user application is going to keep the pointers to the UPnPDevice object that is obtained from
	the add sink (or any pointers inside them), the application <b>must</b> increment the reference counter. Failure to do so
	will lead to references to invalid pointers, when the device leaves the network.
	\{
*/
void DMRCP_AddRef(struct UPnPDevice *device);
void DMRCP_Release(struct UPnPDevice *device);
/*! \} */   



struct UPnPDevice* DMRCP_GetDevice1(struct UPnPDevice *device,int index);
int DMRCP_GetDeviceCount(struct UPnPDevice *device);
struct UPnPDevice* DMRCP_GetDeviceEx(struct UPnPDevice *device, char* DeviceType, int start,int number);
void PrintUPnPDevice(int indents, struct UPnPDevice *device);



/*! \defgroup CustomXMLTags Custom XML Tags
	\ingroup ControlPoint
	\brief Methods used to obtain metadata information from a specific UPnPDevice object.
	\{
*/
char *DMRCP_GetCustomTagFromDevice(struct UPnPDevice *d, char* FullNameSpace, char* Name);
/*! \def X_DLNACAP
\brief Custom XML Element: Local Name
*/
#define X_DLNACAP "X_DLNACAP"
/*! \def X_DLNACAP_NAMESPACE
\brief Custom XML Element: Fully Qualified Namespace
*/
#define X_DLNACAP_NAMESPACE "urn:schemas-dlna-org:device-1-0"
/*! \def X_DLNADOC
\brief Custom XML Element: Local Name
*/
#define X_DLNADOC "X_DLNADOC"
/*! \def X_DLNADOC_NAMESPACE
\brief Custom XML Element: Fully Qualified Namespace
*/
#define X_DLNADOC_NAMESPACE "urn:schemas-dlna-org:device-1-0"

char *DMRCP_GetCustomXML_X_DLNACAP(struct UPnPDevice *d);
char *DMRCP_GetCustomXML_X_DLNADOC(struct UPnPDevice *d);

/*! \} */



/*! \defgroup CPAdministration Administrative Methods
	\ingroup ControlPoint
	\brief Basic administrative functions, used to setup/configure the control point application
	\{
*/
void *DMRCP_CreateControlPoint(void *Chain, void(*A)(struct UPnPDevice*),void(*R)(struct UPnPDevice*));
struct UPnPDevice* DMRCP_GetDeviceAtUDN(void *v_CP,char* UDN);
void DMRCP__CP_IPAddressListChanged(void *CPToken);
int DMRCP_HasAction(struct UPnPService *s, char* action);
void DMRCP_UnSubscribeUPnPEvents(struct UPnPService *service);
void DMRCP_SubscribeForUPnPEvents(struct UPnPService *service, void(*callbackPtr)(struct UPnPService* service,int OK));
struct UPnPService *DMRCP_GetService(struct UPnPDevice *device, char* ServiceName, int length);

void DMRCP_SetUser(void *token, void *user);
void* DMRCP_GetUser(void *token);

struct UPnPService *DMRCP_GetService_AVTransport(struct UPnPDevice *device);
struct UPnPService *DMRCP_GetService_ConnectionManager(struct UPnPDevice *device);
struct UPnPService *DMRCP_GetService_RenderingControl(struct UPnPDevice *device);

/*! \} */


/*! \defgroup InvocationEventingMethods Invocation/Eventing Methods
	\ingroup ControlPoint
	\brief Methods used to invoke actions and receive events from a UPnPService
	\{
*/
extern void (*DMRCP_EventCallback_AVTransport_LastChange)(struct UPnPService* Service,char* LastChange);
extern void (*DMRCP_EventCallback_ConnectionManager_SourceProtocolInfo)(struct UPnPService* Service,char* SourceProtocolInfo);
extern void (*DMRCP_EventCallback_ConnectionManager_SinkProtocolInfo)(struct UPnPService* Service,char* SinkProtocolInfo);
extern void (*DMRCP_EventCallback_ConnectionManager_CurrentConnectionIDs)(struct UPnPService* Service,char* CurrentConnectionIDs);
extern void (*DMRCP_EventCallback_RenderingControl_LastChange)(struct UPnPService* Service,char* LastChange);

void DMRCP_Invoke_AVTransport_GetCurrentTransportActions(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* Actions),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_GetDeviceCapabilities(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* PlayMedia,char* RecMedia,char* RecQualityModes),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_GetMediaInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned int NrTracks,char* MediaDuration,char* CurrentURI,char* CurrentURIMetaData,char* NextURI,char* NextURIMetaData,char* PlayMedium,char* RecordMedium,char* WriteStatus),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_GetPositionInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned int Track,char* TrackDuration,char* TrackMetaData,char* TrackURI,char* RelTime,char* AbsTime,int RelCount,int AbsCount),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_GetTransportInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* CurrentTransportState,char* CurrentTransportStatus,char* CurrentSpeed),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_GetTransportSettings(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* PlayMode,char* RecQualityMode),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_Next(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_Pause(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_Play(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_Speed);
void DMRCP_Invoke_AVTransport_Previous(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_AVTransport_Seek(struct UPnPService *service, 
								   void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),
								   void* _user,
								   unsigned int InstanceID,
								   char* unescaped_Unit, 
								   char* unescaped_Target);
void DMRCP_Invoke_AVTransport_SetAVTransportURI(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_CurrentURI, char* unescaped_CurrentURIMetaData);
void DMRCP_Invoke_AVTransport_SetPlayMode(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_NewPlayMode);
void DMRCP_Invoke_AVTransport_Stop(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_ConnectionManager_ConnectionComplete(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, int ConnectionID);
void DMRCP_Invoke_ConnectionManager_GetCurrentConnectionIDs(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* ConnectionIDs),void* _user);
void DMRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status),void* _user, int ConnectionID);
void DMRCP_Invoke_ConnectionManager_GetProtocolInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* Source,char* Sink),void* _user);
void DMRCP_Invoke_ConnectionManager_PrepareForConnection(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,int ConnectionID,int AVTransportID,int RcsID),void* _user, char* unescaped_RemoteProtocolInfo, char* unescaped_PeerConnectionManager, int PeerConnectionID, char* unescaped_Direction);
void DMRCP_Invoke_RenderingControl_GetBlueVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentBlueVideoBlackLevel),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetBlueVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentBlueVideoGain),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetBrightness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentBrightness),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetColorTemperature(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentColorTemperature),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetContrast(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentContrast),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetGreenVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentGreenVideoBlackLevel),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetGreenVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentGreenVideoGain),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetHorizontalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,short CurrentHorizontalKeystone),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetLoudness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,int CurrentLoudness),void* _user, unsigned int InstanceID, char* unescaped_Channel);
void DMRCP_Invoke_RenderingControl_GetMute(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,int CurrentMute),void* _user, unsigned int InstanceID, char* unescaped_Channel);
void DMRCP_Invoke_RenderingControl_GetRedVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentRedVideoBlackLevel),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetRedVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentRedVideoGain),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetSharpness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentSharpness),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetVerticalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,short CurrentVerticalKeystone),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_GetVolume(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,unsigned short CurrentVolume),void* _user, unsigned int InstanceID, char* unescaped_Channel);
void DMRCP_Invoke_RenderingControl_GetVolumeDB(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,short CurrentVolume),void* _user, unsigned int InstanceID, char* unescaped_Channel);
void DMRCP_Invoke_RenderingControl_GetVolumeDBRange(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,short MinValue,short MaxValue),void* _user, unsigned int InstanceID, char* unescaped_Channel);
void DMRCP_Invoke_RenderingControl_ListPresets(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user,char* CurrentPresetNameList),void* _user, unsigned int InstanceID);
void DMRCP_Invoke_RenderingControl_SelectPreset(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_PresetName);
void DMRCP_Invoke_RenderingControl_SetBlueVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBlueVideoBlackLevel);
void DMRCP_Invoke_RenderingControl_SetBlueVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBlueVideoGain);
void DMRCP_Invoke_RenderingControl_SetBrightness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBrightness);
void DMRCP_Invoke_RenderingControl_SetColorTemperature(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredColorTemperature);
void DMRCP_Invoke_RenderingControl_SetContrast(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredContrast);
void DMRCP_Invoke_RenderingControl_SetGreenVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredGreenVideoBlackLevel);
void DMRCP_Invoke_RenderingControl_SetGreenVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredGreenVideoGain);
void DMRCP_Invoke_RenderingControl_SetHorizontalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, short DesiredHorizontalKeystone);
void DMRCP_Invoke_RenderingControl_SetLoudness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_Channel, int DesiredLoudness);
void DMRCP_Invoke_RenderingControl_SetMute(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_Channel, int DesiredMute);
void DMRCP_Invoke_RenderingControl_SetRedVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredRedVideoBlackLevel);
void DMRCP_Invoke_RenderingControl_SetRedVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredRedVideoGain);
void DMRCP_Invoke_RenderingControl_SetSharpness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredSharpness);
void DMRCP_Invoke_RenderingControl_SetVerticalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, short DesiredVerticalKeystone);
void DMRCP_Invoke_RenderingControl_SetVolume(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_Channel, unsigned short DesiredVolume);
void DMRCP_Invoke_RenderingControl_SetVolumeDB(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService *sender,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* unescaped_Channel, short DesiredVolume);

/*! \} */


/*! \} */
#endif
