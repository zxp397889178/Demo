#include "stdafx.h"
#include "InternalDefine.h"
#include "Dispatch.h"
#include "WorkPool.h"
#include "Target.h"
#include "Observer.h"
#include "Event.h"
#include "EventBusImpl.h"


CEventBusImpl::CEventBusImpl()
{
	m_hWnd			= NULL;
	m_dwMessage		= 0;
	m_pThreadPool	= NULL;
	m_bInited		= false;
}

CEventBusImpl::~CEventBusImpl()
{

}

void CEventBusImpl::ConnectToBus(IObserver* pObserver)
{
	if (!m_bInited)
	{
		return;
	}

	LPCSTR lpcsEvtId = pObserver->GetEventId();

	//lock observer map
	AutoLock lock(&m_lckObserversMap);

	ObserversMap::iterator itor = m_EventObserver.find(lpcsEvtId);
	if (itor != m_EventObserver.end())
	{
		InvokerList* pInvokerList = itor->second;
		pInvokerList->insert(pInvokerList->end(), pObserver);
	}
	else
	{
		InvokerList* pInvokerList = new_nothrow InvokerList;
		if (pInvokerList)
		{
			pInvokerList->insert(pInvokerList->end(), pObserver);
			m_EventObserver[lpcsEvtId] = pInvokerList;
		}
	}
}

void CEventBusImpl::DisconnectFromBus(IObserver* pObserver)
{
	if (!m_bInited)
	{
		return;
	}

	LPCSTR lpcsId = pObserver->GetEventId();
	{
		//remove it from observer map
		AutoLock lock(&m_lckObserversMap);

		ObserversMap::iterator itorList = m_EventObserver.find(lpcsId);
		if (itorList != m_EventObserver.end())
		{
			for (InvokerList::iterator itor = itorList->second->begin();
				itor != itorList->second->end();
				++itor)
			{
				if (*itor == pObserver)
				{
					itorList->second->erase(itor);
					break;
				}
			}
		}
	}

	IEvent* pEvent = NULL;
	{
		AutoLock lock(&m_lckThreadsMap);

		ThreadsMap::iterator itorList = m_Threads.find(lpcsId);
		if (itorList != m_Threads.end())
		{
			EventThreadArgs* pArgs = itorList->second;
			if (!pArgs->RemoveObserver(pObserver))
			{
				WaitForSingleObject(pArgs->hThread, MaxWaitThreadTime);
				TerminateThread(pArgs->hThread, (DWORD)-1);
				CloseHandle(pArgs->hThread);
				pEvent = pArgs->pEvent->Copy();

				delete pArgs->pEvent;
				delete pArgs;

				m_Threads.erase(itorList);
			}
		}

		ThreadsMap::iterator itorEventArgs = m_mapThreadPoolEvents.find(lpcsId);
		if (itorEventArgs != m_mapThreadPoolEvents.end())
		{
			EventThreadArgs* pArgs = itorList->second;
			if (pArgs
				&& !pArgs->RemoveObserver(pObserver))
			{
				//undefined behavior
			}
		}
	}

	if (pEvent)
	{
		PostInBackground(pEvent);
		delete pEvent;
	}
}

void CEventBusImpl::Send(IEvent* pEvent)
{
	if (!m_bInited)
	{
		return;
	}

	if (m_dwMainThreadId != GetCurrentThreadId())
	{
		if (!m_hWnd)
		{
			//must invoke InitMainHwnd before
			return;
		}

		::SendMessage(m_hWnd, m_dwMessage, 0, (LPARAM)pEvent);
	}
	else
	{
		DispatchEvent(&m_EventObserver, &m_lckObserversMap, pEvent, false);
	}
}

void CEventBusImpl::Sending(IEvent* pEvent)
{
	if (!m_bInited)
	{
		return;
	}

	DispatchEvent(&m_EventObserver,
		&m_lckObserversMap,
		pEvent,
		m_dwMainThreadId != GetCurrentThreadId());
}

void CEventBusImpl::Post(IEvent* pEvent)
{
	if (!m_bInited)
	{
		return;
	}

	if (!m_hWnd)
	{
		//must invoke InitMainHwnd before
		return;
	}

	IEvent* pCopyEvent = pEvent->Copy();
	if (!pCopyEvent)
	{
		//may be alloc failed
		return;
	}

	::PostMessage(m_hWnd, m_dwMessage, (WPARAM)TRUE, (LPARAM)pCopyEvent);
}

void CEventBusImpl::PostInBackground(IEvent* pEvent)
{
	if (!m_bInited)
	{
		return;
	}

	LPCSTR lpId = pEvent->GetId();

	InvokerList invokerList;
	{
		AutoLock lock(&m_lckObserversMap);

		ObserversMap::iterator itorList = m_EventObserver.find(lpId);
		if (itorList != m_EventObserver.end())
		{
			invokerList = *itorList->second;
		}
	}


	//lock threads map
	AutoLock lock(&m_lckThreadsMap);

	ThreadsMap::iterator itor = m_Threads.find(lpId);
	if (itor != m_Threads.end())
	{
		//already exists
		return;
	}

	EventThreadArgs* pThreadArgs = new_nothrow EventThreadArgs;
	if (!pThreadArgs)
	{
		//may be alloc failed.
		return;
	}

	pThreadArgs->pEventBus = this;
	pThreadArgs->pEvent = pEvent->Copy();

	if (!pThreadArgs->pEvent)
	{
		//may be alloc failed.
		delete pThreadArgs;
		return;
	}

	pThreadArgs->invokerList = invokerList;
	pThreadArgs->hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CEventBusImpl::DoInBackground, pThreadArgs, 0, NULL);
	if (!pThreadArgs->hThread)
	{
		//CreateThread failed.
		delete pThreadArgs;
		return;
	}

	m_Threads[lpId] = pThreadArgs;
}

void CEventBusImpl::PostInWorkPool(IEvent* pEvent)
{
	if (!m_bInited)
	{
		return;
	}

	if (!m_pThreadPool)
	{
		return;
	}

	EventThreadArgs* pThreadArgs = new_nothrow EventThreadArgs;
	if (!pThreadArgs)
	{
		//may be alloc failed.
		return;
	}

	pThreadArgs->pEventBus = this;
	pThreadArgs->pEvent = pEvent->Copy();
	//pThreadArgs->invokerList = invokerList;
	pThreadArgs->hThread = NULL;
	if (!pThreadArgs->pEvent)
	{
		//may be alloc failed.
		delete pThreadArgs;
		return;
	}

	if (!m_pThreadPool->Enqueue(pThreadArgs))
	{
		delete pThreadArgs;
		return;
	}
}

bool CEventBusImpl::Initialize()
{
	if (m_bInited)
	{
		return true;
	}

	m_pThreadPool = new_nothrow WorkPool();
	if (!m_pThreadPool)
	{
		return false;
	}

	if (!m_pThreadPool->Initialize(5, (LPTHREAD_START_ROUTINE)&CEventBusImpl::DoInWorkPool))
	{
		delete m_pThreadPool;
		m_pThreadPool = NULL;
		return false;
	}

	InitializeCriticalSection(&m_lckObserversMap);
	InitializeCriticalSection(&m_lckThreadsMap);
	m_dwMainThreadId = GetCurrentThreadId();
	m_bInited = true;

	return true;
}

void CEventBusImpl::Destory()
{
	if (!m_bInited)
	{
		return;
	}

	m_bInited = false;

	{
		AutoLock lock(&m_lckObserversMap);
		for (ObserversMap::iterator itor = m_EventObserver.begin(); itor != m_EventObserver.end(); ++itor)
		{
			InvokerList* pList = itor->second;
			pList->clear();

			delete pList;
		}
		m_EventObserver.clear();
	}

	{
		AutoLock lock(&m_lckThreadsMap);
		for (ThreadsMap::iterator itor = m_Threads.begin(); itor != m_Threads.end(); ++itor)
		{
			EventThreadArgs* pArgs = itor->second;
			WaitForSingleObject(pArgs->hThread, MaxWaitThreadTime);
			TerminateThread(pArgs->hThread, (DWORD)-1);
			CloseHandle(pArgs->hThread);

			//delete pArgs->pEvent;
			//delete pArgs;
		}
		m_Threads.clear();
	}

	if (m_pThreadPool)
	{
		m_pThreadPool->Destory();
		delete m_pThreadPool;
		m_pThreadPool = NULL;
	}

	DeleteCriticalSection(&m_lckObserversMap);
	DeleteCriticalSection(&m_lckThreadsMap);
}

void CEventBusImpl::InitWinEvent(HWND hWnd, UINT dwMessage)
{
	if (!hWnd
		|| dwMessage <= WM_USER)
	{
		return;
	}

	m_hWnd = hWnd;
	m_dwMessage = dwMessage;
}

void CEventBusImpl::ReceiveWinEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hwnd);

	if (!m_bInited)
	{
		return;
	}

	if (message != m_dwMessage
		|| lParam == 0)
	{
		return;
	}

	IEvent* pEvent = (IEvent*)lParam;

	DispatchEvent(&m_EventObserver, &m_lckObserversMap, pEvent, false);

	if (wParam == TRUE)
	{
		delete pEvent;
	}
}

void CEventBusImpl::DispatchEvent(ObserversMap* pObserversMap, CRITICAL_SECTION* pObserverLck, IEvent* pEvent, bool bAsyc)
{
	LPCSTR lpId = pEvent->GetId();

	//temp define 
	typedef map<IObserver*, IObserver*>	InvokerMap;
	//invoked map
	InvokerMap invokedMap;

__Vertify:

	size_t nOriginalCount = 0;
	InvokerList invokerListCopy;
	{
		AutoLock lock(pObserverLck);

		ObserversMap::iterator itorList = pObserversMap->find(lpId);
		if (itorList != pObserversMap->end())
		{
			invokerListCopy = *itorList->second;
			nOriginalCount = itorList->second->size();
		}
	}

	for (InvokerList::iterator itor = invokerListCopy.begin();
		itor != invokerListCopy.end();
		++itor)
	{
		IObserver* pObserver = *itor;
		InvokerMap::iterator itorFind = invokedMap.find(pObserver);
		if (itorFind != invokedMap.end())
		{
			//had be invoked.
			continue;
		}

		//dispatch without lock!!!
		this->Dispatch(pObserver, pEvent, bAsyc);

		invokedMap.insert(make_pair(pObserver, pObserver));

		bool bCheckInvaild = false;

		{
			AutoLock lock(pObserverLck);

			ObserversMap::iterator itorList = pObserversMap->find(lpId);
			if (itorList != pObserversMap->end())
			{
				if (itorList->second->size() < nOriginalCount)
				{

					//one of observer in list may be disconnected
					bCheckInvaild = true;
				}
			}
		}

		if (bCheckInvaild)
		{
			goto __Vertify;
		}
	}
}

void CEventBusImpl::Dispatch(IObserver* pObserver, IEvent* pEvent, bool bAsyc)
{
	ITarget* pTarget = pEvent->GetTarget();
	if (pTarget)
	{
		ITarget* pAsTarget = pObserver->AsTarget();
		if (!pAsTarget)
		{
			return;
		}

		if (!pTarget->IsEqual(pAsTarget))
		{
			return;
		}
	}

	if (bAsyc == pObserver->IsAsyc())
	{
		pObserver->Update(pEvent);
	}
}

void CEventBusImpl::DispatchInBackground(EventThreadArgs* pThreadArgs, IEvent* pEvent)
{
#pragma warning(disable:4127)
	while (true)
#pragma warning(default:4127)
	{
		IObserver* pObserver = pThreadArgs->PeekObserver();
		if (!pObserver)
		{
			break;
		}

		this->Dispatch(pObserver, pEvent, true);
	}
}

DWORD WINAPI CEventBusImpl::DoInBackground(LPVOID pArgs)
{
	EventThreadArgs* pThreadArgs = (EventThreadArgs*)pArgs;
	CEventBusImpl*	pEventBus = pThreadArgs->pEventBus;
	IEvent*			pEvent = pThreadArgs->pEvent;

	AutoRelease<IEvent> at2(pEvent);

	if (pEventBus)
	{
		pEventBus->DispatchInBackground(pThreadArgs, pEvent);
	}
	
	//remove current thread
	if (pThreadArgs->hThread != NULL)
	{
		AutoTryLock lock(&pEventBus->m_lckThreadsMap);
		if (!lock.TryEnter(pEventBus->m_bInited))
		{
			return (DWORD)-1;
		}

		ThreadsMap::iterator itor = pEventBus->m_Threads.find(pEvent->GetId());
		if (itor != pEventBus->m_Threads.end())
		{
			AutoRelease<EventThreadArgs> at1(pThreadArgs);
			pEventBus->m_Threads.erase(itor);
		}
	}

	return 0;
}

void WINAPI CEventBusImpl::DoInWorkPool(LPVOID pArgs)
{
	EventThreadArgs* pThreadArgs = (EventThreadArgs*)pArgs;
	CEventBusImpl*	pEventBus = pThreadArgs->pEventBus;
	IEvent*			pEvent = pThreadArgs->pEvent;

	AutoRelease<IEvent> at2(pEvent);

	LPCSTR lpId = pEvent->GetId();
	{
		AutoTryLock lock(&pEventBus->m_lckObserversMap);
		if (!lock.TryEnter(pEventBus->m_bInited))
		{
			return;
		}

		ObserversMap::iterator itorList = pEventBus->m_EventObserver.find(lpId);
		if (itorList != pEventBus->m_EventObserver.end())
		{
			pThreadArgs->invokerList = *itorList->second;
		}
	}

	{
		AutoTryLock lock(&pEventBus->m_lckThreadsMap);
		if (!lock.TryEnter(pEventBus->m_bInited))
		{
			return;
		}
		pEventBus->m_mapThreadPoolEvents[lpId] = pThreadArgs;
	}

	if (pEventBus)
	{
		pEventBus->DispatchInBackground(pThreadArgs, pEvent);
	}

	{
		AutoTryLock lock(&pEventBus->m_lckThreadsMap);
		if (!lock.TryEnter(pEventBus->m_bInited))
		{
			return;
		}

		ThreadsMap::iterator itorEventArgs = pEventBus->m_mapThreadPoolEvents.find(lpId);
		if (itorEventArgs != pEventBus->m_mapThreadPoolEvents.end())
		{
			AutoRelease<EventThreadArgs> at1(pThreadArgs);
			pEventBus->m_mapThreadPoolEvents.erase(itorEventArgs);
		}
	}
}

