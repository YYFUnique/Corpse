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
 * $Revision: 1.3 $
 * $Author: pdamons $
 * $Date: 2006/11/17 22:11:39 $
 *
 */

#if !defined(__PUSHCONTROLLER_H__)
#define __PUSHCONTROLLER_H__
/**
    \file PushController.h
	\brief An optional DLNA capability for sending local media files to a DMR.

    This file represents the API for creating and managing a DLNA +PU+ (Push
	Controller).
    \{
*/


extern "C"
{
#include "../GeneratedStacks/ILib/ILibThreadPool.h"
#include "../MediaRendererCP/MicroAVRCP.h"
};

/** \brief The token that represents an instance of the Push Controller component. */
typedef void *PC_Token;

/** \brief The token that represents a specific session of the Push Controller
component. */
typedef void *PC_Session;

/** \brief This typedef specifies the signature of the callback function that is called
	to determine the success or failure of the \ref PushController_PushFile function
	that initiates a new session with the DMR.
	\param session The \ref PC_Session token that the call back is for (i.e. the sender
			of the callback).
	\param errorCode This is 0 (zero) on success otherwise it is a the UPnP error code returned
			from the action call to SetAVTransportURI.
	\param sessionTag This is the void* that was specified in the \ref PushController_PushFile as
			the sessionTag.
*/
typedef void (*PC_PushFileResponse)(PC_Session session, int errorCode, void* sessionTag);

/** \brief This typedef specifies the signature of the callback function that is called
	at the start and finish of the stream to the DMR.  To have this called at intervals
	during the streaming call \ref PC_Session_UpdateTransferStatus at regular intervals.
	\param session The \ref PC_Session token that the call back is for (i.e. the sender
			of the callback).
	\param startTransfer When the transfer starts this will be a 1 all other cases it will
			be a 0 (zero).
	\param finishTransfer This will be non-zero when the transfer is completed.  It will 1
			if the transfer completed, -1 if the transfer was aborted, or 0 if the transfer
			is starting or is continuing.
	\param bytesTransfered This is the number of bytes transfered so far.
	\param totalBytesToTransfer This is the total number of bytes that can be transfered.
	\param sessionTag This is the void* that was specified in the \ref PushController_PushFile
			as the sessionTag.

	The actual function is supplied by the application using this API.
*/
typedef void (*PCS_TransferStatus)(PC_Session session, int startTransfer,
                                   int finishTransfer, long bytesTransfered,
                                   long totalBytesToTransfer, void* sessionTag);

/** \brief This function creates an instance of a push controller.  This includes a HTTP
		   server for media transfer.
   \param chain The microstack chain to add the push controller instance to.
   \param threadPool A threadpool of at least one thread is required!
   \param tag This is a application defined value that can be returned by calling
		\ref PushController_GetTag or set after creation by \ref PushController_SetTag.
*/
PC_Token PushController_Create(void* chain, ILibThreadPool threadPool, void* tag);

/** \brief This function will retieve the tag value set in the \ref PushController_Create
		   function or by the \ref PushController_SetTag.
    \param token The instance token of the Push Controller.
	\returns The application specific tag.
*/
void* PushController_GetTag(PC_Token token);

/** \brief This function sets the application defined tag.  It overwrites the old value
		   and it is the responsibilty of the application to destroy any memory associated
		   with the old tag value.
    \param token The instance token of the Push Controller.
    \param tag The new application tag value. 
*/
void PushController_SetTag(PC_Token token, void* tag);

/** \brief This function creates a session with the DMR for a specific piece of local media.
    \param token The instance token of the Push Controller.
	\param rendererConnection This is the reference to the AVRendererConnection structure
		   from the MediaRendererCP component.
    \param filename This is the filename to the local file that will be sent to the DMR via
		   the SetAVTransportURI action call.
	\param mimeType This is used in the HTTP reponse headers when the DMR sends an HTTP GET
		   or HTTP HEAD request to the Push Controller.
    \param protocolInfo This is used in the HTTP reponse headers when the DMR sends an HTTP GET
		   or HTTP HEAD request to the Push Controller.
    \param callback This is the function callback to determine if the requested operation 
		   succeeded.
    \param updateTransferCallback This is the function callback that updates the application
		   while a transfer is occurring.  It will also signal the start and end of a transfer.
	\param sessionTag This is an application tag to be used in the callbacks for this session.
	\returns The \ref PC_Session token created (or NULL) for the given filename.
	
	This will return a previous session if the same filename is specified more than once.
*/
PC_Session PushController_PushFile(PC_Token token,
                                   struct AVRendererConnection* rendererConnection,
                                   const char* filename, const char* mimeType, const char* protocolInfo,
                                   PC_PushFileResponse callback,
                                   PCS_TransferStatus updateTransferCallback,
                                   void* sessionTag);

/** \brief This allows the application to retrieve the \ref PC_Token that created this
		   \ref PC_Session.
*/
PC_Token PC_Session_GetPushControllerToken(PC_Session session);

/** \brief This function should be called to get the status of a file transfer.

	It should only be called after a transfer is started and before the transfer is completed.
	A common technique would be to start a periodic timer when the transfer starts and kill the
	timer when the transfer ends.  As the timer event happens call this function to get called
	back with the bytes transfered via the \ref PCS_TransferStatus callback specified in the
	\ref PushController_PushFile.
*/
void PC_Session_UpdateTransferStatus(PC_Session session);

/** \brief If you need to keep your own copy of the PC_Session token call this so it will not be
		   deleted before you are done with it.  Make sure to call \ref PC_Session_Release when
		   your application is done with the token.
    \param session The session to increase the reference count on.
*/
void PC_Session_AddRef(PC_Session session);

/** \brief Call this to release your copy of the \ref PC_Session token.
    \param session The session to increase the reference count on.
*/
void PC_Session_Release(PC_Session session);

/* \} */
#endif /* __PUSHCONTROLLER_H__ */
