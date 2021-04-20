#pragma once

#include <string>
using std::string;

#include <map>
using std::map;
using std::make_pair;

#include <vector>
using std::vector;

#include <deque>
using std::deque;

//The object is automatically released when it leaves its scope(only use by object release).
//Remark: use multiple ¡°AutoRelease¡± in the same scope, pay attention to the order
template<typename T>
class AutoRelease
{
public:
	AutoRelease(T*& obj)
	{
		if (!obj)
		{
			_obj = NULL;
			return;
		}

		_obj = &obj;
	}

	~AutoRelease()
	{
		if (_obj
			&& *_obj)
		{
			delete *_obj;
			*_obj = NULL;
		}
	}

private:
	T** _obj;

};

typedef struct AutoTryLock
{
	AutoTryLock(CRITICAL_SECTION* lck)
	{
		_lck = lck;
		_can_enter = !!::TryEnterCriticalSection(_lck);
	}

	~AutoTryLock()
	{
		if (_can_enter)
		{
			::LeaveCriticalSection(_lck);
		}
	}

	bool TryEnter(bool bAlwaysEnter = false)
	{
		if (!_can_enter
			&& bAlwaysEnter)
		{
			_can_enter = true;
			EnterCriticalSection(_lck);
		}

#ifdef DEBUG
		if (!bAlwaysEnter
			&& !_can_enter)
		{
			int a = 0;
			a++;
		}
#endif
		return _can_enter;
	}

	bool				_can_enter;
	CRITICAL_SECTION*	_lck;

}AutoTryLock;

class CEventBusImpl;
struct EventThreadArgs;

typedef vector<IObserver*>					InvokerList;
typedef map<string, InvokerList*>			ObserversMap;
typedef map<string, EventThreadArgs*>		ThreadsMap;

typedef struct EventThreadArgs
{
	CEventBusImpl*		pEventBus;
	IEvent*				pEvent;
	IObserver*			pBusyObserver;
	InvokerList			invokerList;
	CRITICAL_SECTION	lckInvokerList;
	HANDLE				hThread;

	EventThreadArgs()
	{
		pBusyObserver = NULL;
		InitializeCriticalSection(&lckInvokerList);
	}

	~EventThreadArgs()
	{
		{
			AutoLock lock(&lckInvokerList);
			invokerList.clear();
		}
		
		DeleteCriticalSection(&lckInvokerList);
	}

	IObserver* PeekObserver()
	{
		AutoLock lock(&lckInvokerList);

		if (invokerList.empty())
		{
			pBusyObserver = NULL;
		}
		else
		{
			pBusyObserver = invokerList.back();
			invokerList.pop_back();
		}

		return pBusyObserver;
	}

	bool RemoveObserver(IObserver* pObserver)
	{
		AutoLock lock(&lckInvokerList);

		if (invokerList.empty())
		{
			return true;
		}

		if (pBusyObserver != pObserver)
		{
			for (InvokerList::iterator itor = invokerList.begin(); itor != invokerList.end(); ++itor)
			{
				if (*itor == pObserver)
				{
					invokerList.erase(itor);
					return true;
				}
			}
		}

		return false;
	}

}EventThreadArgs;
