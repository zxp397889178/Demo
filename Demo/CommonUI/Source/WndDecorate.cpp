#include "stdafx.h"
#include "CommonUI.h"
#include "WndDecorate.h"

CWndDecorate::CWndDecorate()
{

}

CWndDecorate::~CWndDecorate()
{

}

void CWndDecorate::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{

}

LRESULT CWndDecorate::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CWndDecorate::OnCreate()
{

}

void CWndDecorate::SetWindowBase(CWindowUI *pWindow)
{
	m_pWindow = pWindow;
}
