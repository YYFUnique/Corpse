#include "StdAfx.h"
#include "WatcherWithUserData.h"

WatcherWithUserData::WatcherWithUserData()
: m_pWatcher(NULL)
, m_pUserData(NULL)
{

}

void WatcherWithUserData::SetDocWatcher(IDocWatcher* pDocWatcher)
{
	m_pWatcher = pDocWatcher;
}

IDocWatcher* WatcherWithUserData::GetDocWatcher() const
{
	return m_pWatcher;
}

void WatcherWithUserData::SetUserData(void* pUserData)
{
	m_pUserData = pUserData;
}

void* WatcherWithUserData::GetUserData() const
{
	return m_pUserData;
}