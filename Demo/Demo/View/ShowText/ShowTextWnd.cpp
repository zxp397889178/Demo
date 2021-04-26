#include "stdafx.h"
#include "ShowTextWnd.h"

CShowTextWnd::CShowTextWnd()
{

}

CShowTextWnd::~CShowTextWnd()
{

}

HWND CShowTextWnd::CreateWnd(HWND hParent)
{
	SetLayerWindow(true);
	HWND hWnd = Create(hParent, _T("ShowTextWindow"));
	return hWnd;
}

bool CShowTextWnd::OnBtnClose(TNotifyUI* msg)
{
	CloseWindow();
	return true;
}
