#pragma once

//simple thread pool implement
class WorkPool
{
public:
	WorkPool();
	virtual ~WorkPool();

public:
	bool				Initialize(int nMaxThread, LPTHREAD_START_ROUTINE lpStartRoutine);
	void				Destory();
	bool				Enqueue(void* pArgs);

protected:
	int					FindIdleThread();
	static DWORD WINAPI WorkThread(LPVOID lpArgs);

private:
	bool				m_bStop;
	CRITICAL_SECTION	m_lckTask;
	LPTHREAD_START_ROUTINE
						m_lpStartRoutine;
	deque<void*>		m_deqTasks;
	map<string,void*>	m_mapArgs;
	HANDLE*				m_phEvents;
	HANDLE*				m_phThreads;
	BOOL*				m_pbThreadBusy;
	int					m_nMaxThreadCount;
};
