/*********************************************************************
 *       Copyright (C) 2011,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIControlWnd.h"
#include <iso646.h>

CControlWndUI::CControlWndUI(void)
:m_hWindow(NULL)
,m_hWindowParent(NULL)
,m_bShow(true)
,m_dwStyle(0)
,m_dwExStyle(0)
{
	SetStyle(_T("ControlWnd"));
}

CControlWndUI::~CControlWndUI(void)
{
}


void CControlWndUI::DockWnd( HWND hWnd )
{
	if (m_hWindow && ::IsWindow(m_hWindow))
	{
		::SetWindowLong(m_hWindow, GWL_STYLE, m_dwStyle);
		::SetWindowLong(m_hWindow, GWL_EXSTYLE, m_dwExStyle);
		::SetParent(m_hWindow, m_hWindowParent);
	}
	m_hWindow = hWnd;

	CWindowUI* pWindow = GetWindow();
	if (m_hWindow && ::IsWindow(m_hWindow))
	{
		m_dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
		m_dwExStyle = ::GetWindowLong(m_hWindow, GWL_EXSTYLE);
		m_hWindowParent = ::GetParent(m_hWindow);

		::SetWindowLong(m_hWindow, GWL_STYLE, (m_dwStyle bitand compl(WS_POPUP)) or WS_CHILD);
		::SetWindowLong(m_hWindow, GWL_EXSTYLE, (m_dwExStyle bitand compl(WS_EX_APPWINDOW)));
		if (pWindow)
			::SetParent(m_hWindow, pWindow->GetHWND());
	}
	if (pWindow && pWindow->IsWindowRender())
		ShowWnd(m_bShow);
}

void CControlWndUI::ShowWnd( bool bShow )
{
	m_bShow = bShow;
	OffsetWnd(m_bShow, true);
}

void CControlWndUI::OffsetWnd(bool bShow, bool bChangeRect)
{
	if (bShow)
	{
		::ShowWindow(m_hWindow, SW_SHOW);
	}
	else
	{
		::ShowWindow(m_hWindow, SW_HIDE);
	}
	if (bChangeRect)
	{
		if (::IsWindow(m_hWindow))
		{
			RECT rcItem = GetRect();
			::SetWindowPos(m_hWindow, HWND_DESKTOP, rcItem.left, rcItem.top, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

void CControlWndUI::SetRect( RECT& rectRegion )
{
	__super::SetRect(rectRegion);
	OffsetWnd(IsVisible(), true);
}

void CControlWndUI::SetVisible( bool bShow)
{
	__super::SetVisible(bShow);
	OffsetWnd(IsVisible(), false);
}

void CControlWndUI::SetInternVisible( bool bVisible )
{
	__super::SetInternVisible(bVisible);
	OffsetWnd(IsVisible() && bVisible, false);
}

void CControlWndUI::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	__super::Render(pRenderDC, rcPaint);
	CWindowUI* pWindow = GetWindow();
	if (!pWindow->IsWindowRender())
	{
		if (m_bShow)
		{
			OffsetWnd(true, false);
		}
	}
}