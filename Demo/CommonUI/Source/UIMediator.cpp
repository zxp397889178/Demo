#include "stdafx.h"
#include "UIMediator.h"

CControlMediatorUI::CControlMediatorUI()
{
	m_pOwner = NULL;
}

CControlMediatorUI::~CControlMediatorUI()
{
	OnCtrlDestory();
}

void CControlMediatorUI::OnCtrlInit(CControlUI* pOwner)
{
	m_pOwner = pOwner;

	if (pOwner)
	{
		pOwner->OnEvent += MakeDelegate(this, &CControlMediatorUI::OnCtrlEvent);
	}

}

void CControlMediatorUI::OnCtrlDestory()
{
	if (m_pOwner)
	{
		m_pOwner->OnEvent -= MakeDelegate(this, &CControlMediatorUI::OnCtrlEvent);
	}

	m_pOwner = NULL;
}

bool CControlMediatorUI::OnCtrlEvent(TEventUI& msg)
{
	return true;
}

CControlUI* CControlMediatorUI::GetCtrl()
{
	return m_pOwner;
}


//////////////////////////////////////////////////////////////////////////


CWindowMediatorUI::CWindowMediatorUI()
{
	m_pWnd = NULL;
}

CWindowMediatorUI::~CWindowMediatorUI()
{

}

void CWindowMediatorUI::OnWndCreate(CWindowUI* pWindow)
{
	m_pWnd = pWindow;
}

void CWindowMediatorUI::OnWndDestory()
{
	m_pWnd = NULL;
}

LRESULT CWindowMediatorUI::OnWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

CWindowUI* CWindowMediatorUI::GetWnd()
{
	return m_pWnd;
}

void CWindowMediatorUI::CloseWnd()
{
	if (m_pWnd)
	{
		m_pWnd->CloseWindow();
	}
}
