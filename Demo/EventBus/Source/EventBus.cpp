#include "stdafx.h"
#include "EventBus.h"
#include <string>
using std::string;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include "InternalDefine.h"
#include "Dispatch.h"
#include "WorkPool.h"
#include "EventBusImpl.h"


CEventBusImpl* g_pEventbusImpl = NULL;

EVENTBUS_API
bool EventbusStartup()
{
	if (g_pEventbusImpl)
	{
		return true;
	}

	g_pEventbusImpl = new_nothrow CEventBusImpl;
	if (!g_pEventbusImpl)
	{
		return false;
	}

	if (!g_pEventbusImpl->Initialize())
	{
		EventbusShutdown();
		return false;
	}

	return true;
}

EVENTBUS_API
void EventbusShutdown()
{
	if (g_pEventbusImpl)
	{
		g_pEventbusImpl->Destory();

		delete g_pEventbusImpl;
		g_pEventbusImpl = NULL;
	}
}

EVENTBUS_API
bool EventbusAssociateWnd(HWND hMainWnd, UINT dwUserMessageType)
{
	if (!::IsWindow(hMainWnd))
	{
		return false;
	}

	if (g_pEventbusImpl)
	{
		g_pEventbusImpl->InitWinEvent(hMainWnd, dwUserMessageType);
	}

	return true;
}

EVENTBUS_API
void EventbusReceiveMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_pEventbusImpl)
	{
		g_pEventbusImpl->ReceiveWinEvent(hwnd, message, wParam, lParam);
	}
}

EVENTBUS_API
IEventBus* GetEventbus()
{
	return g_pEventbusImpl;
}
