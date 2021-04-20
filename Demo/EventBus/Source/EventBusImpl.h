#pragma once


class CEventBusImpl : 
	public IEventBus
{
public:
	CEventBusImpl();
	virtual ~CEventBusImpl();

public:
	//invoke on main thread
	virtual	bool
		Initialize();

	virtual	void
		Destory();

	virtual void
		InitWinEvent(
			HWND hWnd,
			UINT dwMessage);

	virtual void
		ReceiveWinEvent(
			HWND	hwnd,
			UINT	message,
			WPARAM	wParam,
			LPARAM	lParam);

	//implement
protected:
	virtual void
		ConnectToBus(IObserver* pObserver);

	virtual	void
		DisconnectFromBus(IObserver* pObserver);

	virtual void
		Send(IEvent* pEvent);

	virtual void
		Sending(IEvent* pEvent);

	virtual void
		Post(IEvent* pEvent);

	virtual void
		PostInBackground(IEvent* pEvent);

	virtual void
		PostInWorkPool(IEvent* pEvent);

private:
	virtual void	
		DispatchEvent(
			ObserversMap*		pObserversMap,
			CRITICAL_SECTION*	pObserverLck,
			IEvent*				pEvent,
			bool				bAsyc);

	virtual	void
		Dispatch(
			IObserver*	pObserver,
			IEvent*		pEvent,
			bool		bAsyc);

	virtual	void 
		DispatchInBackground(
			EventThreadArgs*	pThreadArgs,
			IEvent*				pEvent);

	static DWORD WINAPI
		DoInBackground(LPVOID pArgs);

	static void WINAPI
		DoInWorkPool(LPVOID pArgs);

private:
	WorkPool*						m_pThreadPool;
	CRITICAL_SECTION				m_lckObserversMap;
	CRITICAL_SECTION				m_lckThreadsMap;
	DWORD							m_dwMainThreadId;

	bool							m_bInited;
	HWND							m_hWnd;
	UINT							m_dwMessage;
	ThreadsMap						m_mapThreadPoolEvents;
	ObserversMap					m_EventObserver;
	ThreadsMap						m_Threads;
};

