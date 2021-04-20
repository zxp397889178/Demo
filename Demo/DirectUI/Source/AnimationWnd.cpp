#include "stdafx.h"
#include "AnimationWnd.h"

CAnimationWnd::CAnimationWnd()
{
	m_iWindowWidth = 0;
	m_iWindowHeight = 0;
	m_dwStyle   = WS_POPUP;
	m_dwExStyle = WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW;
}

CAnimationWnd::~CAnimationWnd()
{

}

void CAnimationWnd::Create(HWND hParent, LPCTSTR lpWindowID,  int, int, int nWidth, int nHeight)
{
	CreateEx( NULL, _T("AnimationWnd"), m_dwStyle, m_dwExStyle, m_rcWindow.left, m_rcWindow.top, m_iWindowWidth, m_iWindowHeight);
	m_hPaintDC = ::GetDC( m_hWnd );

	m_hInstance = LoadLibrary(L"User32.DLL"); 
	if ( m_hInstance ) 
	{
		UpdateLayeredWindow=( UPDATELAYEREDWINDOW )GetProcAddress( m_hInstance, "UpdateLayeredWindow" );
	}
	else
	{
		exit(0);
	}
	m_BlendFun.BlendOp=0;
	m_BlendFun.BlendFlags=0;
	m_BlendFun.AlphaFormat=1;
	m_BlendFun.SourceConstantAlpha=200;
}

bool CAnimationWnd::ShowWindow(int nCmdShow)
{
	if ( !IsWindow(m_hWnd) )
	{
		return false;
	}
	return ::ShowWindow(m_hWnd, nCmdShow);
}

void CAnimationWnd::CloseWindow( )
{
	if ( !OnClosing() )
	{
		return;
	}
	::DestroyWindow(m_hWnd);
}

LRESULT CAnimationWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_TIMER:
		{
			OnTimer((UINT_PTR)wParam);
		}
		break;
	default:
		break;
	}

	return CWindowBase::WindowProc(message, wParam, lParam);
}

void CAnimationWnd::OnTimer(UINT_PTR nIDEvent)
{

}

UINT CAnimationWnd::SetTimer(UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
{
	return ::SetTimer( m_hWnd, nIDEvent, uElapse, NULL );
}

BOOL CAnimationWnd::KillTimer( UINT nIDEvent )
{
	return ::KillTimer( m_hWnd, nIDEvent );
}