#include "StdAfx.h"
#include "UIDockTool.h"

map<HWND, CChildDockTool*> g_sWndMap;
CChildDockTool::CChildDockTool()
{
	m_hParent = NULL;
	m_hChild = NULL;
	m_pWindowChild = NULL;
	m_oldProcParent = NULL;

}

CChildDockTool::~CChildDockTool()
{
	map<HWND, CChildDockTool*>::const_iterator it = g_sWndMap.find(m_hParent);
	if (it != g_sWndMap.end())
		g_sWndMap.erase(it);

	if (IsWindow(m_hParent) && m_oldProcParent)
		::SetWindowLong(m_hParent, GWL_WNDPROC, (LONG)m_oldProcParent);
}

HWND CChildDockTool::GetParent()
{
	return m_hParent;
}

HWND CChildDockTool::GetChild()
{
	return m_hChild;

}

void CChildDockTool::Dock(HWND hParent, HWND hChild, const Coordinate& coordinate)
{
	m_hParent = hParent;
	m_hChild = hChild;
	m_pWindowChild = GetUIEngine()->GetWindow(hChild);
	if (m_pWindowChild)
	{
		m_pWindowChild->EnableMove(false);
		m_pWindowChild->EnableResize(false);
		if (!IsWindowVisible(hChild))
			m_pWindowChild->ShowWindow();
	}

	m_oldProcParent = (WNDPROC)::SetWindowLong(hParent, GWL_WNDPROC, (LONG)ParentWindowProc);
	g_sWndMap[hParent] = this;
	m_coordinate = coordinate;
	MoveChild();
}

void CChildDockTool::Dock(HWND hParent, HWND hChild, const RECT& coordinateValue, UITYPE_ANCHOR anchor)
{
	Coordinate coordinate;
	coordinate.rect = coordinateValue;
	coordinate.anchor = anchor;
	Dock(hParent, hChild, coordinate);
}

void CChildDockTool::Dock(CControlUI* pControl, HWND hChild)
{
	Dock(pControl->GetWindow()->GetHWND(), hChild, *pControl->GetCoordinate());
}


void CChildDockTool::UnDock()
{
	map<HWND, CChildDockTool*>::const_iterator it = g_sWndMap.find(m_hParent);
	if (it != g_sWndMap.end())
		g_sWndMap.erase(it);

	if (IsWindow(m_hParent) && m_oldProcParent)
		::SetWindowLong(m_hParent, GWL_WNDPROC, (LONG)m_oldProcParent);
	m_oldProcParent = NULL;

}

LRESULT CALLBACK CChildDockTool::ParentWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CChildDockTool* pDockTool = NULL;
	WNDPROC oldProc = NULL;
	map<HWND, CChildDockTool*>::const_iterator it = g_sWndMap.find(hWnd);
	if (it != g_sWndMap.end())
	{
		pDockTool = it->second;
		oldProc = pDockTool->m_oldProcParent;
	}

	if (pDockTool == NULL || oldProc == NULL)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT lr = 0;
	if (pDockTool->GetChild() && !IsWindow(pDockTool->GetChild()))
	{
		lr = ::CallWindowProc(oldProc, hWnd, uMsg, wParam, lParam);
		pDockTool->UnDock();
		return lr;
	}
	
	switch (uMsg)
	{
	case WM_SIZE:
	case WM_MOVE:
		{
			lr = ::CallWindowProc(oldProc, hWnd, uMsg, wParam, lParam);
			pDockTool->MoveChild();
			return lr;
		}
		break;

	case WM_NCDESTROY: 
		{ 
			pDockTool->UnDock();
			pDockTool->CloseChild();
		}
		break;

	case WM_SHOWWINDOW:
		{

			if (pDockTool->GetChild() && IsWindow(pDockTool->GetChild()))
			{
				BOOL bShow = (BOOL)wParam;
				if (bShow != ::IsWindowVisible(pDockTool->GetChild()))
					::ShowWindow(pDockTool->GetChild(), bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
				if (bShow && lParam == SW_PARENTOPENING)
				{
					HWND hParent = ::GetParent(hWnd);
					if (hParent)
					{
						RECT rect;
						::GetWindowRect(pDockTool->GetChild(), &rect);
						POINT pt = {rect.left, rect.top};
						ScreenToClient(hParent, &pt);
						int w = rect.right - rect.left;
						int h = rect.bottom - rect.top;
						rect.left = pt.x;
						rect.top = pt.y;
						rect.right = rect.left + w;
						rect.bottom = rect.top + h;
						::RedrawWindow(hParent, &rect, NULL, RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
					}

				}

			}
			
		}
		break;
	}

	return ::CallWindowProc(oldProc, hWnd, uMsg, wParam, lParam);
}

void CChildDockTool::MoveChild(LPRECT lpRect)
{
	RECT rect;
	if (!lpRect)
	{
		RECT rcParent;
		::GetClientRect(m_hParent, &rcParent);
		rect = m_coordinate.rect;
		DPI_SCALE(&rect);
		GetAnchorPos(m_coordinate.anchor, &rcParent, &rect);
	}
	else
	{
		rect = *lpRect;
	}

	POINT pt = {rect.left, rect.top};
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	ClientToScreen(m_hParent, &pt);
	rect.left = pt.x;
	rect.top = pt.y;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	if (IsRectEmpty(&rect))
	{
		if (IsWindowVisible(GetChild()))
			ShowWindow(GetChild(), SW_HIDE);
		return;
	}
	

	if (m_pWindowChild && m_pWindowChild->IsLayerWindow())
	{
		::SetWindowPos(m_hChild, HWND_TOPMOST, pt.x, pt.y, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
		m_pWindowChild->ShowWindow();
	}
	else
	{
		::SetWindowPos(m_hChild, HWND_TOPMOST, pt.x, pt.y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOZORDER);
	}
}

void CChildDockTool::CloseChild()
{
	if (m_pWindowChild)
	{
		m_pWindowChild->CloseWindow();
	}
	else
	{
		if (::IsWindow(GetChild())) 
			::PostMessage(GetChild(), WM_CLOSE, (WPARAM)0, NULL);
	}

}