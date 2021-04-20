#include "stdafx.h"
#include "CommonUI\WindowPtrMap.h"
#include "Event\SuspendedMenuEvent.hpp"
#include "CommonUI\UISuspendedMenuCtrlMeditor.h"

//////////////////////////////////////////////////////////////////////////

CSuspendedMenuCtrlMeditorUI::CSuspendedMenuCtrlMeditorUI()
{

}

CSuspendedMenuCtrlMeditorUI::~CSuspendedMenuCtrlMeditorUI()
{

}

bool CSuspendedMenuCtrlMeditorUI::OnCtrlEvent(TEventUI& msg)
{
	if (!m_pOwner)
	{
		return true;
	}

	if (msg.nType == UIEVENT_MOUSEENTER)
	{
		RequestCreateSuspendedMenuToTarget reqCreateSuspendedMenuToTarget;
		reqCreateSuspendedMenuToTarget.hWnd = m_pOwner->GetWindow()->GetHWND();
		reqCreateSuspendedMenuToTarget.rt = msg.pSender->GetRect();
		reqCreateSuspendedMenuToTarget.SendToTarget(m_pOwner);
	}
	else if (msg.nType == UIEVENT_MOUSELEAVE)
	{
		std::shared_ptr<IWindowPtrMap> spWindowPtrMap = GetWindowPtrMap();
		if (!spWindowPtrMap)
		{
			return true;
		}

		auto pWindow = dynamic_cast<CWindowUI *>(spWindowPtrMap->FindWnd(m_pOwner->GetWindow()->GetHWND()));
		if (pWindow != NULL)
		{
			DWORD dwPos = GetMessagePos();
			POINTS pts = MAKEPOINTS(dwPos);
			POINT pt = {pts.x, pts.y};

			RequestCloseSuspendedMenuToTarget reqCloseSuspendedMenuToTarget;
			reqCloseSuspendedMenuToTarget.ptScreen = pt;
			reqCloseSuspendedMenuToTarget.pSourceCtrl = m_pOwner;

			if (m_pOwner->GetWindow())
			{
				reqCloseSuspendedMenuToTarget.hSourceWnd = m_pOwner->GetWindow()->GetHWND();
			}
			
			reqCloseSuspendedMenuToTarget.SendToTarget(pWindow->GetHWND());

		}
	}


	return true;
}

void CSuspendedMenuCtrlMeditorUI::OnEvent(RequestCreateSuspendedMenuToTarget* pEvent)
{
	std::shared_ptr<IWindowPtrMap> spWindowPtrMap = GetWindowPtrMap();
	if (!spWindowPtrMap)
	{
		return;
	}

	auto pWindow = dynamic_cast<CWindowUI *>(spWindowPtrMap->FindWnd(pEvent->hWnd));
	if (pWindow != NULL)
	{
		pWindow->ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
	{
		OnCreateMenu(spWindowPtrMap, pEvent);
	}

}

void CSuspendedMenuCtrlMeditorUI::OnEvent(RequestPointHitTestToCtrl* pEvent)
{
	if (!m_pOwner)
	{
		return;
	}

	HWND hWnd = m_pOwner->GetWindow()->GetHWND();

	RECT rect = m_pOwner->GetRect();

	::ClientToScreen(hWnd, (LPPOINT)&rect.left);
	::ClientToScreen(hWnd, (LPPOINT)&rect.right);

	pEvent->bHitted = !!::PtInRect(&rect, pEvent->ptScreen);
}

CControlUI* CSuspendedMenuCtrlMeditorUI::GetTargetId()
{
	return m_pOwner;
}

void CSuspendedMenuCtrlMeditorUI::OnCreateMenu(std::shared_ptr<IWindowPtrMap> spWindowPtrMap, RequestCreateSuspendedMenuToTarget* pEvent)
{

}

