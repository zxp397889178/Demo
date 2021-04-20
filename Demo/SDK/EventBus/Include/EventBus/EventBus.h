#pragma once

#ifndef EVENTBUS_STATICLIB
	#ifdef EVENTBUS_EXPORTS
	#define EVENTBUS_API __declspec(dllexport)
	#else
	#define EVENTBUS_API __declspec(dllimport)
	#define EVENTBUS_INCLUDE
	#endif
#else
	#define EVENTBUS_API 
	#define EVENTBUS_INCLUDE
#endif

#ifndef new_nothrow
#define new_nothrow  new(std::nothrow)
#endif

#ifndef eventbus_auto_lock
#define eventbus_auto_lock
typedef struct AutoLock
{
	AutoLock(CRITICAL_SECTION* lck)
	{
		_lck = lck;
		::EnterCriticalSection(_lck);
	}

	~AutoLock()
	{
		::LeaveCriticalSection(_lck);
	}

	CRITICAL_SECTION* _lck;

}AutoLock;
#endif

#ifndef MaxWaitThreadTime
#define MaxWaitThreadTime 100
#endif

class IObserver;
class IEvent;
class IDispatcher;
class IEventBus;

//call on applicaion init
EVENTBUS_API
bool
EventbusStartup();


EVENTBUS_API
bool
EventbusAssociateWnd(
	HWND hMainWnd,
	UINT dwUserMessageType);


EVENTBUS_API
void
EventbusReceiveMessage(
	HWND        hwnd,
	UINT        message,
	WPARAM      wParam,
	LPARAM      lParam);

EVENTBUS_API
IEventBus*
GetEventbus();

//call on applicaion exit
EVENTBUS_API
void
EventbusShutdown();


#ifdef EVENTBUS_INCLUDE
#include "EventBus/Target.h"
#include "EventBus/Event.h"
#include "EventBus/Observer.h"
#include "EventBus/Dispatch.h"
#include "EventBus/EventTargeter.hpp"
#include "EventBus/EventObserver.hpp"
#include "EventBus/EventPublisher.hpp"
#endif
