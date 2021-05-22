#pragma once

#include "IDocWatcher.h"

class WatcherWithUserData
{
public:
	WatcherWithUserData();
public:
	IDocWatcher* GetDocWatcher() const;
	void SetDocWatcher(IDocWatcher* pDocWatcher);
	void* GetUserData() const;
	void SetUserData(void* pUserData);
protected:
	IDocWatcher* m_pWatcher;
	void * m_pUserData;
};