#include "stdafx.h"
#include "DecorateBaseWindow.h"
#include "WndDecorate.h"


CDecorateBaseWindow::CDecorateBaseWindow(void)
{

}

CDecorateBaseWindow::~CDecorateBaseWindow(void)
{
	m_vecDecorates.clear();
}

void CDecorateBaseWindow::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
//#ifndef _DEBUG
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetAttribute(lpszName, lpszValue);
		}
	}
//#endif
  	
	return __super::SetAttribute(lpszName, lpszValue);
}

void CDecorateBaseWindow::AddDecorate(shared_ptr<CWndDecorate> spWndDecorate)
{
	if (spWndDecorate)
	{
		spWndDecorate->SetWindowBase(this);
		m_vecDecorates.push_back(spWndDecorate);
	}
	
}

LRESULT CDecorateBaseWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
//#ifndef _DEBUG
 	for (auto it : m_vecDecorates)
 	{
 		if (it)
 		{
 			it->WindowProc(message, wParam, lParam);
 		}	
 	}
//#endif
	return __super::WindowProc(message, wParam, lParam);
}

void CDecorateBaseWindow::OnCreate()
{
	__super::OnCreate();
//#ifndef _DEBUG
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->OnCreate();
		}
	}
//#endif
}
