#pragma once

class CDirChangeNotification;

class CDelayedNotifier
{
public:
	virtual ~CDelayedNotifier(){}
	virtual void PostNotification(CDirChangeNotification * pNotification) = 0;
};

class CDelayedNotificationWindow : public CDelayedNotifier
{
public:
	CDelayedNotificationWindow(){  AddRef(); }
	virtual ~CDelayedNotificationWindow(){ Release(); }

public:
	void PostNotification(CDirChangeNotification * pNotification);
private:
	LONG AddRef();		//	the window handle is reference counted
	LONG Release();		//
	BOOL RegisterWindowClass(LPCTSTR szClassName);
	BOOL CreateNotificationWindow();
private:
	static LRESULT CALLBACK DelayedNotificationWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static LONG s_nRefCnt;
	static HWND s_hWnd; //there's only one window no matter how many instances of this class there are.... this means that all notifications are handled by the same thread.
	static BOOL s_bRegisterWindow;
};

//
//	Class that implements a worker thread w/ a message pump.
//	CDirectoryChangeWatcher posts notifications to this thread, where they are dispatched.
//	This thread executes CDirectoryChangeHandler notifications.
//
class CDelayedNotificationThread : public CDelayedNotifier
{
public:
	CDelayedNotificationThread()
		:m_hThreadStartEvent(NULL)
	{ 
		m_hThreadStartEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		ASSERT( m_hThreadStartEvent );
		AddRef(); 
	}
	virtual ~CDelayedNotificationThread()
	{ 
		Release(); 
		if( m_hThreadStartEvent ) 
			CloseHandle(m_hThreadStartEvent), m_hThreadStartEvent = NULL;
	}

	void PostNotification(CDirChangeNotification * pNotification);

private:
	LONG AddRef();					// The thread handle is reference
	LONG Release();					// counted so that only one thread is used
	// so that there's only one worker thread(performing this functino)
	static UINT __stdcall ThreadFunc(LPVOID lpvThis);

	BOOL StartThread();
	BOOL StopThread();

	BOOL WaitForThreadStartup(){ return WaitForSingleObject(m_hThreadStartEvent, INFINITE) == WAIT_OBJECT_0; };
	BOOL SignalThreadStartup(){ return SetEvent( m_hThreadStartEvent ) ; }
private:
	static LONG		s_nRefCnt;		// no matter how many directories are being watched
	static HANDLE	s_hThread;		//	
	static DWORD	s_dwThreadID;	//  
	HANDLE m_hThreadStartEvent;//signals that the worker thread has started. this fixes a bug condition.

};