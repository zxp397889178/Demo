#include "stdafx.h"
#include "UIMediator.h"
#include "WindowPtrRef.h"
#include "UIAutoSizeButton.h"
#include "UISuspended.h"


CSuspendedWindowUI::CSuspendedWindowUI()
{
	m_pMediator = NULL;
	m_bInited = false;
}

CSuspendedWindowUI::~CSuspendedWindowUI()
{
	if (m_pMediator)
	{
		delete m_pMediator;
		m_pMediator = NULL;
	}
}

void CSuspendedWindowUI::OnCreate()
{
	__super::OnCreate();
	if (m_pMediator)
	{
		m_bInited = true;
		m_pMediator->OnWndCreate(this);
	}
}

void CSuspendedWindowUI::OnClose()
{
	if (m_pMediator)
	{
		m_pMediator->OnWndDestory();
	}

	__super::OnClose();
}

LRESULT CSuspendedWindowUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pMediator)
	{
		if (!m_pMediator->OnWndProc(message, wParam, lParam))
		{
			return FALSE;
		}
	}

	return __super::WindowProc(message, wParam, lParam);
}

void CSuspendedWindowUI::SetWindowMediator(CWindowMediatorUI* pMediator)
{
	if (!pMediator)
	{
		return;
	}

	if (m_pMediator)
	{
		delete m_pMediator;
		m_pMediator = NULL;
	}

	m_pMediator = pMediator;
}

//////////////////////////////////////////////////////////////////////////

CSuspendedMenuButtonUI::CSuspendedMenuButtonUI()
{
	m_bInited = false;
	m_pMediator = NULL;
}

CSuspendedMenuButtonUI::~CSuspendedMenuButtonUI()
{
	if (m_pMediator)
	{
		delete m_pMediator;
		m_pMediator = NULL;
	}
}

void CSuspendedMenuButtonUI::Init()
{
	__super::Init();

	if (m_pMediator
		&& !m_bInited)
	{
		m_bInited = true;
		m_pMediator->OnCtrlInit(this);
	}

}

void CSuspendedMenuButtonUI::OnDestroy()
{
	if (m_pMediator)
	{
		m_pMediator->OnCtrlDestory();
	}
	__super::OnDestroy();
}

void CSuspendedMenuButtonUI::SetCtrlMediator(CControlMediatorUI* pMediator)
{
	if (!pMediator)
	{
		return;
	}

	if (m_pMediator)
	{
		m_bInited = false;
		delete m_pMediator;
		m_pMediator = NULL;
	}

	m_pMediator = pMediator;

	if (m_pMediator
		&& !m_bInited)
	{
		m_bInited = true;
		m_pMediator->OnCtrlInit(this);
	}
}