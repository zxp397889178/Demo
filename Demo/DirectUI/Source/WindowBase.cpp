/***************************************************************************
  Copyright    : 2005, NetDragon Websoft Inc
  Program ID   :  NDSkinCtrl.cpp: implementation of the CBaseWnd class.
  Description  : 所有肤化控件的基类
  Version      : 91USkin 2.0
  Modification Log:
       DATE         AUTHOR          DESCRIPTION
 --------------------------------------------------------------------------
     2005-01-29     ZhouJS			create 
***************************************************************************/
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WindowBase.h"

LRESULT CALLBACK  SkinWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindowBase* pWnd = CWindowBase::FromHandle(hWnd);
	if (pWnd) return pWnd->WindowProc(uMsg, wParam, lParam);
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


typedef std::map<HWND,CWindowBase*> TmapWindowTable;
TmapWindowTable s_mapNdCtrls;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWindowBase::CWindowBase()
:m_hWnd(NULL)
, m_oldProc(NULL)
,m_bAutoDel(true)
,m_strWndClassName(_T("NDWindowBase"))
{
	m_dwExStyle = WS_EX_CONTROLPARENT;
	m_dwStyle   = WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX /*| WS_THICKFRAME*/;
}

CWindowBase::~CWindowBase()
{
	UnsubclassWindow();
}
HWND CWindowBase::GetHWND()
{
	return this == NULL ? NULL : m_hWnd;
}

CWindowBase* CWindowBase::FromHandle(HWND hWnd)
{
 	CWindowBase *p91UWnd = NULL;
	TmapWindowTable::iterator it = s_mapNdCtrls.find(hWnd);
	if (s_mapNdCtrls.end() != it)
	{
		p91UWnd = it->second;
	}
	return p91UWnd;
}

void CWindowBase::SetWndStyle(int index, DWORD dwStyle)
{
	if (index == GWL_STYLE)
		m_dwStyle = dwStyle;
	else if (index == GWL_EXSTYLE)
	{
		m_dwExStyle = dwStyle;
	}
	else
		return;
	if (::IsWindow(m_hWnd))
		::SetWindowLong(m_hWnd, index, dwStyle);
}

DWORD CWindowBase::GetWndStyle(int index)
{
	if (index == GWL_STYLE)
		return m_dwStyle;
	else
		return m_dwExStyle;
}

void CWindowBase::ModifyWndStyle(int index, DWORD dwRemove, DWORD dwAdd)
{
	if (::IsWindow(m_hWnd))
	{
		DWORD dwStyle = ::GetWindowLong(m_hWnd, index);
		DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
		if (dwStyle != dwNewStyle)
			::SetWindowLong(m_hWnd, index, dwNewStyle);
		if (index == GWL_STYLE)
			m_dwStyle = dwNewStyle;
		else if (index == GWL_EXSTYLE)
			m_dwExStyle = dwNewStyle;
	}
	else
	{
		if (index == GWL_STYLE)
			m_dwStyle = (m_dwStyle & ~dwRemove) | dwAdd;
		else if (index == GWL_EXSTYLE)
			m_dwExStyle = (m_dwExStyle & ~dwRemove) | dwAdd;
	}
}

	
BOOL CWindowBase::SubclassWindow(HWND hWnd) 
{ 
	if (hWnd == NULL)
		return FALSE;

	s_mapNdCtrls[hWnd] = this; 
	m_hWnd = hWnd;

	ModifyWndStyle(GWL_STYLE, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	::SetClassLong(m_hWnd, GCL_STYLE, CS_DBLCLKS | CS_SAVEBITS /*| CS_IME*/);

	m_oldProc = (WNDPROC) SetWindowLong(hWnd, GWL_WNDPROC, (LONG) SkinWndProc);
	return TRUE;
} 

void CWindowBase::UnsubclassWindow() 
{ 
	if (!m_hWnd)
		return;
	if (m_oldProc)
		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_oldProc); 
	s_mapNdCtrls.erase(m_hWnd);	
	m_hWnd = NULL;
} 

LRESULT CWindowBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (!m_hWnd)
		return S_OK;
	switch(message)
	{
	case WM_NCDESTROY: 
		{ 
			LRESULT lret; 
			WNDPROC wndproc; 
			wndproc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC); 
			if (wndproc == SkinWndProc) 
			{ 
				GetEngineObj()->UnSkinWindow(m_hWnd);
				lret = CallWindowProc(m_oldProc, 
					m_hWnd, 
					message, 
					wParam, 
					lParam); 
			} 
			else 
			{ 
				lret = CallWindowProc(m_oldProc, 
					m_hWnd, 
					message, 
					wParam, 
					lParam); 
			} 
			if (m_bAutoDel)
				delete this; 
			return lret;
		}  
	case WM_ENDSESSION:
		{
			return (HRESULT)1L;
		}
		break;
	}
	
	return	CallWindowProc(m_oldProc, m_hWnd
		, message, wParam, lParam); 
}


static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool CWindowBase::RegisterWindowClass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	UINT uBigIconId = GetEngineObj()->GetIconResource(true);
	UINT uSmallIconId = GetEngineObj()->GetIconResource(false);
	wc.hIcon = uBigIconId ? LoadIcon(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uBigIconId)) : NULL;
	wc.hIconSm = uSmallIconId ? LoadIcon(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uSmallIconId)) : NULL;

	wc.lpfnWndProc = __WndProc;
	wc.hInstance = GetEngineObj()->GetInstanceHandle();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND CWindowBase::Create(HWND hwndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	if (!RegisterWindowClass())
		return NULL;
	m_dwStyle = dwStyle;
	m_dwExStyle = dwExStyle;
	m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), lpszName, dwStyle, x, y, cx, cy, hwndParent, NULL, GetEngineObj()->GetInstanceHandle(), this);
	SubclassWindow(m_hWnd);
	return m_hWnd;

}

void CWindowBase::SetAutoDel(bool bAutoDel) //是否自动释放指针
{
	m_bAutoDel = bAutoDel;
}

bool CWindowBase::IsAutoDel()
{
	return m_bAutoDel;
}

LPCTSTR CWindowBase::GetWindowClassName()
{
	return m_strWndClassName.c_str();
}

void CWindowBase::SetWindowClassName(LPCTSTR lpszName)
{
	if ( NULL == lpszName)
		return;
	m_strWndClassName = lpszName;
}

bool CWindowBase::PreMessageHandler( const LPMSG pMsg, LRESULT& lRes )
{
	return false;
}

bool CWindowBase::Attach( HWND hWnd )
{
	if (!hWnd)
		return false;

	CWindowBase* pWindowBase = CWindowBase::FromHandle(hWnd);
	if (pWindowBase)
		return false;

	s_mapNdCtrls[hWnd] = this; 
	m_hWnd = hWnd;
	return TRUE;
}

void CWindowBase::Detach()
{
	if (!m_hWnd)
		return;
	s_mapNdCtrls.erase(m_hWnd);	
	m_hWnd = NULL;
}

void  CWindowBase::SetFocusOnWnd()
{
	::SetFocus(m_hWnd);
}