#include "stdafx.h"
#include "WebPreviewBiz.h"

#include "WindowPtrRef.h"
#include "UIWebPreviewInterface.h"

CWebPreviewBiz::CWebPreviewBiz()
{
	m_pWindow = NULL;
}

CWebPreviewBiz::~CWebPreviewBiz()
{

}

void CWebPreviewBiz::OnCreateBefore(WebPreviewInfo* pInfo)
{

}

void CWebPreviewBiz::ShowCefWnd(const tstring & strUrl)
{
	if (m_pWindow)
	{
		m_pWindow->ShowCefUrl(strUrl, 0, 0);
	}
}

void CWebPreviewBiz::InitWebWindow(CWebPreviewInterface* pWindow)
{
	m_pWindow = pWindow;
}

void CWebPreviewBiz::UpdateWindowPos(CWindowUI* pWindow, HWND hParent, float fWidthPercent, float fHeightPercent, float fConstraintSspectRatio)
{
	if (!hParent
		|| !pWindow
		|| !pWindow->GetHWND()
		|| !::IsWindow(hParent)
		|| !::IsWindow(pWindow->GetHWND()))
	{
		return;
	}

	RECT rt = { 0 };
	if (::IsIconic(hParent))
	{
		MONITORINFO oMonitor = { 0 };
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hParent, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rt = oMonitor.rcMonitor;
	}
	else
	{
		::GetWindowRect(hParent, &rt);
	}

	if (fWidthPercent > 0.0001 && fHeightPercent > 0.0001)
	{
		int nWndWidth = (rt.right - rt.left);
		int nWndHeight = rt.bottom - rt.top;
		int nPosX = (int)(rt.left + nWndWidth * (1 - fWidthPercent) * 0.5);
		int nPosY = (int)(rt.top + nWndHeight * (1 - fHeightPercent) * 0.5);
		nWndHeight = (int)(nWndHeight * fHeightPercent);
		nWndWidth = (int)(nWndWidth * fWidthPercent);

		::SetWindowPos(pWindow->GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
	}
	else if (fWidthPercent > 0.0001)
	{
		//使用 宽约束高
		int nWndWidth = (rt.right - rt.left);
		int nPosX = (int)(rt.left + nWndWidth * (1 - fWidthPercent) * 0.5);
		nWndWidth = (int)(nWndWidth * fWidthPercent);
		int nWndHeight = (int)(nWndWidth * fConstraintSspectRatio);
		int nPosY = (int)(rt.top + (rt.bottom - rt.top - nWndHeight) * 0.5);

		::SetWindowPos(pWindow->GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
	}
	else if (fHeightPercent > 0.0001)
	{
		//使用 高约束宽
		int nWndHeight = rt.bottom - rt.top;
		int nPosY = (int)(rt.top + nWndHeight * (1 - fHeightPercent) * 0.5);
		nWndHeight = (int)(nWndHeight * fHeightPercent);
		int nWndWidth = (int)(nWndHeight * fConstraintSspectRatio);
		if (nWndWidth >= rt.right - rt.left)
		{
			nWndWidth = (rt.right - rt.left) * 0.9;
		}
		int nPosX = (int)(rt.left + (rt.right - rt.left - nWndWidth) * 0.5);

		::SetWindowPos(pWindow->GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
	}

}

void CWebPreviewBiz::SetReceiveStatusChange(bool bReceive)
{
	if (m_pWindow)
	{
		m_pWindow->SetReceiveStatusChange(bReceive);
	}
}

void CWebPreviewBiz::RefreshCefWnd()
{
	if (m_pWindow)
	{
		m_pWindow->ReloadCefWnd();
	}
}

void CWebPreviewBiz::ForwardCefWnd()
{
	if (m_pWindow)
	{
		m_pWindow->ForwardCefWnd();
	}
}

void CWebPreviewBiz::BackOffCefWnd()
{
	if (m_pWindow)
	{
		m_pWindow->BackOffCefWnd();
	}
}

void CWebPreviewBiz::DestroyWebWindow()
{
	m_pWindow = NULL;	
}

int CWebPreviewBiz::GetLinkPopupType()
{
	return -1;
}

void CWebPreviewBiz::ReloadCefUrl(LPCTSTR lptcsUrl)
{
	if (m_pWindow)
	{
		m_pWindow->ReloadCefUrl(lptcsUrl);
	}
}

void CWebPreviewBiz::OnCefInvokeMethodPpt(LPCSTR pszJsonInfo)
{

}

void CWebPreviewBiz::OnCefInvokeAsync(LPCSTR pszJsonInfo)
{

}

void CWebPreviewBiz::OnCefInvoke(LPCSTR pszJsonInfo)
{

}

void CWebPreviewBiz::OnCefUrlPopUp(LPCTSTR lptcsUrl)
{

}

void CWebPreviewBiz::OnCefUrlChange(LPCTSTR lptcsUrl)
{

}

bool CWebPreviewBiz::isRetryErrorUrl(bool bLocal /*= false*/)
{
	return bLocal;
}

wstring CWebPreviewBiz::OnCefRetryErrorUrl()
{
	return _T("");
}

void CWebPreviewBiz::OnCefLoaded()
{

}

void CWebPreviewBiz::SetErrorLanguage(bool bLocal)
{
	if (m_pWindow)
	{
		m_pWindow->SetErrorLanguage(bLocal);
	}
}

void CWebPreviewBiz::MiniWnd()
{

}

