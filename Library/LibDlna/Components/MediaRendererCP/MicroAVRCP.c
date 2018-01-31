#ifndef MICROSTACK_NO_STDAFX
#include "stdafx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif

#include "../GeneratedStacks/ControlPointStacks/MediaRenderer/UPnPControlPointStructs.h"
#include "../GeneratedStacks/ControlPointStacks/MediaRenderer/DMRCP_ControlPoint.h"
#include "../GeneratedStacks/ILib/ILibParsers.h"
#include "MicroAVRCP.h"
#include "../ProtocolInfoParser/DLNAProtocolInfo.h"
#include "../CdsObjects/CdsDidlSerializer.h"

#ifdef _POSIX
#include <semaphore.h>
#endif

struct AVR_Object *UPnP_CP;
void *UPnP_CP_chain;
void (*RendererAdded)(void *token, struct AVRenderer* r);
void (*RendererRemoved)(void *token, struct AVRenderer *r);

void HookupEvents(struct AVRenderer *);
void UpdateConnectionInfo(struct AVRendererConnection *connection);

char* RCP_PlayStateTable[] = {"PLAYING","STOPPED","PAUSED_PLAYBACK","RECORDING","TRANSITIONING","NO_MEDIA_PRESENT",NULL};
char* RCP_PlayModeTable[] = {"NORMAL","REPEAT_ALL","REPEAT_ONE","RANDOM","SHUFFLE","INTRO",NULL};

struct AVR_ConnectionState
{
	struct AVRenderer *AVR;
	void *CIDStack;
	int CMID;
};

struct AVR_VolumeState
{
	struct AVRendererConnection *connection;
	int index;
};

struct AVR_Object
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);
	void *CP;
	sem_t GlobalLock;
	void *Chain;

/* >>> Added by PDA */
	void* Tag;
/* <<< */
};

struct AVR_State
{
	struct AVRendererConnection *connection;
	void *CB;
	void *Tag;
};

void StableState_0(struct AVRendererConnection *connection)
{
	connection->Reserved = NULL;

	// yql modify begin
	//HookupEvents(connection->Parent);	// a crash
	// yql modify end
	
	connection->Parent->Reserved = 1;
	if(RendererAdded!=NULL)
	{
		RendererAdded(UPnP_CP,connection->Parent);
	}
}

unsigned short RCS_GetMaxVolume(struct AVRenderer *r)
{
	struct UPnPStateVariable *SV = r->RCS->Variables;

	while(SV!=NULL)
	{
		if(strcmp(SV->Name,"Volume")==0)
		{
			return((unsigned short)atoi(SV->Max));		
		}
		SV = SV->Next;
	}

	return(0);
}
unsigned short RCS_GetMinVolume(struct AVRenderer *r)
{
	struct UPnPStateVariable *SV = r->RCS->Variables;

	while(SV!=NULL)
	{
		if(strcmp(SV->Name,"Volume")==0)
		{
			return((unsigned short)atoi(SV->Min));		
		}
		SV = SV->Next;
	}

	return(0);
}


/* Event Processing */
void ProcessRCS_Event(struct AVRenderer *r,struct ILibXMLNode *xml,long InstanceID)
{
	struct AVRendererConnection *c=NULL,*temp=NULL;
	struct ILibXMLAttribute *rootATTR,*attr;
	int StateChanged = 0;
	int volume_index=0;
	int volume_channel_index=-1;
	int volume_value;

	unsigned short _MAX = RCS_GetMaxVolume(r);
	unsigned short _MIN = RCS_GetMinVolume(r);
	double pcnt; 

	temp = r->Connection;
	while(temp!=NULL)
	{
		if((long)temp->RCID==InstanceID)
		{
			c = temp;
			break;
		}
		temp = temp->Next;
	}

	if(c!=NULL)
	{
		if(xml->Next->StartTag!=0)
		{
			xml = xml->Next;
			while(xml!=NULL)
			{
				if(xml->NameLength==6 && memcmp(xml->Name,"Volume",6)==0)
				{
					volume_channel_index = -1;
					volume_value = -1;
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==7 && memcmp(attr->Name,"channel",7)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							for(volume_index=0;volume_index<c->ChannelCount;++volume_index)
							{
								if(strcmp(c->Channel[volume_index],attr->Value)==0)
								{
									volume_channel_index = volume_index;
								}
							}
						}
						else if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							pcnt = ((double)atoi(attr->Value)-(double)_MIN)/(double)(_MAX-_MIN);
							volume_value = (int)(pcnt*(double)100);
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
					if(volume_value != -1 && volume_channel_index != -1)
					{
						c->Volume[volume_channel_index] = (char)volume_value;
						StateChanged = -1;
					}
				}
				else if(xml->NameLength==4 && memcmp(xml->Name,"Mute",4)==0)
				{
					volume_channel_index = -1;
					volume_value = -1;
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==7 && memcmp(attr->Name,"channel",7)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							for(volume_index=0;volume_index<c->ChannelCount;++volume_index)
							{
								if(strcmp(c->Channel[volume_index],attr->Value)==0)
								{
									volume_channel_index = volume_index;
								}
							}
						}
						else if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							if(strcmp(attr->Value,"1")==0 || strcmp(attr->Value,"true")==0 || strcmp(attr->Value+1,"yes")==0)
							{
								volume_value = 1;
							}
							else
							{
								volume_value = 0;
							}
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
					if(volume_value != -1 && volume_channel_index != -1)
					{
						c->Mute[volume_channel_index] = volume_value;
						StateChanged = -1;
					}
				}
				xml = xml->Peer;
			}
		}
		if(StateChanged!=0 && r->StateChanged!=NULL)
		{
			r->StateChanged(r,c);
		}
	}
}
void ProcessAVT_Event(struct AVRenderer *r,struct ILibXMLNode *xml,long InstanceID)
{
	struct AVRendererConnection *c=NULL,*temp=NULL;
	struct ILibXMLAttribute *rootATTR,*attr;
	int StateChanged = 0,i;
	unsigned long TempLong;

	struct parser_result *pr;
	struct parser_result_field *prf;

	temp = r->Connection;
	while(temp!=NULL)
	{
		if((long)temp->AVTID==InstanceID)
		{
			c = temp;
			break;
		}
		temp = temp->Next;
	}

	if(c!=NULL)
	{
		if(xml->Next->StartTag!=0)
		{
			xml = xml->Next;
			while(xml!=NULL)
			{
				if(xml->NameLength==14 && memcmp(xml->Name,"AVTransportURI",14)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							LockRendererCP(UPnP_CP);
							
							if(c->MediaUri!=NULL) {free(c->MediaUri);}
							c->MediaUri = (char*)malloc(1+attr->ValueLength);
							memcpy(c->MediaUri,attr->Value,attr->ValueLength);
							c->MediaUri[attr->ValueLength] = '\0';
							StateChanged = 1;

							UnLockRendererCP(UPnP_CP);
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				} 
				else if(xml->NameLength==15 && memcmp(xml->Name,"CurrentTrackURI",15)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							LockRendererCP(UPnP_CP);
							
							if(c->TrackUri!=NULL) {free(c->TrackUri);}
							c->TrackUri = (char*)malloc(1+attr->ValueLength);
							memcpy(c->TrackUri,attr->Value,attr->ValueLength);
							c->TrackUri[attr->ValueLength] = '\0';
							StateChanged = 1;

							UnLockRendererCP(UPnP_CP);
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==15 && memcmp(xml->Name,"TransportStatus",15)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							i = ILibFindEntryInTable(attr->Value,c->Parent->ValidTransportStatus);
							LockRendererCP(UPnP_CP);
							if(i<0)
							{
								c->TransportStatus="__INVALID__";
							}
							else
							{
								c->TransportStatus = c->Parent->ValidTransportStatus[i];
							}
							StateChanged = 1;

							UnLockRendererCP(UPnP_CP);
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==14 && memcmp(xml->Name,"TransportState",14)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							c->PlayState = ILibFindEntryInTable(attr->Value,RCP_PlayStateTable);
							StateChanged = 1;
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==12 && memcmp(xml->Name,"CurrentTrack",12)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							ILibGetULong(attr->Value,attr->ValueLength,&TempLong);
							c->TrackNumber = (unsigned int)TempLong;
							StateChanged = 1;
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==14 && memcmp(xml->Name,"NumberOfTracks",14)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							ILibGetULong(attr->Value,attr->ValueLength,&TempLong);
							c->TotalTracks = (unsigned int)TempLong;
							StateChanged = 1;
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==15 && memcmp(xml->Name,"CurrentPlayMode",15)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							c->PlayMode = 1 << ILibFindEntryInTable(attr->Value,RCP_PlayModeTable);
							StateChanged = 1;
							break;
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
				else if(xml->NameLength==20 && memcmp(xml->Name,"CurrentTrackDuration",20)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
							attr->Value[attr->ValueLength] = '\0';
							if(strcmp(attr->Value,"NOT_IMPLEMENTED")==0)
							{
								break;
							}
							pr = ILibParseString(attr->Value,0,attr->ValueLength,":",1);
							if(pr == NULL || pr->NumResults != 3)
							{ // PDA: prevents crash on illformed duration string!!!
								if(c->Duration != 0)
								{
									c->Duration = 0;
									StateChanged = 1;
								}
								if(pr != NULL)
								{
									ILibDestructParserResults(pr);
								}
								break;
							}
							else
							{
								prf = pr->FirstResult;
								//Hour
								prf->data[prf->datalength]='\0';
								c->Duration = 3600 * atoi(prf->data);

								//Minute
								prf=prf->NextResult;
								prf->data[prf->datalength]='\0';
								c->Duration += (60 * atoi(prf->data));

								//Seconds
								prf=prf->NextResult;
								prf->data[prf->datalength]='\0';
								c->Duration += atoi(prf->data);

								ILibDestructParserResults(pr);
								StateChanged = 1;
								break;
							}
						}
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
                else if(xml->NameLength==20 && memcmp(xml->Name,"RelativeTimePosition",20)==0)
                {
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
                            c->TrackPosition[attr->ValueLength] = 0;
                            strncpy(c->TrackPosition, attr->Value, attr->ValueLength);
                            StateChanged = 1;
                            break;
                        }
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
                }
                else if(xml->NameLength == 20 && memcmp(xml->Name, "CurrentTrackMetaData", 20) == 0)
                {
                    int foundVal = 0;
					rootATTR = attr = ILibGetXMLAttributes(xml);
                    if(c->TrackObject != NULL)
                    {
                        CDS_ObjRef_Release(c->TrackObject);
                        c->TrackObject = NULL;
                    }
					while(attr!=NULL)
					{
						if(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0)
						{
                            foundVal = 1;

                            if(attr->ValueLength > 15) /* Length greater than "NOT_IMPLEMENTED" */
                            {
                                struct ILibXMLNode* rootNode = NULL;
                                int sxmlLength = 0;
                                char* sxml = (char*)malloc(attr->ValueLength + 1);

                                strncpy(sxml, attr->Value, attr->ValueLength);
                                sxml[attr->ValueLength] = 0;
                                sxmlLength = ILibInPlaceXmlUnEscape(sxml);
                                sxml[sxmlLength] = 0;

                                rootNode = ILibParseXML(sxml, 0, sxmlLength);
                                if(rootNode != NULL)
                                {
                                    if(ILibProcessXMLNodeList(rootNode) == 0)
                                    {
                                        struct ILibXMLNode* itemNode = NULL;

                                        ILibXML_BuildNamespaceLookupTable(rootNode);

                                        itemNode = rootNode->Next;

                                        if(itemNode != NULL && itemNode->NameLength == 4 && strncmp(itemNode->Name, "item", 4) == 0)
                                        {
                                            struct ILibXMLAttribute* attrs = ILibGetXMLAttributes(itemNode);
                                            if(attrs != NULL)
                                            {
                                                c->TrackObject = CDS_DeserializeDidlToObject(itemNode, attrs, 1, sxml, sxml + sxmlLength - 1);
                                                if(c->TrackObject != NULL)
                                                {
                                                    StateChanged = 1;
                                                }
                                                ILibDestructXMLAttributeList(attrs);
                                            }
                                        }
                                    }
                                    ILibDestructXMLNodeList(rootNode);
                                }

                                free(sxml);
                            }
                            if(foundVal == 1)
                            {
                                break;
                            }
                        }
						attr = attr->Next;
					}
					ILibDestructXMLAttributeList(rootATTR);
                }
				xml = xml->Peer;
			}
		}
		if(StateChanged!=0 && r->StateChanged!=NULL)
		{
			r->StateChanged(r,c);
		}
	}
}
void UPnPEventSink_RenderingControl_LastChange(struct UPnPService* Service,char* LastChange)
{
	struct ILibXMLNode *rootXML,*xml;
	struct ILibXMLAttribute *rootATTR,*attr;
	long l_InstanceID;

	ILibInPlaceXmlUnEscape(LastChange);
	rootXML = xml = ILibParseXML(LastChange,0,(int)strlen(LastChange));
	ILibProcessXMLNodeList(xml);

	if(xml->Next!=NULL && xml->Next->StartTag!=0)
	{
		do
		{
			xml = xml->Next;
			if(xml!=NULL)
			{
				if(xml->NameLength==10 && memcmp(xml->Name,"InstanceID",10)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL && !(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0))
					{
						attr = attr->Next;
					}
					if(attr!=NULL)
					{
						ILibGetLong(attr->Value,attr->ValueLength,&l_InstanceID);
						ProcessRCS_Event((struct AVRenderer*)Service->Parent->Tag,xml,l_InstanceID);
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
			}
		}while(xml!=NULL);
	}
	ILibDestructXMLNodeList(rootXML);
}
void UPnPEventSink_AVTransport_LastChange(struct UPnPService* Service,char* LastChange)
{
	struct ILibXMLNode *rootXML,*xml;
	struct ILibXMLAttribute *rootATTR,*attr;
	long l_InstanceID;
	int vx;

	ILibInPlaceXmlUnEscape(LastChange);
	rootXML = xml = ILibParseXML(LastChange,0,(int)strlen(LastChange));
	vx=ILibProcessXMLNodeList(xml);

	if(vx==0 && xml->Next->StartTag!=0)
	{
		do
		{
			xml = xml->Next;
			if(xml!=NULL)
			{
				if(xml->NameLength==10 && memcmp(xml->Name,"InstanceID",10)==0)
				{
					rootATTR = attr = ILibGetXMLAttributes(xml);
					while(attr!=NULL && !(attr->NameLength==3 && memcmp(attr->Name,"val",3)==0))
					{
						attr = attr->Next;
					}
					if(attr!=NULL)
					{
						ILibGetLong(attr->Value,attr->ValueLength,&l_InstanceID);
						ProcessAVT_Event((struct AVRenderer*)Service->Parent->Tag,xml,l_InstanceID);
					}
					ILibDestructXMLAttributeList(rootATTR);
				}
			}
		}while(xml!=NULL);
	}
	ILibDestructXMLNodeList(rootXML);
	printf("UPnP Event from %s/AVTransport/LastChange: %s\r\n",Service->Parent->FriendlyName,LastChange);
}


void HookupEvents(struct AVRenderer *r)
{
	DMRCP_SubscribeForUPnPEvents(r->AVT,NULL);
	DMRCP_SubscribeForUPnPEvents(r->RCS,NULL);
	DMRCP_SubscribeForUPnPEvents(r->CMS,NULL);
}






/* Action Respones */
void UPnPResponseSink_Generic(struct UPnPService* Service,int ErrorCode,void *User)
{
	struct AVR_State *s = (struct AVR_State*)User;
	if(s->CB!=NULL)
	{
		((void(*)(struct AVRendererConnection *sender,int ErrorCode, void *Tag))s->CB)(s->connection,ErrorCode,s->Tag);
	}
	free(s);
}

void UPnPUser_GetPositionInfo(struct UPnPService* Service,int ErrorCode,void *User,
							  unsigned int Track,char* TrackDuration,char* TrackMetaData,char* TrackURI,
							  char* RelTime,char* AbsTime,int RelCount,int AbsCount)
{
	struct AVR_State *s = (struct AVR_State*)User;
	struct parser_result *pr;
	struct parser_result_field *prf;

	int AbsoluteTime = 0;
	int RelativeTime = 0;

	if(ErrorCode!=0) 
	{
		free(s);
		return;
	}
	if(s->CB!=NULL)
	{
		if(strcmp("NOT_IMPLEMENTED",RelTime)!=0)
		{
			pr = ILibParseString(RelTime,0,(int)strlen(RelTime),":",1);
			prf = pr->FirstResult;

			//Hour
			prf->data[prf->datalength]='\0';
			RelativeTime = 3600 * atoi(prf->data);

			//Minute
			prf=prf->NextResult;
			prf->data[prf->datalength]='\0';
			RelativeTime += (60 * atoi(prf->data));

			//Seconds
			prf=prf->NextResult;
			prf->data[prf->datalength]='\0';
			RelativeTime += atoi(prf->data);

			ILibDestructParserResults(pr);
		}
		if(strcmp("NOT_IMPLEMENTED",AbsTime)!=0)
		{
			pr = ILibParseString(AbsTime,0,(int)strlen(AbsTime),":",1);
			prf = pr->FirstResult;

			//Hour
			prf->data[prf->datalength]='\0';
			AbsoluteTime = 3600 * atoi(prf->data);

			//Minute
			prf=prf->NextResult;
			prf->data[prf->datalength]='\0';
			AbsoluteTime += (60 * atoi(prf->data));

			//Seconds
			prf=prf->NextResult;
			prf->data[prf->datalength]='\0';
			AbsoluteTime += atoi(prf->data);

			ILibDestructParserResults(pr);
		}
		((void (*)(struct AVRendererConnection*,int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter,void *Tag))s->CB)(s->connection,ErrorCode,RelativeTime,AbsoluteTime,RelCount,AbsCount,s->Tag);
	}
	free(s);
}




/* Get State Responses */
void UPnPResponseSink_RenderingControl_GetMute(struct UPnPService* Service,int ErrorCode,void *User,int CurrentMute)
{
	struct AVR_VolumeState *AVRS = (struct AVR_VolumeState*)User;

	if(ErrorCode==0)
	{
		AVRS->connection->Mute[AVRS->index] = CurrentMute;
	}
	else
	{
		AVRS->connection->Error = RCP_GETMUTESTATE_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))AVRS->connection->Reserved)(AVRS->connection);
			free(AVRS);
			return;
		}
	}

	++AVRS->index;
	if(AVRS->index>=AVRS->connection->ChannelCount)
	{
		((void (*)(struct AVRendererConnection*))AVRS->connection->Reserved)(AVRS->connection);
		free(AVRS);
	}
	else
	{
		DMRCP_Invoke_RenderingControl_GetMute(AVRS->connection->Parent->RCS, &UPnPResponseSink_RenderingControl_GetMute,AVRS,AVRS->connection->RCID,AVRS->connection->Channel[AVRS->index]);
	}
}
void RCP_GetInitialMuteState(struct AVRendererConnection *connection)
{
	struct UPnPStateVariable *SV;
	struct AVR_VolumeState* AVRS;
	int i;

	SV = connection->Parent->RCS->Variables;
	while(SV!=NULL)
	{
		if(strcmp(SV->Name,"A_ARG_TYPE_Channel")==0)
		{
			connection->ChannelCount = SV->NumAllowedValues;
			if(connection->Channel==NULL) 
			{
				connection->Channel = (char**)malloc(SV->NumAllowedValues*sizeof(char*));
			}
			connection->Mute = (int*)malloc(sizeof(int)*SV->NumAllowedValues);
			for(i=0;i<SV->NumAllowedValues;++i)
			{
				connection->Channel[i] = SV->AllowedValues[i];
				connection->Mute[i] = 0;
			}
			AVRS = (struct AVR_VolumeState*)malloc(sizeof(struct AVR_VolumeState));
			AVRS->connection = connection;
			AVRS->index = 0;

			DMRCP_Invoke_RenderingControl_GetMute(connection->Parent->RCS, &UPnPResponseSink_RenderingControl_GetMute,AVRS,connection->RCID,connection->Channel[0]);
			break;
		}
		SV = SV->Next;
	}
}
void UPnPResponseSink_RenderingControl_GetVolume(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentVolume)
{
	struct AVR_VolumeState *AVRS = (struct AVR_VolumeState*)User;
	unsigned short _MAX = RCS_GetMaxVolume(AVRS->connection->Parent);
	unsigned short _MIN = RCS_GetMinVolume(AVRS->connection->Parent);

	double pcnt = ((double)CurrentVolume-(double)_MIN)/(double)(_MAX-_MIN);

	if(ErrorCode==0)
	{
		AVRS->connection->Volume[AVRS->index] = (char)(pcnt*(double)100);
	}
	else
	{
		AVRS->connection->Error = RCP_GETVOLUME_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))AVRS->connection->Reserved)(AVRS->connection);
			free(AVRS);
			return;
		}
	}

	++AVRS->index;
	if(AVRS->index>=AVRS->connection->ChannelCount)
	{
		if(RCP_SupportMute(AVRS->connection->Parent)!=0)
		{
			RCP_GetInitialMuteState(AVRS->connection);
		}
		else
		{
			((void (*)(struct AVRendererConnection*))AVRS->connection->Reserved)(AVRS->connection);
		}
		free(AVRS);
	}
	else
	{
		DMRCP_Invoke_RenderingControl_GetVolume(AVRS->connection->Parent->RCS, &UPnPResponseSink_RenderingControl_GetVolume,AVRS,AVRS->connection->RCID,AVRS->connection->Channel[AVRS->index]);
	}	
}
void UPnPResponseSink_AVTransport_GetTransportSettings(struct UPnPService* Service,int ErrorCode,void *User,char* PlayMode,char* RecQualityMode)
{
	struct AVRendererConnection *connection = (struct AVRendererConnection*)User;
	struct UPnPStateVariable *SV;
	int i;
	struct AVR_VolumeState *AVRS;


	if(ErrorCode==0)
	{
		connection->PlayMode = 1 << ILibFindEntryInTable(PlayMode,RCP_PlayModeTable);
	}
	else
	{
		connection->Error = RCP_GETTRANSPORTSETTINGS_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}

	if(RCP_SupportVolume(connection->Parent)!=0)
	{
		SV = connection->Parent->RCS->Variables;
		while(SV!=NULL)
		{
			if(strcmp(SV->Name,"A_ARG_TYPE_Channel")==0)
			{
				connection->ChannelCount = SV->NumAllowedValues;
				connection->Channel = (char**)malloc(SV->NumAllowedValues*sizeof(char*));
				connection->Volume = (char*)malloc(SV->NumAllowedValues);
				for(i=0;i<SV->NumAllowedValues;++i)
				{
					connection->Channel[i] = SV->AllowedValues[i];
					connection->Volume[i] = 0;
				}
				AVRS = (struct AVR_VolumeState*)malloc(sizeof(struct AVR_VolumeState));
				memset(AVRS, 0, sizeof(struct AVR_VolumeState));
				AVRS->connection = connection;
				AVRS->index = 0;
				DMRCP_Invoke_RenderingControl_GetVolume(connection->Parent->RCS, &UPnPResponseSink_RenderingControl_GetVolume,AVRS,connection->RCID,connection->Channel[0]);
				break;
			}
			SV = SV->Next;
		}
	}
	else if(RCP_SupportMute(connection->Parent)!=0)
	{
		connection->Volume = NULL;
		RCP_GetInitialMuteState(connection);
	}
	else
	{
		connection->ChannelCount = 0;
		connection->Channel = NULL;
		connection->Volume = NULL;
		connection->Mute = NULL;
		((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
	}
}

void UPnPResponseSink_AVTransport_GetPositionInfo(struct UPnPService* Service,int ErrorCode,void *User,unsigned int Track,char* TrackDuration,char* TrackMetaData,char* TrackURI,char* RelTime,char* AbsTime,int RelCount,int AbsCount)
{
	struct AVRendererConnection *connection = (struct AVRendererConnection*)User;
	struct parser_result *pr;
	struct parser_result_field *prf;

	if(ErrorCode==0)
	{
		connection->TrackNumber = Track;
		if(TrackURI!=NULL)
		{
			connection->TrackUri = (char*)malloc(strlen(TrackURI)+1);
			sprintf(connection->TrackUri,"%s",TrackURI);
		}
		else
		{
			connection->TrackUri = NULL;
		}

		if(TrackDuration!=NULL && strcmp(TrackDuration,"NOT_IMPLEMENTED")!=0)
		{
			pr = ILibParseString(TrackDuration,0,(int)strlen(TrackDuration),":",1);
			if(pr == NULL || pr->NumResults != 3) // PDA: prevents crash on illformed duration string!!!
			{
				if(connection->Duration != 0)
				{
					connection->Duration = 0;
				}
				if(pr != NULL)
				{
					ILibDestructParserResults(pr);
				}
			}
			else
			{
				prf = pr->FirstResult;

				//Hour
				prf->data[prf->datalength]='\0';
				connection->Duration = 3600 * atoi(prf->data);

				//Minute
				prf=prf->NextResult;
				prf->data[prf->datalength]='\0';
				connection->Duration += (60 * atoi(prf->data));

				//Seconds
				prf=prf->NextResult;
				prf->data[prf->datalength]='\0';
				connection->Duration += atoi(prf->data);

				ILibDestructParserResults(pr);
			}
		}
	}
	else
	{
		connection->Error = RCP_GETPOSITIONINFO_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}

	DMRCP_Invoke_AVTransport_GetTransportSettings(Service, &UPnPResponseSink_AVTransport_GetTransportSettings,connection,connection->AVTID);
}
void UPnPResponseSink_AVTransport_GetTransportInfo(struct UPnPService* Service,int ErrorCode,void *User,char* CurrentTransportState,char* CurrentTransportStatus,char* CurrentSpeed)
{
	struct AVRendererConnection *connection = (struct AVRendererConnection*)User;
	int i;

	if(ErrorCode==0)
	{
		connection->PlayState = ILibFindEntryInTable(CurrentTransportState,RCP_PlayStateTable);
		i = ILibFindEntryInTable(CurrentTransportStatus,connection->Parent->ValidTransportStatus);
		if(i<0)
		{
			connection->TransportStatus="__INVALID__";
		}
		else
		{
			connection->TransportStatus = connection->Parent->ValidTransportStatus[i];
		}
	}
	else
	{
		connection->Error = RCP_GETTRANSPORTINFO_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}

	DMRCP_Invoke_AVTransport_GetPositionInfo(connection->Parent->AVT, &UPnPResponseSink_AVTransport_GetPositionInfo,connection,connection->AVTID);
}
void UPnPResponseSink_AVTransport_GetMediaInfo(struct UPnPService* Service,int ErrorCode,void *User,unsigned int NrTracks,char* MediaDuration,char* CurrentURI,char* CurrentURIMetaData,char* NextURI,char* NextURIMetaData,char* PlayMedium,char* RecordMedium,char* WriteStatus)
{
	struct AVRendererConnection *connection = (struct AVRendererConnection*)User;

	if(ErrorCode==0)
	{
		if(CurrentURI!=NULL)
		{
			connection->MediaUri = (char*)malloc(strlen(CurrentURI)+1);
			sprintf(connection->MediaUri,"%s",CurrentURI);
		}
		else
		{
			connection->MediaUri = NULL;
		}
		connection->TotalTracks = NrTracks;
	}
	else
	{
		connection->MediaUri = NULL;
		connection->TotalTracks = 0;
		connection->Error = RCP_GETMEDIAINFO_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}

	DMRCP_Invoke_AVTransport_GetTransportInfo(connection->Parent->AVT, &UPnPResponseSink_AVTransport_GetTransportInfo,connection,connection->AVTID);
}


void ReAcquireStateSink(struct AVRendererConnection *connection)
{
	struct AVR_State *s = connection->Reserved2;
	connection->Reserved = connection->Reserved2 = NULL;

	if(s->CB!=NULL)
	{
		((void(*)(struct AVRendererConnection*, int,void*))s->CB)(connection,connection->Error,s->Tag);
	}
	free(s);
}
void UPnPResponseSink_AVTransport_GetDeviceCapabilities(struct UPnPService* Service,int ErrorCode,void *User,char* PlayMedia,char* RecMedia,char* RecQualityModes)
{
	struct AVRendererConnection *connection = (struct AVRendererConnection*)User;
	struct parser_result *pr;
	struct parser_result_field *prf;
	int i,x;

	if(ErrorCode!=0)
	{
		connection->Error = RCP_GETDEVICECAPABILITIES_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}
	else
	{
		pr = ILibParseString(PlayMedia,0,(int)strlen(PlayMedia),",",1);
		if(connection->PlayMedia!=NULL) {free(connection->PlayMedia);}
		connection->PlayMedia = (char**)malloc(pr->NumResults*sizeof(char*));
		connection->PlayMediaLength = pr->NumResults;
		prf = pr->FirstResult;
		x=0;
		while(prf!=NULL)
		{
			prf->data[prf->datalength]='\0';
			i = ILibFindEntryInTable(prf->data,connection->Parent->PlayMedia);
			if(i==-1)
			{
				connection->PlayMedia[x] = "__INVALID__";
			}
			else
			{
				connection->PlayMedia[x] = connection->Parent->PlayMedia[i];
			}
			prf = prf->NextResult;
			++x;
		}
		ILibDestructParserResults(pr);


		pr = ILibParseString(RecMedia,0,(int)strlen(RecMedia),",",1);
		if(connection->RecordMedia!=NULL) {free(connection->RecordMedia);}
		connection->RecordMedia = (char**)malloc(pr->NumResults*sizeof(char*));
		connection->RecordMediaLength = pr->NumResults;
		prf = pr->FirstResult;
		x=0;
		while(prf!=NULL)
		{
			prf->data[prf->datalength]='\0';
			i = ILibFindEntryInTable(prf->data,connection->Parent->RecordMedia);
			if(i==-1)
			{
				connection->RecordMedia[x] = "__INVALID__";
			}
			else
			{
				connection->RecordMedia[x] = connection->Parent->RecordMedia[i];
			}
			prf = prf->NextResult;
			++x;
		}
		ILibDestructParserResults(pr);
	}

	// Get Media Info
	DMRCP_Invoke_AVTransport_GetMediaInfo(connection->Parent->AVT, &UPnPResponseSink_AVTransport_GetMediaInfo,connection,connection->AVTID);
}


void RCP_ReAcquireConnectionState(struct AVRendererConnection *connection, void *Tag, void (*ReAcquireConnectionStateSink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));

	s->CB = ReAcquireConnectionStateSink;
	s->connection = connection;
	s->Tag = Tag;
	connection->Reserved2 = s;
	connection->Reserved = &ReAcquireStateSink;
	UpdateConnectionInfo(connection);
}

void StableState_1(struct AVRendererConnection *connection)
{
	connection->Reserved = NULL;
	if(connection->Next==NULL)
	{
		//DONE
		HookupEvents(connection->Parent);
		connection->Parent->Reserved = 1;
		if(RendererAdded!=NULL)
		{
			RendererAdded(UPnP_CP,connection->Parent);
		}
	}
	else
	{
		connection->Next->Reserved = &StableState_1;
		UpdateConnectionInfo(connection->Next);
	}
}
void UPnPResponseSink_ConnectionManager_GetCurrentConnectionInfo(struct UPnPService* Service,int ErrorCode,void *User,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status)
{
	struct AVR_ConnectionState *ACS = (struct AVR_ConnectionState*)User;
	struct AVRendererConnection *c;
	int *id;
	int ok;

	if(ErrorCode==0)
	{
		c = (struct AVRendererConnection*)malloc(sizeof(struct AVRendererConnection));
		memset(c,0,sizeof(struct AVRendererConnection));
		
		c->ProtocolInfo = (char*)malloc(1+strlen(ProtocolInfo));
		strcpy(c->ProtocolInfo,ProtocolInfo);
		c->Parent = ACS->AVR;
		c->AVTID = AVTransportID;
		c->CMID = ACS->CMID;
		c->RCID = RcsID;
		c->Prev = NULL;
		c->Next = ACS->AVR->Connection;
		ACS->AVR->Connection = c;
		if(c->Next!=NULL) {c->Next->Prev = ACS->AVR->Connection;}
	}
	else
	{
		if(ErrorCode<300)
		{
			ok = 0;
			while(ok==0)
			{
				id = (int*)ILibPopStack(&(ACS->CIDStack));
				if(id!=NULL) 
				{
					free(id);
				}
				else
				{
					ok=1;
				}
			}
			free(ACS);
			return;
		}
	}

	id = (int*)ILibPopStack(&(ACS->CIDStack));
	if(id!=NULL)
	{
		// More Connections to process
		ACS->CMID = *id;
		DMRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(ACS->AVR->CMS, &UPnPResponseSink_ConnectionManager_GetCurrentConnectionInfo,ACS,*id);
		free(id);
	}
	else
	{
		// No More Connections
		if(ACS->AVR->Connection!=NULL)
		{
			ACS->AVR->Connection->Reserved = &StableState_1;
			UpdateConnectionInfo(ACS->AVR->Connection);
		}
		else
		{
			HookupEvents(ACS->AVR);

			if(RendererAdded!=NULL)
			{
				RendererAdded(UPnP_CP,ACS->AVR);
			}
		}
		free(ACS);
	}
}
void UPnPResponseSink_ConnectionManager_GetCurrentConnectionIDs(struct UPnPService* Service,int ErrorCode,void *User,char* ConnectionIDs)
{
	struct AVRenderer *AVR = (struct AVRenderer*)User;
	struct parser_result *r;
	struct parser_result_field *f;
	struct AVR_ConnectionState *ACS;
	int *id;

	if(ConnectionIDs==NULL)
	{
		HookupEvents(AVR);
		AVR->Reserved = 1;

		if(RendererAdded!=NULL)
		{
			RendererAdded(UPnP_CP,AVR);
		}
		return;
	}
	
	r = ILibParseString(ConnectionIDs,0,(int)strlen(ConnectionIDs),",",1);
	ACS = (struct AVR_ConnectionState*)malloc(sizeof(struct AVR_ConnectionState));
	memset(ACS, 0, sizeof(struct AVR_ConnectionState));
	ACS->AVR = AVR;
	ILibCreateStack(&(ACS->CIDStack));

	f = r->FirstResult;
	while(f!=NULL)
	{
		f->data[f->datalength]='\0';
		id = (int*)malloc(sizeof(int));
		*id = atoi(f->data);
		ILibPushStack(&(ACS->CIDStack),id);
		f = f->NextResult;
	}
	id = (int*)ILibPopStack(&(ACS->CIDStack));
	ACS->CMID = *id;
	DMRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(AVR->CMS, &UPnPResponseSink_ConnectionManager_GetCurrentConnectionInfo,ACS,*id);
	free(id);
	ILibDestructParserResults(r);
}
void UPnPResponseSink_ConnectionManager_GetProtocolInfo(struct UPnPService* Service,int ErrorCode,void *User,char* Source,char* Sink)
{
	struct parser_result *r;
	struct parser_result_field *f;
	struct AVRenderer *AVR = (struct AVRenderer*)User;
	int i;

	if(ErrorCode!=0 || Sink==NULL)
	{
		// SomeType of Error
		return;
	}

	//printf("UPnP Invoke Response: ConnectionManager/GetProtocolInfo(%s,%s)\r\n",Source,Sink);
	r = ILibParseString(Sink,0,(int)strlen(Sink),",",1);

	AVR->ProtocolInfo = (char**)malloc((r->NumResults)*sizeof(char*));
	AVR->ProtocolInfoLength = r->NumResults;
	f = r->FirstResult;
	for(i=0;i<r->NumResults;++i)
	{
		AVR->ProtocolInfo[i] = (char*)malloc(1+f->datalength);
		memcpy(AVR->ProtocolInfo[i],f->data,f->datalength);
		AVR->ProtocolInfo[i][f->datalength] = '\0';
		f = f->NextResult;
	}
	ILibDestructParserResults(r);

	AVR->ImplementsPrepareForConnection = DMRCP_HasAction(AVR->CMS, "PrepareForConnection");

	if(AVR->ImplementsPrepareForConnection==0)
	{
		AVR->Connection = (struct AVRendererConnection*)malloc(sizeof(struct AVRendererConnection));
		memset(AVR->Connection,0,sizeof(struct AVRendererConnection));
		
		AVR->Connection->Parent = AVR;
		AVR->Connection->Reserved = (void*)&StableState_0;
		UpdateConnectionInfo(AVR->Connection);
	}
	else
	{
		AVR->Connection = NULL;
		DMRCP_Invoke_ConnectionManager_GetCurrentConnectionIDs(AVR->CMS, &UPnPResponseSink_ConnectionManager_GetCurrentConnectionIDs,AVR);
	}
}



void FinishedPrepareForConnection(struct AVRendererConnection *connection)
{
	struct AVRenderer *r = connection->Parent;
	char id[16];
	int idLength;
	void *Tag=NULL;
	struct AVR_State *s = (struct AVR_State*)connection->Reserved2;
	int ErrorCode=connection->Error;

	if(s!=NULL) 
	{
		Tag = s->Tag;
		free(s);
	}

	connection->Reserved = NULL;
	connection->Reserved2 = NULL;

	idLength = sprintf(id,"%d",connection->CMID);
	ILibDeleteEntry(r->FetchingTable,id,idLength);

	connection->Prev = NULL;
	connection->Next = r->Connection;
	if(r->Connection!=NULL)
	{
		r->Connection->Prev = connection;
	}
	r->Connection = connection;
	
	if(r->ConnectionAdded!=NULL)
	{
		r->ConnectionAdded(r, connection, ErrorCode ,Tag);
	}
}
void ProcessUpdateConnectionInfo_GetCurrentConnectionInfo(struct UPnPService* Service,int ErrorCode,void *User,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status)
{
	struct AVRendererConnection* connection = (struct AVRendererConnection*)User;
	if(ErrorCode==0)
	{
		if(connection->ProtocolInfo!=NULL) {free(connection->ProtocolInfo);}
		connection->ProtocolInfo = NULL;
		if(ProtocolInfo!=NULL)
		{
			connection->ProtocolInfo = (char*)malloc(1+(int)strlen(ProtocolInfo));
			strcpy(connection->ProtocolInfo,ProtocolInfo);
		}
	}
	else
	{
		connection->Error = RCP_GETCONNECTIONINFO_FAILED;
		if(ErrorCode<300)
		{
			// Do NOT Continue
			((void (*)(struct AVRendererConnection*))connection->Reserved)(connection);
			return;
		}
	}


	DMRCP_Invoke_AVTransport_GetDeviceCapabilities(connection->Parent->AVT, &UPnPResponseSink_AVTransport_GetDeviceCapabilities,connection,connection->AVTID);
}

void ProcessAddedConnection_GetCurrentConnectionInfo(struct UPnPService* Service,int ErrorCode,void *User,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status)
{
	struct AVR_ConnectionState *s = (struct AVR_ConnectionState*)User;

	struct AVRenderer *r = s->AVR;
	struct AVRendererConnection *c;

	if(ErrorCode==0)
	{
		c = (struct AVRendererConnection*)malloc(sizeof(struct AVRendererConnection));
		memset(c,0,sizeof(struct AVRendererConnection));
		c->Parent = r;
		c->AVTID = AVTransportID;
		c->CMID = s->CMID;
		c->RCID = RcsID;
		
		c->ProtocolInfo = (char*)malloc(1+(int)strlen(ProtocolInfo));
		strcpy(c->ProtocolInfo,ProtocolInfo);

		c->Reserved = &FinishedPrepareForConnection;

		UpdateConnectionInfo(c);
	}
	free(s);
}

void ProcessRemovedConnection(struct AVRenderer *AVR, int id)
{
	struct AVRendererConnection *c = AVR->Connection;

	while(c!=NULL)
	{
		if(c->CMID==id)
		{
			if(c->Prev==NULL)
			{
				// First in List
				AVR->Connection = c->Next;
				if(c->Next!=NULL)
				{
					c->Next->Prev = NULL;
				}
			}
			else
			{
				// Middle of List
				c->Prev->Next = c->Next;
				if(c->Next!=NULL)
				{
					c->Next->Prev = c->Prev;
				}
			}
			c->Next = NULL;
			c->Prev = NULL;
			if(AVR->ConnectionRemoved!=NULL)
			{
				AVR->ConnectionRemoved(AVR,c);
			}
			if(c->ProtocolInfo!=NULL) {free(c->ProtocolInfo);}
			if(c->MediaUri!=NULL) {free(c->MediaUri);}
			if(c->TrackUri!=NULL) {free(c->TrackUri);}
			if(c->Channel!=NULL) {free(c->Channel);}
			if(c->Volume!=NULL) {free(c->Volume);}
			if(c->Mute!=NULL) {free(c->Mute);}
			if(c->PlayMedia!=NULL) {free(c->PlayMedia);}
			if(c->RecordMedia!=NULL) {free(c->RecordMedia);}
            if(c->TrackObject!=NULL)
            {
                CDS_ObjRef_Release(c->TrackObject);
            }
			free(c);
			break;
		}
		c = c->Next;
	}
}
void ProcessAddedConnection(struct AVRenderer *AVR, int id)
{
	struct AVR_ConnectionState *s = (struct AVR_ConnectionState*)malloc(sizeof(struct AVR_ConnectionState));
	memset(s, 0, sizeof(struct AVR_ConnectionState));
	s->AVR = AVR;
	s->CMID = id;
	DMRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(AVR->CMS, ProcessAddedConnection_GetCurrentConnectionInfo,s, id);
}
void ProcessConnectionIDEvents(struct AVRenderer *AVR, char* CurrentConnectionIDs, int IgnoreID)
{
	int AddedIDsLength=0;
	int *AddedIDs = NULL;
	int *RemovedIDs = NULL;
	int RemovedIDsLength=0;

	struct AVRendererConnection *AVC;
	struct parser_result* pr;
	struct parser_result_field *prf;
	int i,nc,has,id;

	pr = ILibParseString(CurrentConnectionIDs,0,(int)strlen(CurrentConnectionIDs),",",1);

	nc=0;
	AVC = AVR->Connection;
	while(AVC!=NULL)
	{
		++nc;
		AVC = AVC->Next;
	}
	if(nc!=0) {RemovedIDs = (int*)malloc(nc*sizeof(int));}
	if(pr->NumResults!=0) {AddedIDs = (int*)malloc(pr->NumResults*sizeof(int));}

	prf = pr->FirstResult;
	while(prf!=NULL)
	{
		prf->data[prf->datalength]='\0';
		id = atoi(prf->data);
		has = 0;
		AVC = AVR->Connection;
		while(AVC!=NULL)
		{
			if(AVC->CMID==id)
			{
				has = 1;
				break;
			}
			AVC = AVC->Next;
		}
		if(IgnoreID != id && has==0 && ILibHasEntry(AVR->FetchingTable,prf->data,prf->datalength)==0)
		{
			//Added Connection ID
			ILibAddEntry(AVR->FetchingTable,prf->data,prf->datalength,NULL);
			AddedIDs[AddedIDsLength] = id;
			++AddedIDsLength;
		}
		prf = prf->NextResult;
	}
	prf = pr->FirstResult;
	AVC = AVR->Connection;
	while(AVC!=NULL)
	{
		has=0;
		while(prf!=NULL)
		{
			id = atoi(prf->data);
			if(id==AVC->CMID)
			{
				has=1;
				break;
			}
			prf = prf->NextResult;
		}
		if(IgnoreID != AVC->CMID && has==0)
		{
			//Removed Connection ID
			RemovedIDs[RemovedIDsLength] = AVC->CMID;
			++RemovedIDsLength;
		}
		AVC = AVC->Next;
	}

	for(i=0;i<AddedIDsLength;++i)
	{
		ProcessAddedConnection(AVR,AddedIDs[i]);
	}
	for(i=0;i<RemovedIDsLength;++i)
	{
		ProcessRemovedConnection(AVR,RemovedIDs[i]);
	}
	if(AddedIDs!=NULL) {free(AddedIDs);}
	if(RemovedIDs!=NULL) {free(RemovedIDs);}
	ILibDestructParserResults(pr);
}







void UPnPResponseSink_ConnectionManager_PrepareForConnection(struct UPnPService* Service,int ErrorCode,void *User,int ConnectionID,int AVTransportID,int RcsID)
{
	struct AVR_State *s = (struct AVR_State*)User;
	struct AVRenderer *renderer = (struct AVRenderer*)s->CB;
	struct AVRendererConnection *connection;


	if(ErrorCode !=0 )
	{
		LockRendererCP(renderer->CP);
		--renderer->PendingPrepareForConnection;
		UnLockRendererCP(renderer->CP);
		if(renderer->ConnectionAdded!=NULL)
		{
			renderer->ConnectionAdded(renderer,NULL,RCP_PREPAREFORCONNECTION_FAILED,s->Tag);
		}
		free(s);
		return;
	}

	connection = (struct AVRendererConnection*)malloc(sizeof(struct AVRendererConnection));
	memset(connection,0,sizeof(struct AVRendererConnection));
	connection->AVTID = AVTransportID;
	connection->CMID = ConnectionID;
	connection->Parent = renderer;
	connection->RCID = RcsID;
	connection->Reserved = &FinishedPrepareForConnection;
	connection->Reserved2 = s;
	s->connection = connection;

	LockRendererCP(renderer->CP);
	--renderer->PendingPrepareForConnection;
	UnLockRendererCP(renderer->CP);
	if(renderer->PendingPrepareForConnection==0 && renderer->PendingConnectionEvent!=NULL)
	{
		ProcessConnectionIDEvents(renderer,renderer->PendingConnectionEvent,ConnectionID);
		free(renderer->PendingConnectionEvent);
		renderer->PendingConnectionEvent=NULL;
	}

	UpdateConnectionInfo(connection);
}














void UPnPResponseSink_ConnectionManager_ConnectionComplete(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: ConnectionManager/ConnectionComplete()\r\n");
}
void UPnPResponseSink_RenderingControl_GetHorizontalKeystone(struct UPnPService* Service,int ErrorCode,void *User,short CurrentHorizontalKeystone)
{
	printf("UPnP Invoke Response: RenderingControl/GetHorizontalKeystone(%d)\r\n",CurrentHorizontalKeystone);
}

void UPnPResponseSink_RenderingControl_SelectPreset(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SelectPreset()\r\n");
}

void UPnPResponseSink_RenderingControl_ListPresets(struct UPnPService* Service,int ErrorCode,void *User,char* CurrentPresetNameList)
{
	printf("UPnP Invoke Response: RenderingControl/ListPresets(%s)\r\n",CurrentPresetNameList);
}

void UPnPResponseSink_RenderingControl_SetVolumeDB(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetVolumeDB()\r\n");
}

void UPnPResponseSink_RenderingControl_SetRedVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetRedVideoBlackLevel()\r\n");
}

void UPnPResponseSink_RenderingControl_SetContrast(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetContrast()\r\n");
}

void UPnPResponseSink_RenderingControl_SetLoudness(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetLoudness()\r\n");
}

void UPnPResponseSink_RenderingControl_SetBrightness(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetBrightness()\r\n");
}

void UPnPResponseSink_RenderingControl_GetLoudness(struct UPnPService* Service,int ErrorCode,void *User,int CurrentLoudness)
{
	printf("UPnP Invoke Response: RenderingControl/GetLoudness(%d)\r\n",CurrentLoudness);
}

void UPnPResponseSink_RenderingControl_GetColorTemperature(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentColorTemperature)
{
	printf("UPnP Invoke Response: RenderingControl/GetColorTemperature(%u)\r\n",CurrentColorTemperature);
}

void UPnPResponseSink_RenderingControl_GetSharpness(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentSharpness)
{
	printf("UPnP Invoke Response: RenderingControl/GetSharpness(%u)\r\n",CurrentSharpness);
}

void UPnPResponseSink_RenderingControl_GetContrast(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentContrast)
{
	printf("UPnP Invoke Response: RenderingControl/GetContrast(%u)\r\n",CurrentContrast);
}

void UPnPResponseSink_RenderingControl_GetGreenVideoGain(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentGreenVideoGain)
{
	printf("UPnP Invoke Response: RenderingControl/GetGreenVideoGain(%u)\r\n",CurrentGreenVideoGain);
}

void UPnPResponseSink_RenderingControl_SetRedVideoGain(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetRedVideoGain()\r\n");
}

void UPnPResponseSink_RenderingControl_SetGreenVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetGreenVideoBlackLevel()\r\n");
}

void UPnPResponseSink_RenderingControl_GetVolumeDBRange(struct UPnPService* Service,int ErrorCode,void *User,short MinValue,short MaxValue)
{
	printf("UPnP Invoke Response: RenderingControl/GetVolumeDBRange(%d,%d)\r\n",MinValue,MaxValue);
}

void UPnPResponseSink_RenderingControl_GetRedVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentRedVideoBlackLevel)
{
	printf("UPnP Invoke Response: RenderingControl/GetRedVideoBlackLevel(%u)\r\n",CurrentRedVideoBlackLevel);
}

void UPnPResponseSink_RenderingControl_GetBlueVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentBlueVideoBlackLevel)
{
	printf("UPnP Invoke Response: RenderingControl/GetBlueVideoBlackLevel(%u)\r\n",CurrentBlueVideoBlackLevel);
}

void UPnPResponseSink_RenderingControl_GetBlueVideoGain(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentBlueVideoGain)
{
	printf("UPnP Invoke Response: RenderingControl/GetBlueVideoGain(%u)\r\n",CurrentBlueVideoGain);
}

void UPnPResponseSink_RenderingControl_SetBlueVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetBlueVideoBlackLevel()\r\n");
}

void UPnPResponseSink_RenderingControl_SetBlueVideoGain(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetBlueVideoGain()\r\n");
}

void UPnPResponseSink_RenderingControl_GetVerticalKeystone(struct UPnPService* Service,int ErrorCode,void *User,short CurrentVerticalKeystone)
{
	printf("UPnP Invoke Response: RenderingControl/GetVerticalKeystone(%d)\r\n",CurrentVerticalKeystone);
}

void UPnPResponseSink_RenderingControl_SetVerticalKeystone(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetVerticalKeystone()\r\n");
}

void UPnPResponseSink_RenderingControl_GetBrightness(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentBrightness)
{
	printf("UPnP Invoke Response: RenderingControl/GetBrightness(%u)\r\n",CurrentBrightness);
}

void UPnPResponseSink_RenderingControl_GetVolumeDB(struct UPnPService* Service,int ErrorCode,void *User,short CurrentVolume)
{
	printf("UPnP Invoke Response: RenderingControl/GetVolumeDB(%d)\r\n",CurrentVolume);
}

void UPnPResponseSink_RenderingControl_GetGreenVideoBlackLevel(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentGreenVideoBlackLevel)
{
	printf("UPnP Invoke Response: RenderingControl/GetGreenVideoBlackLevel(%u)\r\n",CurrentGreenVideoBlackLevel);
}

void UPnPResponseSink_RenderingControl_GetRedVideoGain(struct UPnPService* Service,int ErrorCode,void *User,unsigned short CurrentRedVideoGain)
{
	printf("UPnP Invoke Response: RenderingControl/GetRedVideoGain(%u)\r\n",CurrentRedVideoGain);
}

void UPnPResponseSink_RenderingControl_SetGreenVideoGain(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetGreenVideoGain()\r\n");
}

void UPnPResponseSink_RenderingControl_SetSharpness(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetSharpness()\r\n");
}

void UPnPResponseSink_RenderingControl_SetHorizontalKeystone(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetHorizontalKeystone()\r\n");
}

void UPnPResponseSink_RenderingControl_SetColorTemperature(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: RenderingControl/SetColorTemperature()\r\n");
}

void UPnPResponseSink_AVTransport_GetCurrentTransportActions(struct UPnPService* Service,int ErrorCode,void *User,char* Actions)
{
	printf("UPnP Invoke Response: AVTransport/GetCurrentTransportActions(%s)\r\n",Actions);
}

void UPnPResponseSink_AVTransport_Previous(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: AVTransport/Previous()\r\n");
}

void UPnPResponseSink_AVTransport_Next(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: AVTransport/Next()\r\n");
}

void UPnPResponseSink_AVTransport_Stop(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: AVTransport/Stop()\r\n");
}


void UPnPResponseSink_AVTransport_Seek(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: AVTransport/Seek()\r\n");
}

void UPnPResponseSink_AVTransport_Pause(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("UPnP Invoke Response: AVTransport/Pause()\r\n");
}

void UPnPEventSink_ConnectionManager_SourceProtocolInfo(struct UPnPService* Service,char* SourceProtocolInfo)
{
	printf("UPnP Event from %s/ConnectionManager/SourceProtocolInfo: %s\r\n",Service->Parent->FriendlyName,SourceProtocolInfo);
}

void UPnPEventSink_ConnectionManager_SinkProtocolInfo(struct UPnPService* Service,char* SinkProtocolInfo)
{
	printf("UPnP Event from %s/ConnectionManager/SinkProtocolInfo: %s\r\n",Service->Parent->FriendlyName,SinkProtocolInfo);
}


void UPnPEventSink_ConnectionManager_CurrentConnectionIDs(struct UPnPService* Service,char* CurrentConnectionIDs)
{
	struct AVRenderer *AVR = (struct AVRenderer*)Service->Parent->Tag;

	LockRendererCP(AVR->CP);
	if(AVR->PendingPrepareForConnection==0)
	{
		ProcessConnectionIDEvents(AVR,CurrentConnectionIDs,0);
	}
	else
	{
		if(AVR->PendingConnectionEvent!=NULL) {free(AVR->PendingConnectionEvent);}
		AVR->PendingConnectionEvent = (char*)malloc(strlen(CurrentConnectionIDs)+1);
		strcpy(AVR->PendingConnectionEvent,CurrentConnectionIDs);
	}
	UnLockRendererCP(AVR->CP);


	printf("UPnP Event from %s/ConnectionManager/CurrentConnectionIDs: %s\r\n",Service->Parent->FriendlyName,CurrentConnectionIDs);
}


/* Called whenever a new device on the correct type is discovered */
void UPnPDeviceDiscoverSink(struct UPnPDevice *device)
{
	struct AVRenderer *AVR = (struct AVRenderer*)malloc(sizeof(struct AVRenderer));
	int i;
	struct UPnPStateVariable *SV;
	
	printf("**<< UPnP A/V Renderer Added: %s >>**\r\n", device->FriendlyName);
	/* 	UPnPPrintUPnPDevice(0,device); */
	
	
	memset(AVR,0,sizeof(struct AVRenderer));
	AVR->EventTimer = ILibCreateLifeTime(((struct AVR_Object*)UPnP_CP)->Chain);
	AVR->CP = UPnP_CP;
	device->Tag = AVR;
	AVR->FriendlyName = device->FriendlyName;
	AVR->FetchingTable = ILibInitHashTree();

	AVR->RCS = DMRCP_GetService_RenderingControl(device);
	AVR->AVT = DMRCP_GetService_AVTransport(device);
	AVR->CMS = DMRCP_GetService_ConnectionManager(device);
	AVR->device = device;

	if(AVR->AVT!=NULL)
	{
		SV = AVR->AVT->Variables;
		while(SV!=NULL)
		{
			if(strcmp(SV->Name,"CurrentPlayMode")==0)
			{
				for(i=0;i<SV->NumAllowedValues;++i)
				{
					AVR->SupportedPlayModeMask = AVR->SupportedPlayModeMask | (1 << ILibFindEntryInTable(SV->AllowedValues[i],RCP_PlayModeTable));
				}
			}
			else if(strcmp(SV->Name,"PlaybackStorageMedium")==0)
			{
				AVR->PlayMedia = (char**)malloc((1+SV->NumAllowedValues)*sizeof(char*));
				AVR->PlayMediaLength = SV->NumAllowedValues;
				for(i=0;i<SV->NumAllowedValues;++i)
				{
					AVR->PlayMedia[i] = (char*)malloc(1+strlen(SV->AllowedValues[i]));
					strcpy(AVR->PlayMedia[i],SV->AllowedValues[i]);
				}
				AVR->PlayMedia[i] = NULL;
			}
			else if(strcmp(SV->Name,"RecordStorageMedium")==0)
			{
				AVR->RecordMedia = (char**)malloc((1+SV->NumAllowedValues)*sizeof(char*));
				AVR->RecordMediaLength = SV->NumAllowedValues;
				for(i=0;i<SV->NumAllowedValues;++i)
				{
					AVR->RecordMedia[i] = (char*)malloc(1+strlen(SV->AllowedValues[i]));
					strcpy(AVR->RecordMedia[i],SV->AllowedValues[i]);
				}
				AVR->RecordMedia[i] = NULL;

			}
			else if(strcmp(SV->Name,"TransportStatus")==0)
			{
				AVR->ValidTransportStatus = (char**)malloc((1+SV->NumAllowedValues)*sizeof(char*));
				AVR->ValidTransportStatusLength = SV->NumAllowedValues;
				for(i=0;i<SV->NumAllowedValues;++i)
				{
					AVR->ValidTransportStatus[i] = (char*)malloc(1+strlen(SV->AllowedValues[i]));
					strcpy(AVR->ValidTransportStatus[i],SV->AllowedValues[i]);
				}
				AVR->ValidTransportStatus[i] = NULL;
			}
			SV = SV->Next;
		}
	}

	DMRCP_Invoke_ConnectionManager_GetProtocolInfo(AVR->CMS, &UPnPResponseSink_ConnectionManager_GetProtocolInfo,AVR);

	
	/*
	MRCP_Invoke_RenderingControl_GetHorizontalKeystone(tempService, &UPnPResponseSink_RenderingControl_GetHorizontalKeystone,NULL,250);
	MRCP_Invoke_RenderingControl_GetVolume(tempService, &UPnPResponseSink_RenderingControl_GetVolume,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_SelectPreset(tempService, &UPnPResponseSink_RenderingControl_SelectPreset,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_SetVolume(tempService, &UPnPResponseSink_RenderingControl_SetVolume,NULL,250,"Sample String",250);
	MRCP_Invoke_RenderingControl_ListPresets(tempService, &UPnPResponseSink_RenderingControl_ListPresets,NULL,250);
	MRCP_Invoke_RenderingControl_SetVolumeDB(tempService, &UPnPResponseSink_RenderingControl_SetVolumeDB,NULL,250,"Sample String",25000);
	MRCP_Invoke_RenderingControl_SetRedVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_SetRedVideoBlackLevel,NULL,250,250);
	MRCP_Invoke_RenderingControl_SetContrast(tempService, &UPnPResponseSink_RenderingControl_SetContrast,NULL,250,250);
	MRCP_Invoke_RenderingControl_SetLoudness(tempService, &UPnPResponseSink_RenderingControl_SetLoudness,NULL,250,"Sample String",1);
	MRCP_Invoke_RenderingControl_SetBrightness(tempService, &UPnPResponseSink_RenderingControl_SetBrightness,NULL,250,250);
	MRCP_Invoke_RenderingControl_GetLoudness(tempService, &UPnPResponseSink_RenderingControl_GetLoudness,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_GetColorTemperature(tempService, &UPnPResponseSink_RenderingControl_GetColorTemperature,NULL,250);
	MRCP_Invoke_RenderingControl_GetSharpness(tempService, &UPnPResponseSink_RenderingControl_GetSharpness,NULL,250);
	MRCP_Invoke_RenderingControl_GetContrast(tempService, &UPnPResponseSink_RenderingControl_GetContrast,NULL,250);
	MRCP_Invoke_RenderingControl_GetGreenVideoGain(tempService, &UPnPResponseSink_RenderingControl_GetGreenVideoGain,NULL,250);
	MRCP_Invoke_RenderingControl_SetRedVideoGain(tempService, &UPnPResponseSink_RenderingControl_SetRedVideoGain,NULL,250,250);
	MRCP_Invoke_RenderingControl_SetGreenVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_SetGreenVideoBlackLevel,NULL,250,250);
	MRCP_Invoke_RenderingControl_GetVolumeDBRange(tempService, &UPnPResponseSink_RenderingControl_GetVolumeDBRange,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_GetRedVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_GetRedVideoBlackLevel,NULL,250);
	MRCP_Invoke_RenderingControl_GetBlueVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_GetBlueVideoBlackLevel,NULL,250);
	MRCP_Invoke_RenderingControl_GetBlueVideoGain(tempService, &UPnPResponseSink_RenderingControl_GetBlueVideoGain,NULL,250);
	MRCP_Invoke_RenderingControl_SetBlueVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_SetBlueVideoBlackLevel,NULL,250,250);
	MRCP_Invoke_RenderingControl_GetMute(tempService, &UPnPResponseSink_RenderingControl_GetMute,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_SetBlueVideoGain(tempService, &UPnPResponseSink_RenderingControl_SetBlueVideoGain,NULL,250,250);
	MRCP_Invoke_RenderingControl_GetVerticalKeystone(tempService, &UPnPResponseSink_RenderingControl_GetVerticalKeystone,NULL,250);
	MRCP_Invoke_RenderingControl_SetVerticalKeystone(tempService, &UPnPResponseSink_RenderingControl_SetVerticalKeystone,NULL,250,25000);
	MRCP_Invoke_RenderingControl_GetBrightness(tempService, &UPnPResponseSink_RenderingControl_GetBrightness,NULL,250);
	MRCP_Invoke_RenderingControl_GetVolumeDB(tempService, &UPnPResponseSink_RenderingControl_GetVolumeDB,NULL,250,"Sample String");
	MRCP_Invoke_RenderingControl_GetGreenVideoBlackLevel(tempService, &UPnPResponseSink_RenderingControl_GetGreenVideoBlackLevel,NULL,250);
	MRCP_Invoke_RenderingControl_GetRedVideoGain(tempService, &UPnPResponseSink_RenderingControl_GetRedVideoGain,NULL,250);
	MRCP_Invoke_RenderingControl_SetMute(tempService, &UPnPResponseSink_RenderingControl_SetMute,NULL,250,"Sample String",1);
	MRCP_Invoke_RenderingControl_SetGreenVideoGain(tempService, &UPnPResponseSink_RenderingControl_SetGreenVideoGain,NULL,250,250);
	MRCP_Invoke_RenderingControl_SetSharpness(tempService, &UPnPResponseSink_RenderingControl_SetSharpness,NULL,250,250);
	MRCP_Invoke_RenderingControl_SetHorizontalKeystone(tempService, &UPnPResponseSink_RenderingControl_SetHorizontalKeystone,NULL,250,25000);
	MRCP_Invoke_RenderingControl_SetColorTemperature(tempService, &UPnPResponseSink_RenderingControl_SetColorTemperature,NULL,250,250);
	
	MRCP_Invoke_AVTransport_GetCurrentTransportActions(tempService, &UPnPResponseSink_AVTransport_GetCurrentTransportActions,NULL,250);
	MRCP_Invoke_AVTransport_Play(tempService, &UPnPResponseSink_AVTransport_Play,NULL,250,"Sample String");
	MRCP_Invoke_AVTransport_Previous(tempService, &UPnPResponseSink_AVTransport_Previous,NULL,250);
	MRCP_Invoke_AVTransport_Next(tempService, &UPnPResponseSink_AVTransport_Next,NULL,250);
	MRCP_Invoke_AVTransport_Stop(tempService, &UPnPResponseSink_AVTransport_Stop,NULL,250);
	MRCP_Invoke_AVTransport_GetTransportSettings(tempService, &UPnPResponseSink_AVTransport_GetTransportSettings,NULL,250);
	MRCP_Invoke_AVTransport_Seek(tempService, &UPnPResponseSink_AVTransport_Seek,NULL,250,"Sample String","Sample String");
	MRCP_Invoke_AVTransport_Pause(tempService, &UPnPResponseSink_AVTransport_Pause,NULL,250);
	MRCP_Invoke_AVTransport_GetPositionInfo(tempService, &UPnPResponseSink_AVTransport_GetPositionInfo,NULL,250);
	MRCP_Invoke_AVTransport_GetTransportInfo(tempService, &UPnPResponseSink_AVTransport_GetTransportInfo,NULL,250);
	MRCP_Invoke_AVTransport_SetAVTransportURI(tempService, &UPnPResponseSink_AVTransport_SetAVTransportURI,NULL,250,"Sample String","Sample String");
	MRCP_Invoke_AVTransport_GetDeviceCapabilities(tempService, &UPnPResponseSink_AVTransport_GetDeviceCapabilities,NULL,250);
	MRCP_Invoke_AVTransport_SetPlayMode(tempService, &UPnPResponseSink_AVTransport_SetPlayMode,NULL,250,"Sample String");
	MRCP_Invoke_AVTransport_GetMediaInfo(tempService, &UPnPResponseSink_AVTransport_GetMediaInfo,NULL,250);
	
	MRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(tempService, &UPnPResponseSink_ConnectionManager_GetCurrentConnectionInfo,NULL,25000);
	MRCP_Invoke_ConnectionManager_PrepareForConnection(tempService, &UPnPResponseSink_ConnectionManager_PrepareForConnection,NULL,"Sample String","Sample String",25000,"Sample String");
	MRCP_Invoke_ConnectionManager_ConnectionComplete(tempService, &UPnPResponseSink_ConnectionManager_ConnectionComplete,NULL,25000);
	MRCP_Invoke_ConnectionManager_GetProtocolInfo(tempService, &UPnPResponseSink_ConnectionManager_GetProtocolInfo,NULL);
	MRCP_Invoke_ConnectionManager_GetCurrentConnectionIDs(tempService, &UPnPResponseSink_ConnectionManager_GetCurrentConnectionIDs,NULL);
	*/
}

/* Called whenever a discovered device was removed from the network */
void UPnPDeviceRemoveSink(struct UPnPDevice *device)
{
	struct AVRenderer *r = (struct AVRenderer*)device->Tag;
	struct AVRendererConnection *c,*c2;
	int i;

	printf("**<< UPnP A/V Renderer Removed: %s >>**\r\n", device->FriendlyName);

	ILibDestroyHashTree(r->FetchingTable);

	if(r->Reserved!=0 && RendererRemoved!=NULL)
	{
		RendererRemoved(UPnP_CP,r);
	}

	for(i=0;i<r->PlayMediaLength;++i)
	{
		free(r->PlayMedia[i]);
	}
	free(r->PlayMedia);
	for(i=0;i<r->RecordMediaLength;++i)
	{
		free(r->RecordMedia[i]);
	}
	free(r->RecordMedia);


	for(i=0;i<r->ValidTransportStatusLength;++i)
	{
		free(r->ValidTransportStatus[i]);
	}
	free(r->ValidTransportStatus);


	for(i=0;i<r->ProtocolInfoLength;++i)
	{
		free(r->ProtocolInfo[i]);
	}
	if(r->ProtocolInfo!=NULL)
	{
		free(r->ProtocolInfo);
	}
	c = r->Connection;
	while(c!=NULL)
	{
		c2 = c->Next;
		if(c->ProtocolInfo!=NULL) {free(c->ProtocolInfo);}
		if(c->MediaUri!=NULL) {free(c->MediaUri);}
		if(c->TrackUri!=NULL) {free(c->TrackUri);}
		if(c->Channel!=NULL) {free(c->Channel);}
		if(c->Volume!=NULL) {free(c->Volume);}
		if(c->Mute!=NULL) {free(c->Mute);}
		if(c->PlayMedia!=NULL) {free(c->PlayMedia);}
		if(c->RecordMedia!=NULL) {free(c->RecordMedia);}
        if(c->TrackObject != NULL)
        {
            CDS_ObjRef_Release(c->TrackObject);
        }
		free(c);
		c = c2;
	}
	free(r);
}

void LockRendererCP(void *token)
{
	sem_wait(&(((struct AVR_Object*)token)->GlobalLock));
}
void UnLockRendererCP(void *token)
{
	sem_post(&(((struct AVR_Object*)token)->GlobalLock));
}

void DestroyRendererCP(void *object)
{
	struct AVR_Object *avrobject = (struct AVR_Object*)object;
	sem_destroy(&(avrobject->GlobalLock));
}
void RCP_IPAddressChanged(void *RCP)
{
	DMRCP__CP_IPAddressListChanged(((struct AVR_Object *)RCP)->CP);
}
void *CreateRendererCP(void *Chain)
{
	/* Event callback function registration code */
	DMRCP_EventCallback_RenderingControl_LastChange=&UPnPEventSink_RenderingControl_LastChange;
	DMRCP_EventCallback_ConnectionManager_SourceProtocolInfo=&UPnPEventSink_ConnectionManager_SourceProtocolInfo;
	DMRCP_EventCallback_ConnectionManager_SinkProtocolInfo=&UPnPEventSink_ConnectionManager_SinkProtocolInfo;
	DMRCP_EventCallback_ConnectionManager_CurrentConnectionIDs=&UPnPEventSink_ConnectionManager_CurrentConnectionIDs;
	DMRCP_EventCallback_AVTransport_LastChange=&UPnPEventSink_AVTransport_LastChange;

	UPnP_CP = (struct AVR_Object*)malloc(sizeof(struct AVR_Object));
	memset(UPnP_CP, 0, sizeof(struct AVR_Object));
	UPnP_CP->CP = DMRCP_CreateControlPoint(Chain,&UPnPDeviceDiscoverSink,&UPnPDeviceRemoveSink);
	sem_init(&(UPnP_CP->GlobalLock),0,1);
	UPnP_CP->PreSelect = NULL;
	UPnP_CP->PostSelect = NULL;
	UPnP_CP->Destroy = &DestroyRendererCP;
	UPnP_CP->Tag = NULL; /* Added by PDA */

	ILibAddToChain(Chain,UPnP_CP);

	UPnP_CP->Chain = Chain;
	return(UPnP_CP);
}
int RCP_CompatibleWithProtocolInfo(struct AVRendererConnection *avc, char *ProtocolInfo)
{
	struct DLNAProtocolInfo *target = DLNAProtocolInfo_Parse(ProtocolInfo,(int)strlen(ProtocolInfo));
	
	struct DLNAProtocolInfo *source;
	int i;
	int RetVal = 0;
	
	if(avc->CMID==0)
	{
		//
		// Compare with Parent
		//
		for(i=0;i<avc->Parent->ProtocolInfoLength && RetVal==0;++i)
		{
			source=DLNAProtocolInfo_Parse(avc->Parent->ProtocolInfo[i],(int)strlen(avc->Parent->ProtocolInfo[i]));
			RetVal = DLNAProtocolInfo_IsMatch(source,target);
			DLNAProtocolInfo_Destruct(source);
		}
	}
	else
	{
		//
		// Compare with Connection
		//
		source = DLNAProtocolInfo_Parse(avc->ProtocolInfo,(int)strlen(avc->ProtocolInfo));
		RetVal = DLNAProtocolInfo_IsMatch(source,target);
		DLNAProtocolInfo_Destruct(source);
	}
	DLNAProtocolInfo_Destruct(target);
	return(RetVal);
}
int RCP_SupportPlayMode(struct AVRenderer *r, enum PlayModeEnum PlayMode)
{
	return(PlayMode & r->SupportedPlayModeMask);
}
int RCP_SupportVolume(struct AVRenderer *r)
{
	struct UPnPStateVariable *SV;

	SV = r->RCS->Variables;
	while(SV!=NULL)
	{
		if(strcmp(SV->Name,"Volume")==0)
		{
			return(1);
		}
		SV = SV->Next;
	}
	return(0);
}
int RCP_SupportMute(struct AVRenderer *r)
{
	struct UPnPStateVariable *SV;

	SV = r->RCS->Variables;
	while(SV!=NULL)
	{
		if(strcmp(SV->Name,"Mute")==0)
		{
			return(1);
		}
		SV = SV->Next;
	}
	return(0);
}

void RCP_SetUri(struct AVRendererConnection *connection,char *URI,void *Tag,void (*SetUriSink)(struct AVRendererConnection*,int ErrorCode,void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->connection = connection;
	s->CB = SetUriSink;
	s->Tag = Tag;
	DMRCP_Invoke_AVTransport_SetAVTransportURI(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID,URI,"");
}

void RCP_SetPlayMode(struct AVRendererConnection *connection, enum PlayModeEnum PlayMode, void *Tag, void(*SetPlayModeSink)(struct AVRendererConnection *sender, int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	int index = 0;

	s->connection = connection;
	s->CB = SetPlayModeSink;
	s->Tag = Tag;

	while(PlayMode!=0)
	{
		PlayMode = PlayMode >> 1;
		++index;
	}

	DMRCP_Invoke_AVTransport_SetPlayMode(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID,RCP_PlayModeTable[index-1]);
}

void RCP_Play(struct AVRendererConnection *connection,void *Tag,void (*PlaySink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->connection = connection;
	s->CB = PlaySink;
	s->Tag = Tag;
	DMRCP_Invoke_AVTransport_Play(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID,"1");
}

void RCP_Seek(struct AVRendererConnection *connection,
			  void *Tag,
			  void (*SeekSink)(struct AVRendererConnection*,int ErrorCode, void *Tag),
			  char* unescaped_Unit, 
			  char* unescaped_Target)
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->connection = connection;
	s->CB = SeekSink;
	s->Tag = Tag;
	DMRCP_Invoke_AVTransport_Seek(connection->Parent->AVT,
		&UPnPResponseSink_Generic, s, connection->AVTID, unescaped_Unit, unescaped_Target);
}


void RCP_GetPosition(struct AVRendererConnection *connection,
					 void *Tag,
					 void (*GetPositionSink)(struct AVRendererConnection*,
					 int ErrorCode,
					 int RelativeSeconds,
					 int AbsoluteSeconds,
					 int RelativeCounter,
					 int AbsoluteCounter,
					 void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->connection = connection;
	s->CB = GetPositionSink;
	s->Tag = Tag;

	DMRCP_Invoke_AVTransport_GetPositionInfo(connection->Parent->AVT,
		&UPnPUser_GetPositionInfo, s, connection->AVTID);
}
void RCP_SetVolume(struct AVRendererConnection *connection, char *Channel, char NewVolumeValue, void *Tag, void(*SetVolumeSink)(struct AVRendererConnection *sender,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	unsigned short MaxVol = RCS_GetMaxVolume(connection->Parent);
	unsigned short MinVol = RCS_GetMinVolume(connection->Parent);
	double pcnt;
	unsigned short DesiredVolume;

	memset(s, 0, sizeof(struct AVR_State));
	s->connection = connection;
	s->Tag = Tag;
	s->CB = SetVolumeSink;

	pcnt = (double)NewVolumeValue / (double)100;
	DesiredVolume = (unsigned short)(pcnt*(double)(MaxVol-MinVol));
	
	DMRCP_Invoke_RenderingControl_SetVolume(connection->Parent->RCS, &UPnPResponseSink_Generic,s,connection->RCID,Channel,DesiredVolume);
}
void RCP_SetMute(struct AVRendererConnection *connection, char *Channel, int MuteState, void *Tag, void(*SetMuteSink)(struct AVRendererConnection *sender,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->CB=SetMuteSink;
	s->connection = connection;
	s->Tag = Tag;

	DMRCP_Invoke_RenderingControl_SetMute(connection->Parent->RCS, &UPnPResponseSink_Generic,s,connection->RCID,Channel,MuteState);	
}
void RCP_Stop(struct AVRendererConnection *connection,void *Tag, void (*StopSink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->CB=StopSink;
	s->connection = connection;
	s->Tag = Tag;

	DMRCP_Invoke_AVTransport_Stop(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID);
}
void RCP_Pause(struct AVRendererConnection *connection,void *Tag, void (*PauseSink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->CB=PauseSink;
	s->connection = connection;
	s->Tag = Tag;

	DMRCP_Invoke_AVTransport_Pause(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID);
}
void RCP_Next(struct AVRendererConnection *connection,void *Tag, void (*NextSink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->CB=NextSink;
	s->connection = connection;
	s->Tag = Tag;

	DMRCP_Invoke_AVTransport_Next(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID);
}
void RCP_Prev(struct AVRendererConnection *connection,void *Tag, void (*PrevSink)(struct AVRendererConnection*,int ErrorCode, void *Tag))
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));
	s->CB=PrevSink;
	s->connection = connection;
	s->Tag = Tag;

	DMRCP_Invoke_AVTransport_Previous(connection->Parent->AVT, &UPnPResponseSink_Generic,s,connection->AVTID);
}
void RCP_CloseConnection(struct AVRendererConnection *connection)
{
	DMRCP_Invoke_ConnectionManager_ConnectionComplete(connection->Parent->CMS,NULL,NULL,connection->CMID);
}
void RCP_CreateConnection(struct AVRenderer *renderer, char* RemoteProtocolInfo, void *Tag)
{
	struct AVR_State *s = (struct AVR_State*)malloc(sizeof(struct AVR_State));
	memset(s, 0, sizeof(struct AVR_State));

	LockRendererCP(renderer->CP);
	++renderer->PendingPrepareForConnection;
	UnLockRendererCP(renderer->CP);

	s->CB = renderer;
	s->connection = NULL;
	s->Tag = Tag;

	DMRCP_Invoke_ConnectionManager_PrepareForConnection(renderer->CMS, &UPnPResponseSink_ConnectionManager_PrepareForConnection,s,RemoteProtocolInfo,"/",-1,"Input");
}
void RCP_AddRef(struct AVRenderer *r)
{
	DMRCP_AddRef(r->device);
}
void RCP_Release(struct AVRenderer *r)
{
	DMRCP_Release(r->device);
}
void UpdateConnectionInfo(struct AVRendererConnection *connection)
{
	DMRCP_Invoke_ConnectionManager_GetCurrentConnectionInfo(connection->Parent->CMS, &ProcessUpdateConnectionInfo_GetCurrentConnectionInfo,connection,connection->CMID);

}

void* RCPToken_GetTag(void* token)
{
	if(token != NULL)
	{
		return ((struct AVR_Object*)token)->Tag;
	}
	else
	{
		return NULL;
	}
}

void RCPToken_SetTag(void* token, void* tag)
{
	if(token != NULL)
	{
		((struct AVR_Object*)token)->Tag = tag;
	}
}

char* RCP_GetDLNADOC(struct AVRenderer* renderer)
{
	return DMRCP_GetCustomXML_X_DLNADOC(renderer->device);
}

char* RCP_GetDLNACAP(struct AVRenderer* renderer)
{
	return DMRCP_GetCustomXML_X_DLNACAP(renderer->device);
}
