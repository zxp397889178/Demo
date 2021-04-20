#include "stdafx.h"
#include "UIDwmWindow.h"


static void SetWindowRgn(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int nWidth = rect.right - rect.left;        
	int nHeight = rect.bottom - rect.top;   
	HRGN hRgn = CreateRectRgn(0, 0, nWidth, nHeight);
	if (hRgn)
	{
		SetWindowRgn(hwnd, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
}


static bool CheckDwmIsCompositionEnabled()
{
	BOOL isCompositionEnabled = false;
	typedef HRESULT(WINAPI* LPDwmIsCompositionEnabled)(_Out_ BOOL *pfEnabled);

	HINSTANCE hLibaray = LoadLibrary(L"dwmapi.dll");
	if (hLibaray)
	{
		LPDwmIsCompositionEnabled pDwmIsCompositionEnabled = (LPDwmIsCompositionEnabled)GetProcAddress(hLibaray, "DwmIsCompositionEnabled");
		if (pDwmIsCompositionEnabled)
		{
			pDwmIsCompositionEnabled(&isCompositionEnabled);
		}
		FreeLibrary(hLibaray);
	}
	
	return !!isCompositionEnabled;
}

static bool SetDwmWindow(HWND hWnd)
{
	if (!IsWindow(hWnd))
		return false;

	typedef struct _MARGINS
	{
		int cxLeftWidth;     
		int cxRightWidth;    
		int cyTopHeight;     
		int cyBottomHeight;  
	} MARGINS, *PMARGINS;
	typedef HRESULT(__stdcall *LPDwmExtendFrameIntoClientArea)(HWND hWnd, _In_ const MARGINS* pMarInset);
	
	SetWindowRgn(hWnd);
	
	HINSTANCE hLibaray = LoadLibrary(L"dwmapi.dll");
	if (hLibaray)
	{
		LPDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = (LPDwmExtendFrameIntoClientArea)GetProcAddress(hLibaray, "DwmExtendFrameIntoClientArea");
		if (pDwmExtendFrameIntoClientArea)
		{
			MARGINS margins = { -1 };
			HRESULT hr = pDwmExtendFrameIntoClientArea(hWnd, &margins);
			if (hr == S_OK)
			{
				FreeLibrary(hLibaray);
				return true;
			}
		}
		FreeLibrary(hLibaray);
	}
	return false;
}


void CDwmWindowUI::OnCreate()
{
	if (CheckDwmIsCompositionEnabled() && SetDwmWindow(GetHWND()))
	{
		SetLayerWindow(false);
	}
	else
	{
		SetLayerWindow(true);
	}
}

#define WM_DWMCOMPOSITIONCHANGED        0x031E
LRESULT CDwmWindowUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DWMCOMPOSITIONCHANGED)
	{
		if (CheckDwmIsCompositionEnabled() && SetDwmWindow(GetHWND()))
		{
			SetLayerWindow(false);	
		}
		else
		{
			SetLayerWindow(true);
		}
	}
	return  __super::WindowProc(message, wParam, lParam);
}

void CDwmWindowUI::OnSetWindowRgn()
{
	SetWindowRgn(GetHWND());
}


void CDwmWindowUI::OnDrawBefore(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint)
{
	m_eDefaultFontType = GetUIRes()->GetDefaultFontType();
	GetUIRes()->SetDefaultFontType(UIFONT_GDIPLUS);
}
void CDwmWindowUI::OnDrawAfter(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint)
{
	GetUIRes()->SetDefaultFontType(m_eDefaultFontType);
}



