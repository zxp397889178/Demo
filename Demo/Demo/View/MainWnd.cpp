#include "stdafx.h"
#include "View/MainWnd.h"


HINSTANCE CMainWnd::m_hInstance = NULL;
CMainWnd::CMainWnd() 
{
	m_hWnd		  = NULL;
}

CMainWnd::~CMainWnd()
{
   
}

LPCTSTR CMainWnd::GetWindowClass()
{
	return _T("Login_Main");
}

HRESULT CMainWnd::RegisterWnd(HINSTANCE hInstance)
{
	if (m_hInstance)
	{
		return true;
	}

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= _T("");
	wcex.lpszClassName	= GetWindowClass();
	wcex.hIconSm		= LoadIcon(wcex.hInstance, IDI_APPLICATION);

	bool bRes = RegisterClassEx(&wcex) != 0;
	if (bRes)
	{
		m_hInstance = hInstance;
	}

	return bRes;
}

HWND CMainWnd::CreateWnd()
{
	//create case window
	m_hWnd = CreateWindow(GetWindowClass(), _T(""), WS_OVERLAPPED, 0, 0, 0, 0, HWND_MESSAGE, NULL, m_hInstance, this);

	return m_hWnd;

}

LRESULT CALLBACK CMainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CMainWnd* pThis = NULL;
	if (uMsg == WM_NCCREATE && lParam != NULL) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CMainWnd*>(lpcs->lpCreateParams);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CMainWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (uMsg == WM_NCDESTROY) {
			pThis = NULL;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
		}
	}

	if (pThis)
	{
		pThis->ThisWndProc(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CMainWnd::ThisWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case Wm_User_EventBus:
		EventbusReceiveMessage(m_hWnd, uMsg, wParam, lParam);
		break;
	}

	return FALSE;
}

void CMainWnd::LoopMessage()
{
	MSG msg = { 0 };
	while (::IsWindow(m_hWnd)
		&& ::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (msg.message == WM_QUIT) break;
	}
}