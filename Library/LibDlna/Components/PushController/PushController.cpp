/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2002 - 2006 Intel Corporation.  All rights reserved.
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
 * $File: $
 * $Revision: 1.5 $
 * $Author: byroe $
 * $Date: 2007/03/16 21:17:13 $
 *
 */

#include "../../DMC.h"

#include "PushController.h"
#include "../HttpFiles/DlnaHttpServer.h"

extern "C"
{
#include "../GeneratedStacks/ILib/ILibParsers.h"
#include "../FileAbstractionFiles/FileIoAbstraction.h"
#include "../GeneratedStacks/ControlPointStacks/MediaRenderer/UPnPControlPointStructs.h"
};

#define MAXCONNECTIONS  1

/**********************************************************************/
struct _PC_Token
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);

    void*                       Chain;
    ILibThreadPool              ThreadPool;
    void*                       Tag;

    ILibWebServer_ServerToken   Server;

    void*                       SessionsList;
    sem_t                       SessionListSync;
};

struct _PC_Session
{
    int                                     _refCount;
    void*                                   _reserved_1;

    struct _PC_Token*                       Token;

    char*                                   FileName;
    unsigned long                           FileLength;
    char*                                   URI;
    char*                                   MimeType;
    char*                                   ProtocolInfo;

    struct ILibWebServer_Session*           Session;
    DH_TransferStatus                       Status;

    void*                                   Tag;

    PCS_TransferStatus                      UpdateStatusCallback;
};
/**********************************************************************/


/**********************************************************************/
void _destroyFromChain(void* object)
{
    struct _PC_Token* instance = (struct _PC_Token*)object;
    void* localList = ILibLinkedList_Create();
    void* node = ILibLinkedList_GetNode_Head(instance->SessionsList);
    while(node != NULL)
    {
        PC_Session session = NULL;
        sem_wait(&instance->SessionListSync);
        session = (PC_Session)ILibLinkedList_GetDataFromNode(node);
        sem_post(&instance->SessionListSync);
        ILibLinkedList_AddTail(localList, session);

        node = ILibLinkedList_GetNextNode(node);
    }
    node = ILibLinkedList_GetNode_Head(localList);
    while(node != NULL)
    {
        PC_Session session = (PC_Session)ILibLinkedList_GetDataFromNode(node);
        PC_Session_Release(session);
        node = ILibLinkedList_GetNextNode(node);
    }
    ILibLinkedList_Destroy(localList);
    sem_destroy(&instance->SessionListSync);
    instance->SessionListSync = NULL;
    ILibLinkedList_Destroy(instance->SessionsList);
    instance->SessionsList = NULL;
}

void _onSession(struct ILibWebServer_Session* session, void* tag);
PC_Token PushController_Create(void* chain, ILibThreadPool threadPool, void* tag)
{
    struct _PC_Token* instance = (struct _PC_Token*)malloc(sizeof(struct _PC_Token));
    if(chain == NULL || instance == NULL || threadPool == NULL || ILibThreadPool_GetThreadCount(threadPool) < MAXCONNECTIONS)
    {
        return NULL;
    }
    memset(instance, 0, sizeof(struct _PC_Token));
    instance->Chain = chain;
    instance->ThreadPool = threadPool;
    instance->Tag = tag;
    sem_init(&instance->SessionListSync, 0, 1);
    instance->SessionsList = ILibLinkedList_Create();
    instance->Destroy = &_destroyFromChain;

    instance->Server = ILibWebServer_Create(chain, MAXCONNECTIONS, 0, &_onSession, instance);

    ILibAddToChain(chain, instance);


    return (PC_Token)instance;
}
/**********************************************************************/


/**********************************************************************/
void _onDisconnect(struct ILibWebServer_Session* sender);
void _onReceive(struct ILibWebServer_Session *sender, int InterruptFlag, struct packetheader *header, char *bodyBuffer, int *beginPointer, int endPointer, int done);
void _onSession(struct ILibWebServer_Session* session, void* tag)
{
    session->OnDisconnect = &_onDisconnect;
    session->OnReceive = &_onReceive;
    session->User = tag;
}

void _fireUpdateStatusCallback(struct _PC_Session* pcSession, int start, int finish, long transfered, long total)
{
    if(pcSession->UpdateStatusCallback != NULL)
    {
        pcSession->UpdateStatusCallback(pcSession, start, finish, transfered, total, pcSession->Tag);
    }
}

struct _PC_Session* _findSessionByServerSessionPtr(struct _PC_Token* instance, struct ILibWebServer_Session* session);
void _onDisconnect(struct ILibWebServer_Session* sender)
{
    struct _PC_Token* instance = (struct _PC_Token*)sender->User;
    struct _PC_Session* pcSession = _findSessionByServerSessionPtr(instance, sender);
    if(pcSession != NULL)
    {
        pcSession->Status = NULL;
        pcSession->Session = NULL;
    }
}

struct _PC_Session* _findSessionByServerSessionPtr(struct _PC_Token* instance, struct ILibWebServer_Session* session)
{
    struct _PC_Session* result = NULL;
    void* node = ILibLinkedList_GetNode_Head(instance->SessionsList);
    sem_wait(&instance->SessionListSync);
    while(node != NULL)
    {
        struct _PC_Session* current = (struct _PC_Session*)ILibLinkedList_GetDataFromNode(node);
        if(current->Session == session)
        {
            result = current;
            break;
        }
        node = ILibLinkedList_GetNextNode(node);
    }
    sem_post(&instance->SessionListSync);

    return result;
}

void _onResponseDone(struct ILibWebServer_Session *session, DH_TransferStatus transfer_status_handle, enum DHS_Errors dhs_error_code, void *user_obj);
struct _PC_Session* _findSessionByUri(struct _PC_Token* instance, char* uri);
char* _createUriFromPacketHeader(struct packetheader* header);
void _onReceive(struct ILibWebServer_Session *sender, int InterruptFlag, struct packetheader *header, char *bodyBuffer, int *beginPointer, int endPointer, int done)
{
    struct _PC_Token* instance = (struct _PC_Token*)sender->User;
    char* uri = _createUriFromPacketHeader(header);
    char* directive = ILibString_ToUpper(header->Directive, header->DirectiveLength);
    struct _PC_Session* pcSession = NULL;

    if(uri != NULL)
    {
        pcSession = _findSessionByUri(instance, uri);
        free(uri);
    }
    if(pcSession != NULL && (strcmp(directive, "GET") == 0 || strcmp(directive, "HEAD") == 0))
    {
        pcSession->Session = sender;
        if(strcmp(directive, "GET") == 0)
        {
            _fireUpdateStatusCallback(pcSession, 1, 0, 0L, pcSession->FileLength);
        }
        pcSession->Status = DHS_RespondWithLocalFile(sender, instance->ThreadPool, header, (size_t)2048, pcSession->FileName, DH_TransferMode_Streaming, pcSession->MimeType, pcSession->ProtocolInfo, NULL, pcSession, &_onResponseDone);
    }
    else
    {
        struct packetheader* response = ILibCreateEmptyPacket();
        if(pcSession == NULL)
        {
            response->StatusCode = 500;
            response->StatusData = "Internal Server Error";
            response->StatusDataLength = 21;
        }
        else
        {
            response->StatusCode = 405;
            response->StatusData = "Method Not Allowed";
            response->StatusDataLength = 18;
            ILibAddHeaderLine(response, "Allow", 5, "GET, HEAD", 9);
        }
        response->Version = "1.1";
        response->VersionLength = 3;
        ILibAddHeaderLine(response, "Server", 6, "PushServer/1.0", 14);

        ILibWebServer_StreamHeader(sender, response);
    }
    free(directive);
}

void _onResponseDone(struct ILibWebServer_Session *session, DH_TransferStatus transfer_status_handle, enum DHS_Errors dhs_error_code, void *user_obj)
{
    struct _PC_Session* pcSession = (struct _PC_Session*)user_obj;
    struct _PC_Token* instance = pcSession->Token;
    long sendTotal = 0;
    long sendExpected = 0;

    pcSession->Status = NULL;

    DH_QueryTransferStatus(transfer_status_handle, &sendTotal, &sendExpected, NULL, NULL);
    _fireUpdateStatusCallback(pcSession, 0, 1, sendTotal, sendExpected);
}

struct _PC_Session* _findSessionByUri(struct _PC_Token* instance, char* uri)
{
    struct _PC_Session* result = NULL;
    void* node = ILibLinkedList_GetNode_Head(instance->SessionsList);
    sem_wait(&instance->SessionListSync);
    while(node != NULL)
    {
        struct _PC_Session* current = (struct _PC_Session*)ILibLinkedList_GetDataFromNode(node);
        if(strcmp(uri, current->URI) == 0)
        {
            result = current;
            break;
        }
        node = ILibLinkedList_GetNextNode(node);
    }
    sem_post(&instance->SessionListSync);

    return result;
}

char* _createUriFromPacketHeader(struct packetheader* header)
{
    char* result = NULL;
    char tmp[1024];
    char* host = ILibGetHeaderLine(header, "Host", 4);
    char* address = NULL;
    char* object = NULL;
    //object = ILibString_Copy(header->DirectiveObj, header->DirectiveObjLength);

    if(host != NULL)
    {
        int port = 80;
        char* aport = strchr(host, ':');
        object = ILibString_Copy(header->DirectiveObj, header->DirectiveObjLength);
        if(aport != NULL)
        {
            port = atoi(aport + 1);
            *aport = 0;
        }
        sprintf(tmp, "http://%s:%d%s", host, port, object);
        result = (char*)malloc(strlen(tmp) + 1);
        strcpy(result, tmp);
        free(object);
    }

    return result;
}
/**********************************************************************/


/**********************************************************************/
void* PushController_GetTag(PC_Token token)
{
    struct _PC_Token* instance = (struct _PC_Token*)token;
    if(token == NULL)
    {
        return NULL;
    }
    return instance->Tag;
}

void PushController_SetTag(PC_Token token, void* tag)
{
    struct _PC_Token* instance = (struct _PC_Token*)token;
    if(token == NULL)
    {
        return;
    }
    instance->Tag = tag;
}

PC_Token PC_Session_GetPushControllerToken(PC_Session session)
{
    if(session != NULL)
    {
        struct _PC_Session* pcSession = (struct _PC_Session*)session;
        return pcSession->Token;
    }
    else
    {
        return NULL;
    }
}
/**********************************************************************/


/**********************************************************************/
void PC_Session_AddRef(PC_Session session)
{
    if(session != NULL)
    {
        struct _PC_Session* pcSession = (struct _PC_Session*)session;
        pcSession->_refCount++;
    }
}

void PC_Session_Release(PC_Session session)
{
    if(session != NULL)
    {
        struct _PC_Session* pcSession = (struct _PC_Session*)session;
        struct _PC_Token* instance = (struct _PC_Token*)pcSession->Token;
        pcSession->_refCount--;
        if(pcSession->_refCount < 0)
        {
            if(pcSession->Status != NULL)
            {
                long bytes = 0;
                long total = 0;
                DH_QueryTransferStatus(pcSession->Status, &bytes, &total, NULL, NULL);
                DH_AbortTransfer(pcSession->Status);
            }

            free(pcSession->FileName);
            free(pcSession->MimeType);
            free(pcSession->ProtocolInfo);
            free(pcSession->URI);

            if(instance->SessionListSync != NULL)
            {
                sem_wait(&instance->SessionListSync);
                if(instance->SessionsList != NULL)
                {
                    ILibLinkedList_Remove_ByData(instance->SessionsList, pcSession);
                }
                sem_post(&instance->SessionListSync);
            }

            free(pcSession);
        }
    }
}
/**********************************************************************/


/**********************************************************************/
void PC_Session_UpdateTransferStatus(PC_Session session)
{
    if(session != NULL)
    {
        struct _PC_Session* pcSession = (struct _PC_Session*)session;
        if(pcSession->Status != NULL)
        {
            long bytes = 0;
            long total = 0;

            DH_QueryTransferStatus(pcSession->Status, &bytes, &total, NULL, NULL);
            _fireUpdateStatusCallback(pcSession, 0, 0, bytes, total);
        }
    }
}
/**********************************************************************/


/**********************************************************************/
void _onSetUriCallback(struct AVRendererConnection* rendererConnection, int ErrorCode, void* tag);
char* _buildUri(struct AVRendererConnection* rendererConnection, PC_Token token, const char* filename);
PC_Session PushController_PushFile(PC_Token token,
                                               struct AVRendererConnection* rendererConnection,
                                               const char* filename, const char* mimeType, const char* protocolInfo,
                                               PC_PushFileResponse callback, 
                                               PCS_TransferStatus updateTransferCallback,
                                               void* sessionTag)
{
    if(token == NULL || rendererConnection == NULL || filename == NULL)
    {
        return NULL;
    }
    else
    {
        struct _PC_Token* instance = (struct _PC_Token*)token;
        char* uri = _buildUri(rendererConnection, instance->Server, filename);
        struct _PC_Session* found = _findSessionByUri(instance, uri);

        if(found == NULL && uri != NULL)
        {
            struct _PC_Session* pcSession = (struct _PC_Session*)malloc(sizeof(struct _PC_Session));
            memset(pcSession, 0, sizeof(struct _PC_Session));
            pcSession->FileName = ILibString_Copy(filename, -1);
            pcSession->MimeType = ILibString_Copy(mimeType, -1);
            pcSession->ProtocolInfo = ILibString_Copy(protocolInfo, -1);
            pcSession->Token = instance;
            pcSession->Tag = sessionTag;
            pcSession->UpdateStatusCallback = updateTransferCallback;
            //pcSession->FileLength = ILibFileDir_GetFileSize(filename);
            pcSession->URI = uri;
			pcSession->FileLength = 0;
			if (g_FileIO)
			{
				void* f = g_FileIO->Open(filename);
				if (f)
				{
					unsigned long long file_size = 0;
					g_FileIO->Seek(f, 0, SEEK_END);
					g_FileIO->Tell(f, &file_size);
                    pcSession->FileLength = file_size;
				}
			}

            sem_wait(&instance->SessionListSync);
            ILibLinkedList_AddTail(instance->SessionsList, pcSession);
            sem_post(&instance->SessionListSync);

            pcSession->_reserved_1 = callback;
            RCP_SetUri(rendererConnection, uri, pcSession, &_onSetUriCallback);

            return pcSession;
        }
        else if(found == NULL && uri == NULL)
        {
            return NULL;
        }
        else
        {
            if(uri != NULL)
            {
                free(uri);
            }
            return found;
        }
    }
}

void _onSetUriCallback(struct AVRendererConnection* rendererConnection, int ErrorCode, void* tag)
{
    struct _PC_Session* pcSession = (struct _PC_Session*)tag;
    PC_PushFileResponse callback = (PC_PushFileResponse)pcSession->_reserved_1;

    callback((PC_Session)pcSession, ErrorCode, pcSession->Tag);
}

char* _buildUri(struct AVRendererConnection* rendererConnection, ILibWebServer_ServerToken server, const char* filename)
{
	int i;
    char tmp[1024];
    char* result = NULL;
	int length = (int)strlen(filename);
	int elength = ILibHTTPEscapeLength(filename);
    char* escaped = (char*)malloc((size_t)elength);
	char* fn = (char*)malloc(length + 1);

	if (memcmp(filename, "http:", 5) == 0)
	{
		strcpy (tmp, filename);
	}
	else
	{
		strcpy(fn, filename);
		// BUG FIX: Replace unwanted backslash character to an underscore character...
		for(i = 0; i < length; i++)
		{
			if(fn[i] == '\\')
			{
				fn[i] = '_';
			}
		}

		ILibHTTPEscape(escaped, fn);

		sprintf(tmp, "http://%s:%d/%s", 
			rendererConnection->Parent->device->InterfaceToHost, 
			ILibWebServer_GetPortNumber(server), 
			escaped);
	}

	free(escaped);
	free(fn);

    result = (char*)malloc((size_t)strlen(tmp) + 1);
    strcpy(result, tmp);

    return result;
}
/**********************************************************************/
