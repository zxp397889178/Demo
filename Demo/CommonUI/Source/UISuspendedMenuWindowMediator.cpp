#include "stdafx.h"
#include "Event\SuspendedMenuEvent.hpp"
#include "CommonUI\UISuspendedMenuWindowMediator.h"

CSuspendedMenuWindowMediator::CSuspendedMenuWindowMediator()
{
	m_pTargetCtrl = NULL;
	m_hTargetWnd = NULL;
}

CSuspendedMenuWindowMediator::~CSuspendedMenuWindowMediator()
{
	if (m_hTargetWnd)
	{
		DetachMessageFilter(m_hTargetWnd);
		m_hTargetWnd = NULL;
	}
}

void CSuspendedMenuWindowMediator::OnWndCreate(CWindowUI* pWindow)
{
	__super::OnWndCreate(pWindow);
}

void CSuspendedMenuWindowMediator::OnWndDestory()
{
	__super::OnWndDestory();
}

void CSuspendedMenuWindowMediator::OnEvent(RequestCloseSuspendedMenuToTarget* pEvent)
{
	if (pEvent->hSourceWnd)
	{
		if (!m_hTargetWnd)
		{
			AttachMessageFilter(pEvent->hSourceWnd);
		}
		else
		{
			assert(m_hTargetWnd == pEvent->hSourceWnd);
		}
	}
	else
	{
		if (m_hTargetWnd)
		{
			DetachMessageFilter(m_hTargetWnd);
		}
	}
	

	m_pTargetCtrl = pEvent->pSourceCtrl;
	m_hTargetWnd = pEvent->hSourceWnd;

	if (m_pWnd)
	{
		RECT rect = { 0 };
		GetWindowRect(m_pWnd->GetHWND(), &rect);

		if (!::PtInRect(&rect, pEvent->ptScreen))
		{
			m_pWnd->CloseWindow();
		}
	}
}

HWND CSuspendedMenuWindowMediator::GetTargetId()
{
	if (!m_pWnd)
	{
		return NULL;
	}

	return m_pWnd->GetHWND();

}

CControlUI* CSuspendedMenuWindowMediator::GetTargetCtrl()
{
	return m_pTargetCtrl;
}

LRESULT CSuspendedMenuWindowMediator::OnWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KILLFOCUS)
	{
		CloseWnd();
	}
	else if (message == WM_MOUSELEAVE)
	{
		OnMouseLeave();
	}
	else if (message == WM_ACTIVATEAPP 
			&& wParam == FALSE)
	{
		CloseWnd();
	}

	return TRUE;
}

void CSuspendedMenuWindowMediator::OnMouseLeave()
{
	DWORD dwPos = GetMessagePos();
	POINTS pts = MAKEPOINTS(dwPos);

	if (GetTargetCtrl())
	{
		RequestPointHitTestToCtrl reqPointHitTestToTarget;
		reqPointHitTestToTarget.ptScreen.x = pts.x;
		reqPointHitTestToTarget.ptScreen.y = pts.y;
		reqPointHitTestToTarget.SendToTarget(GetTargetCtrl());

		if (!reqPointHitTestToTarget.bHitted)
		{
			CloseWnd();
		}
	}
	else
	{
		CloseWnd();
	}
}

LRESULT CSuspendedMenuWindowMediator::MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (!m_pWnd)
	{
		return FALSE;
	}

	if (uMsg == WM_MOUSELEAVE
		|| uMsg == WM_LBUTTONUP)
	{
		do 
		{
			DWORD dwPos = GetMessagePos();
			POINTS pts = MAKEPOINTS(dwPos);
			POINT pt = { pts.x, pts.y };
			RECT rt = { 0 };

			if (!GetWindowRect(m_pWnd->GetHWND(), &rt))
			{
				break;
			}

			if (::PtInRect(&rt, pt))
			{
				return TRUE;
			}

		} while (false);
		
		OnMouseLeave();
	}

	return TRUE;
}

void CSuspendedMenuWindowMediator::AttachMessageFilter(HWND hWnd)
{
	CWindowUI* pWindow = GetUIEngine()->GetWindow(hWnd);
	if (pWindow)
	{
		pWindow->AddMessageFilter(this);
	}
}

void CSuspendedMenuWindowMediator::DetachMessageFilter(HWND hWnd)
{
	CWindowUI* pWindow = GetUIEngine()->GetWindow(hWnd);
	if (pWindow)
	{
		pWindow->RemoveMessageFilter(this);
	}
}
