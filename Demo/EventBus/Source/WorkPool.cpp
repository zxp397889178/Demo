#include "stdafx.h"
#include "InternalDefine.h"
#include "WorkPool.h"

typedef struct  WorkThreadParam
{
	int			nThreadIndex;
	WorkPool*	pThis;

}WorkThreadParam;

WorkPool::WorkPool()
{
	m_lpStartRoutine	= NULL;
	m_phEvents			= NULL;
	m_phThreads			= NULL;
	m_pbThreadBusy		= NULL;
	m_nMaxThreadCount	= 0;
	m_bStop				= false;
}

WorkPool::~WorkPool()
{

}

bool WorkPool::Initialize(int nMaxThread, LPTHREAD_START_ROUTINE lpStartRoutine)
{
	m_nMaxThreadCount	= nMaxThread;
	if (m_nMaxThreadCount <= 0)
	{
		return false;
	}

	m_lpStartRoutine = lpStartRoutine;
	if (!m_lpStartRoutine)
	{
		return false;
	}

	m_phEvents		= new_nothrow HANDLE[m_nMaxThreadCount];
	m_phThreads		= new_nothrow HANDLE[m_nMaxThreadCount];
	m_pbThreadBusy	= new_nothrow BOOL[m_nMaxThreadCount];

	if (!m_phEvents
		|| !m_phThreads
		|| !m_pbThreadBusy)
	{

		if (m_phEvents)
		{
			delete[] m_phEvents;
			m_phEvents = NULL;
		}

		if (m_phThreads)
		{
			delete[] m_phThreads;
			m_phThreads = NULL;
		}

		if (m_pbThreadBusy)
		{
			delete[] m_pbThreadBusy;
			m_pbThreadBusy = NULL;
		}

		return false;
	}
	

	for (int i = 0; i < m_nMaxThreadCount; ++i)
	{
		m_phEvents[i]		= NULL;
		m_phThreads[i]		= NULL;
		m_pbThreadBusy[i]	= FALSE;
	}

	InitializeCriticalSection(&m_lckTask);

	return true;
}

void WorkPool::Destory()
{
	m_bStop = true;
	if (m_phEvents)
	{
		for (int i = 0; i < m_nMaxThreadCount; ++i)
		{
			HANDLE hEvent = m_phEvents[i];
			if (!hEvent)
			{
				continue;
			}
			SetEvent(hEvent);
			CloseHandle(hEvent);
		}
	}

	if (m_phThreads)
	{
		for (int i = 0; i < m_nMaxThreadCount; ++i)
		{
			HANDLE hThread = m_phThreads[i];
			if (!hThread)
			{
				continue;
			}
			
			DWORD a = WaitForSingleObject(hThread, MaxWaitThreadTime);
			if (WAIT_OBJECT_0 != a)
			{
				TerminateThread(hThread, (DWORD)-1);
			}

			CloseHandle(hThread);
		}

		delete[] m_phThreads;
		m_phThreads = NULL;

		if (m_phEvents)
		{
			delete[] m_phEvents;
			m_phEvents = NULL;
		}
	}

	if (m_pbThreadBusy)
	{
		delete[] m_pbThreadBusy;
		m_pbThreadBusy = NULL;
	}

	DeleteCriticalSection(&m_lckTask);
}

bool WorkPool::Enqueue(void* pArgs)
{
	if (m_bStop)
	{
		return false;
	}

	AutoLock lckTask(&m_lckTask);
	m_deqTasks.push_back(pArgs);

	int nIdleIndex = FindIdleThread();
	if (nIdleIndex == -1)
	{
		//undefined behavior when 'nIdleIndex = -1' and no threads work.
		return true;
	}

	WorkThreadParam* pThreadParam = new_nothrow WorkThreadParam();
	if (!pThreadParam)
	{
		return false;
	}

	do 
	{
		pThreadParam->pThis = this;
		pThreadParam->nThreadIndex = nIdleIndex;

		// check thread whether has been created
		if (m_phThreads[nIdleIndex] == NULL)
		{
			HANDLE hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
			if (!hEvent)
			{
				break;
			}

			m_phEvents[nIdleIndex] = hEvent;
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)pThreadParam, 0, NULL);
			if (!hThread)
			{
				m_phEvents[nIdleIndex] = NULL;
				CloseHandle(hEvent);
				break;
			}

			m_phThreads[nIdleIndex] = hThread;
		}

		SetEvent(m_phEvents[nIdleIndex]);

		return true;

#pragma warning(disable:4127)
	} while (false);
#pragma warning(default:4127)
	
	delete pThreadParam;


	return false;
}

int WorkPool::FindIdleThread()
{
	for (int i = 0; i < m_nMaxThreadCount; i++)
	{
		BOOL bBusy = InterlockedCompareExchange((LONG*)&m_pbThreadBusy[i], TRUE, FALSE);
		if (!bBusy)
			return i;
	}

	return -1;
}

DWORD WINAPI WorkPool::WorkThread(LPVOID lpArgs)
{
	WorkThreadParam* pThreadParam = (WorkThreadParam*)lpArgs;
	WorkPool* pThis		= pThreadParam->pThis;
	int nThreadIndex	= pThreadParam->nThreadIndex;

#pragma warning(disable:4127)
	while (true)
#pragma warning(default:4127)
	{
		
		DWORD ret = WaitForSingleObject(pThis->m_phEvents[nThreadIndex], INFINITE);
		if (ret == WAIT_FAILED)
		{
			break;
		}
			
		do 
		{
			if (pThis->m_bStop)
			{
				break;
			}

			// obtain one task from deque
			void* pOneTask = NULL;
			{
				AutoLock lckTask(&pThis->m_lckTask);
				if (pThis->m_deqTasks.size() > 0)
				{
					pOneTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			}

			if (!pOneTask)
			{
				break;
			}

			pThis->m_lpStartRoutine(pOneTask);

#pragma warning(disable:4127)
		} while (true);
#pragma warning(default:4127)
		ResetEvent(&pThis->m_phEvents[nThreadIndex]);
		InterlockedCompareExchange((LONG*)&pThis->m_pbThreadBusy[nThreadIndex], FALSE, TRUE);
	}

	delete pThreadParam;
	pThreadParam = NULL;

	if (!pThis->m_bStop)
	{
		CloseHandle(pThis->m_phEvents[nThreadIndex]);
		pThis->m_phEvents[nThreadIndex] = NULL;

		CloseHandle(pThis->m_phThreads[nThreadIndex]);
		pThis->m_phThreads[nThreadIndex] = NULL;
	}

	InterlockedCompareExchange((LONG*)&pThis->m_pbThreadBusy[nThreadIndex], FALSE, TRUE);

	return 0;
}
