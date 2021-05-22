#pragma once

class DocModification;
class Document;

/**
 * A class that wants to receive notifications from a Document must be derived from DocWatcher
 * and implement the notification methods. It can then be added to the watcher list with AddWatcher.
 */
class IDocWatcher
{
public:
	virtual ~IDocWatcher() {}
public:
	virtual void NotifyModifyAttempt(Document *pDoc, void *pUserData) = 0;
	virtual void NotifySavePoint(Document *pDoc, void *pUserData, BOOL bSavePoint) = 0;
	virtual void NotifyModified(Document *pDoc, DocModification mh, void *pUserData) = 0;
	virtual void NotifyDeleted(Document *pDoc, void *pUserData) = 0;
	virtual void NotifyStyleNeeded(Document *pDoc, void *pUserData, int nEndPos) = 0;
	virtual void NotifyLexerChanged(Document *pDoc, void *pUserData) = 0;
	virtual void NotifyErrorOccurred(Document *pDoc, void *pUserData, int nStatus) = 0;
};