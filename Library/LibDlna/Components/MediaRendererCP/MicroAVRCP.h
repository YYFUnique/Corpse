
#ifndef __AVRenderer__
#define __AVRenderer__

#include "../CdsObjects/CdsObject.h"

#define RCP_GETVOLUME_FAILED 1
#define RCP_GETTRANSPORTSETTINGS_FAILED 2
#define RCP_PREPAREFORCONNECTION_FAILED 3
#define RCP_GETMEDIAINFO_FAILED 4
#define RCP_GETTRANSPORTINFO_FAILED 5
#define RCP_GETPOSITIONINFO_FAILED 6
#define RCP_GETMUTESTATE_FAILED 7
#define RCP_GETCONNECTIONINFO_FAILED 8

#define RCP_GETDEVICECAPABILITIES_FAILED 8

enum PlayStateEnum
{
	AVRCP_PLAYING=0,
	AVRCP_STOPPED,
	AVRCP_PAUSED,
	AVRCP_RECORDING,
	AVRCP_TRANSITIONING,
	AVRCP_NO_MEDIA,
	AVRCP_UNKNOWN
};
enum PlayModeEnum
{
	AVRCP_INVALID		=0x0000000,
	AVRCP_NORMAL		=0x0000001,
	AVRCP_REPEAT_ALL	=0x0000002,
	AVRCP_REPEAT_ONE	=0x0000004,
	AVRCP_RANDOM		=0x0000008,
	AVRCP_SHUFFLE       =0x0000010,
	AVRCP_INTRO			=0x0000020
};

struct AVRenderer
{
	struct UPnPDevice *device;
	struct UPnPService *AVT;
	struct UPnPService *CMS;
	struct UPnPService *RCS;
	void *Tag;
	char *FriendlyName;
	void *CP;
	int Reserved;

	int PendingPrepareForConnection;
	char *PendingConnectionEvent;
	void *EventTimer;
	void *FetchingTable;

	char **ProtocolInfo;
	int ProtocolInfoLength;

	
	char **PlayMedia;
	int PlayMediaLength;
	char **RecordMedia;
	int RecordMediaLength;

	char **ValidTransportStatus;
	int ValidTransportStatusLength;

	int ImplementsPrepareForConnection;
	struct AVRendererConnection *Connection;

	int SupportedPlayModeMask;
	
	void (*ConnectionAdded)(struct AVRenderer *sender,struct AVRendererConnection *Connection, int ErrorCode, void *Tag);
	void (*ConnectionRemoved)(struct AVRenderer *sender,struct AVRendererConnection *Connection);
	void (*StateChanged)(struct AVRenderer *sender,struct AVRendererConnection *Connection);
};

struct AVRendererConnection
{
	struct AVRenderer *Parent;

	int CMID;
	int AVTID;
	int RCID;

	struct AVRendererConnection *Next;
	struct AVRendererConnection *Prev;
	void *Reserved;
	void *Reserved2;
	void *Tag;
	int Error;

	char *ProtocolInfo;
	char *MediaUri;
	char *TrackUri;
	char *TransportStatus;
	unsigned int TotalTracks;
	unsigned int TrackNumber;
	enum PlayStateEnum PlayState;
	enum PlayModeEnum PlayMode;

	unsigned int Duration;

    char TrackPosition[32];
    struct CdsObject* TrackObject;

	char **SupportedRecordQualityModes;
	int SupportedRecordQualityModesLength;
	char **RecordMedia;
	int RecordMediaLength;
	char **PlayMedia;
	int PlayMediaLength;


	int ChannelCount;
	char **Channel;
	char *Volume;
	int *Mute;
};

#endif

void LockRendererCP(void *token);
void UnLockRendererCP(void *token);

void *CreateRendererCP(void *Chain);
extern void (*RendererAdded)(void *token, struct AVRenderer* r);
extern void (*RendererRemoved)(void *token, struct AVRenderer *r);
extern char* RCP_PlayStateTable[];
extern char* RCP_PlayModeTable[];

/* >>> Added by PDA */
void* RCPToken_GetTag(void* token);
void RCPToken_SetTag(void* token, void* tag);
char* RCP_GetDLNADOC(void* token);
char* RCP_GetDLNACAP(void* token);
/* <<< */

int RCP_SupportPlayMode(struct AVRenderer *r, enum PlayModeEnum PlayMode);
int RCP_SupportVolume(struct AVRenderer *r);
int RCP_SupportMute(struct AVRenderer *r);

void RCP_CreateConnection(struct AVRenderer *renderer, char* RemoteProtocolInfo, void *Tag);
void RCP_CloseConnection(struct AVRendererConnection *connection);
void RCP_Play(struct AVRendererConnection *connection,void *Tag,void (*PlaySink)(struct AVRendererConnection*,int ErrorCode, void *Tag));
void RCP_Seek(struct AVRendererConnection *connection,void *Tag,
			  void (*SeekSink)(struct AVRendererConnection*,int ErrorCode, void *Tag),
			  char* unescaped_Unit, 
			  char* unescaped_Target);
void RCP_Stop(struct AVRendererConnection *connection,void *Tag, void (*StopSink)(struct AVRendererConnection*,int ErrorCode, void *Tag));
void RCP_Pause(struct AVRendererConnection *connection,void *Tag, void (*PauseSink)(struct AVRendererConnection*,int ErrorCode, void *Tag));
void RCP_Next(struct AVRendererConnection *connection,void *Tag, void (*NextSink)(struct AVRendererConnection*,int ErrorCode, void *Tag));
void RCP_Prev(struct AVRendererConnection *connection,void *Tag, void (*PrevSink)(struct AVRendererConnection*,int ErrorCode, void *Tag));
void RCP_SetUri(struct AVRendererConnection *connection,char *URI,void *Tag,void (*SetUriSink)(struct AVRendererConnection*,int ErrorCode,void *Tag));
int RCP_CompatibleWithProtocolInfo(struct AVRendererConnection *avc, char *ProtocolInfo);

void RCP_ReAcquireConnectionState(struct AVRendererConnection *connection, void *Tag, void (*ReAcquireConnectionStateSink)(struct AVRendererConnection*,int ErrorCode, void *Tag));

void RCP_SetVolume(struct AVRendererConnection *connection, char *Channel, char NewVolumeValue, void *Tag, void(*SetVolumeSink)(struct AVRendererConnection *sender,int ErrorCode, void *Tag));
void RCP_SetMute(struct AVRendererConnection *connection, char *Channel, int MuteState, void *Tag, void(*SetMuteSink)(struct AVRendererConnection *sender,int ErrorCode, void *Tag));
void RCP_SetPlayMode(struct AVRendererConnection *connection, enum PlayModeEnum PlayMode, void *Tag, void(*SetPlayModeSink)(struct AVRendererConnection *sender, int ErrorCode, void *Tag));
void RCP_GetPosition(struct AVRendererConnection *connection,
					 void *Tag,
					 void (*GetPositionSink)(
						struct AVRendererConnection*,
						int ErrorCode,
						int RelativeSeconds,
						int AbsoluteSeconds,
						int RelativeCounter,
						int AbsoluteCounter,
						void *Tag));

void RCP_AddRef(struct AVRenderer *r);
void RCP_Release(struct AVRenderer *r);
void RCP_IPAddressChanged(void *RCP);
