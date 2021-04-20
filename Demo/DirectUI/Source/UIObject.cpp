/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "StdAfx.h"
#include "UIObject.h"

CObjectUI::CObjectUI(void)
: m_hWnd(0)
{
	SetType(_T("Object"));
	SetAnchor(UITYPE_ANCHOR::UIANCHOR_LEFTTOP);
	SetMouseEnabled(true);

	ModifyControlFlags(UICONTROLFLAG_SETFOCUS, 0);
}

CObjectUI::~CObjectUI(void)
{
}

UI_IMPLEMENT_DYNCREATE(CObjectUI);

void CObjectUI::Docked(HWND hWnd)
{
	m_hWnd = hWnd;
}


void CObjectUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	
	if (m_hWnd) 
	{
		UINT  flags = SWP_NOZORDER | SWP_NOACTIVATE;
		::SetWindowPos(m_hWnd, NULL, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, flags);			
	}
}
