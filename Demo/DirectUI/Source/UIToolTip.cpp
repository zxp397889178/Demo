/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIToolTip.h"

RECT OffsetToolTip(const RECT& rcSrc, const RECT& rcOffset, POINT pt)
{
	RECT rcDest = rcSrc;
	if (rcOffset.left == 1 || rcOffset.right == 1)
	{
		if (rcOffset.right == 1)
			OffsetRect(&rcDest, -rcDest.right, 0);

		if (rcOffset.top)
			OffsetRect(&rcDest, 0, -rcOffset.top);
		else
			OffsetRect(&rcDest, 0, rcOffset.bottom - (rcSrc.bottom - rcSrc.top));
		OffsetRect(&rcDest, pt.x, pt.y);
	}
	if (rcOffset.top == 1 || rcOffset.bottom == 1)
	{
		if (rcOffset.bottom == 1)
			OffsetRect(&rcDest, 0, -rcDest.bottom);

		if (rcOffset.left)
			OffsetRect(&rcDest, -rcOffset.left, 0);
		else
			OffsetRect(&rcDest, rcOffset.right - (rcSrc.right - rcSrc.left), 0);
		OffsetRect(&rcDest, pt.x, pt.y);
	}
	return rcDest;
}

CToolTipUI::CToolTipUI(LPCTSTR lpszContent, UITYPE_TOOLTIP type/* = enToolTipTypeInfo*/, HWND hParentWnd/* = HWND_DESKTOP*/)
{
	m_bLockTimer = false;
	m_uIconType = type;
	m_nMaxWidth = 100;
	m_nElapse = 3000;
	m_rcOffset.left = m_rcOffset.right = m_rcOffset.bottom = m_rcOffset.top = 0;
	SetWndStyle(GWL_EXSTYLE, WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
	m_ptPointTo.x = m_ptPointTo.y = 0;
	m_pCallBack = NULL;
	m_pCallBackWParam = NULL;
	m_pCallBackLParam = NULL;
	SetLayerWindow(true);
	SetWindowClassName(_T("CToolTipUI"));
	this->Create(hParentWnd, _T("tooltipdlg"));
	SetAlign(_T("BL"));
	CControlUI* pControl = GetItem(_T("content"));
	if (pControl)
		pControl->SetText(lpszContent);
	m_objTimer += MakeDelegate(this, &CToolTipUI::OnShowTimer);
}

CToolTipUI::~CToolTipUI()
{
	m_objTimer.Stop();
}

void CToolTipUI::OnShowTimer(CUITimerBase* pTimer)
{
	if (!m_bLockTimer)
	{
		if (m_pCallBack && m_pCallBack(this, m_pCallBackWParam, m_pCallBackLParam) != S_OK)
			return;
		if (IsWindow(GetHWND()))
		{
			m_objTimer.Stop();
			this->CloseWindow();
		}
	}
}

void CToolTipUI::SetPointTo(POINT pt)
{
	m_ptPointTo = pt;
}

void CToolTipUI::SetAlign(LPCTSTR lpszAlign)
{
	ChangePosFromStr(lpszAlign);
	SetBkImage(GetModeValueFromStr(lpszAlign).c_str());
}

void CToolTipUI::SetWidth(int nWidth)
{
	m_nMaxWidth = nWidth;
}

void CToolTipUI::SetElapse(int nElapse)
{
	m_nElapse = nElapse;
}

void CToolTipUI::SetContent(LPCTSTR lpszContent)
{
	CControlUI* pControl = GetItem(_T("content"));
	if (pControl)
		pControl->SetText(lpszContent);
}

void CToolTipUI::ShowTootipWindow(int nCmdShow/* = SW_SHOWNOACTIVATE*/)
{
	tstring strText = _T("");
	CControlUI* pControl = GetItem(_T("content"));
	if (pControl)
	{
		strText = pControl->GetText();
		m_objTimer.SetTimerParam(m_nElapse);
		m_objTimer.Start();
	}
	RECT rcCalc = {0, 0, m_nMaxWidth, 0};
	GetUIRes()->GetDefaultFont()->CalcText(this->GetPaintDC(), rcCalc, strText.c_str(), DT_CALC_MULTILINE);
	rcCalc.right += DPI_SCALE(50);
	if (rcCalc.bottom < DPI_SCALE(20))
		rcCalc.bottom = DPI_SCALE(40);
	else
		rcCalc.bottom += DPI_SCALE(20);

	CControlUI* pIcon = GetItem(_T("infoicon"));
	if (pIcon)
		pIcon->SetState((UITYPE_STATE)m_uIconType);

	RECT rcDest = OffsetToolTip(rcCalc, m_rcOffset, m_ptPointTo);
	this->SetWindowPos(HWND_TOPMOST, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, SWP_NOACTIVATE);
	__super::ShowWindow(nCmdShow);
}

tstring  CToolTipUI::GetModeValueFromStr(LPCTSTR lpszValue)
{
	m_rcOffset.left = m_rcOffset.right = m_rcOffset.bottom = m_rcOffset.top = 0;
	if (equal_icmp(lpszValue, _T("BL")))
	{
		m_rcOffset.left = 37;
		m_rcOffset.bottom = 1;
		return _T("#tooltipbkgBL");
	}
	if (equal_icmp(lpszValue, _T("BR")))
	{
		m_rcOffset.right = 37;
		m_rcOffset.bottom = 1;
		return _T("#tooltipbkgBR");
	}
	if (equal_icmp(lpszValue, _T("LB")))
	{
		m_rcOffset.bottom = 16;
		m_rcOffset.left = 1;
		return _T("#tooltipbkgLB");
	}
	if (equal_icmp(lpszValue, _T("LT")))
	{
		m_rcOffset.top = 16;
		m_rcOffset.left = 1;
		return _T("#tooltipbkgLT");
	}
	if (equal_icmp(lpszValue, _T("RB")))
	{
		m_rcOffset.bottom = 16;
		m_rcOffset.right = 1;
		return _T("#tooltipbkgRB");
	}
	if (equal_icmp(lpszValue, _T("RT")))
	{
		m_rcOffset.top = 16;
		m_rcOffset.right = 1;
		return _T("#tooltipbkgRT");
	}
	if (equal_icmp(lpszValue, _T("TL")))
	{
		m_rcOffset.left = 37;
		m_rcOffset.top = 1;
		return _T("#tooltipbkgTL");
	}
	if (equal_icmp(lpszValue, _T("TR")))
	{
		m_rcOffset.right = 37;
		m_rcOffset.top = 1;
		return _T("#tooltipbkgTR");
	}
	m_rcOffset.left = 37;
	m_rcOffset.bottom = 1;
	return _T("#tooltipbkgBL");
}

void CToolTipUI::ChangePosFromStr(LPCTSTR lpszValue)
{
	CControlUI* pIcon = GetItem(_T("infoicon"));
	CControlUI* pContent = GetItem(_T("content"));
	if (pIcon == NULL || pContent == NULL)
		return;
	if (equal_icmp(lpszValue, _T("BL")))
	{
		pIcon->SetCoordinate(8,8,28,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,8,8,12,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("BR")))
	{
		pIcon->SetCoordinate(8,8,28,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,8,8,12,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("LB")))
	{
		pIcon->SetCoordinate(12,8,32,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(36,8,8,8,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("LT")))
	{
		pIcon->SetCoordinate(12,8,32,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(36,8,8,8,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("RB")))
	{
		pIcon->SetCoordinate(8,8,28,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,8,8,8,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("RT")))
	{
		pIcon->SetCoordinate(8,8,28,28,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,8,8,8,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("TL")))
	{
		pIcon->SetCoordinate(8,12,28,32,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,12,8,8,UIANCHOR_LTRB);
		return;
	}
	if (equal_icmp(lpszValue, _T("TR")))
	{
		pIcon->SetCoordinate(8,12,28,32,UIANCHOR_LEFTTOP);
		pContent->SetCoordinate(32,12,8,8,UIANCHOR_LTRB);
		return;
	}
	pIcon->SetCoordinate(8,8,28,28,UIANCHOR_LEFTTOP);
	pContent->SetCoordinate(32,8,8,12,UIANCHOR_LTRB);
}

void CToolTipUI::SetLockTimer(bool bLock)
{
	m_bLockTimer = bLock;
}

void CToolTipUI::SetCallBack(ToolTip_Callback* pCallBack, WPARAM wParam, LPARAM lParam)
{
	m_pCallBack = pCallBack;
	m_pCallBackWParam = wParam;
	m_pCallBackLParam = lParam;
}

//////////////////////////////////////////////////////////////////////////

class TipWindowImpl
{
public:
	TOOLINFO m_ToolTip;
};
//////////////////////////////////////////////////////////////////////////
LRESULT TipWindow::Send( UINT Msg, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL */ )
{
	if (GetHWND())
		return ::SendMessage(m_hWnd, Msg, wParam, lParam);
	return S_FALSE;
}

void TipWindow::ShowTip( bool bShow, LPCTSTR lpszText /*= _T("")*/, LPRECT pRect /*= NULL*/ )
{
	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)GetUIRes()->GetDefaultFont()->GetFont(), NULL);
	if (pRect)
	{
		m_pTipWindowImpl->m_ToolTip.rect.left   = pRect->left;
		m_pTipWindowImpl->m_ToolTip.rect.top    = pRect->top;
		m_pTipWindowImpl->m_ToolTip.rect.right  = pRect->right;
		m_pTipWindowImpl->m_ToolTip.rect.bottom = pRect->bottom;
	}
	if (lpszText)
		m_pTipWindowImpl->m_ToolTip.lpszText = (LPWSTR)lpszText;
	if (lpszText || pRect)
		Send(TTM_SETTOOLINFO, 0, (LPARAM)&(m_pTipWindowImpl->m_ToolTip));
	Send(TTM_TRACKACTIVATE, bShow?TRUE:FALSE, (LPARAM)&(m_pTipWindowImpl->m_ToolTip));
}

bool TipWindow::CreateTip( HWND hParentWnd, int nMaxWidth /*= 260*/ )
{
	DWORD dwExStyle = 0;
	if (GetUIEngine()->IsRTLLayout())
	{
		dwExStyle |= WS_EX_LAYOUTRTL;
	}

	SetWndStyle(GWL_STYLE, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);
	SetWndStyle(GWL_EXSTYLE, dwExStyle);
	m_hWnd = ::CreateWindowEx(GetWndStyle(GWL_EXSTYLE), TOOLTIPS_CLASS, NULL, GetWndStyle(GWL_STYLE), CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hParentWnd, NULL, NULL, NULL);
	
	SubclassWindow(m_hWnd);
	if (m_hWnd)
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	if (hParentWnd)
	{
		m_pTipWindowImpl->m_ToolTip.uFlags = TTF_IDISHWND;
		m_pTipWindowImpl->m_ToolTip.hwnd = hParentWnd;
		m_pTipWindowImpl->m_ToolTip.uId = (UINT)hParentWnd;
	}
	else
	{
		m_pTipWindowImpl->m_ToolTip.uFlags = TTF_TRACK | TTF_TRANSPARENT;
		m_pTipWindowImpl->m_ToolTip.hwnd = NULL;
		m_pTipWindowImpl->m_ToolTip.uId = NULL;
	}

	Send(TTM_ADDTOOL, 0, (LPARAM) &(m_pTipWindowImpl->m_ToolTip));
	Send(TTM_SETMAXTIPWIDTH, 0, nMaxWidth);//tooltip默认最宽
	Send(TTM_SETTIPBKCOLOR, 0x00FFFFFF, 0);
	Send(TTM_SETTIPTEXTCOLOR, 0x00575757, 0);

	return true;
}

TipWindow::~TipWindow()
{
	if (m_pTipWindowImpl)
		delete m_pTipWindowImpl;
	m_pTipWindowImpl = NULL;
}

TipWindow::TipWindow()
{
	m_pTipWindowImpl = new TipWindowImpl;
	m_hWnd = NULL;
	::ZeroMemory(&(m_pTipWindowImpl->m_ToolTip), sizeof(TOOLINFO));
	m_pTipWindowImpl->m_ToolTip.cbSize = sizeof(TOOLINFO)- sizeof(void *);
	m_pTipWindowImpl->m_ToolTip.hinst = NULL;
}

LRESULT TipWindow::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case WM_CLOSE:
		Send(TTM_TRACKACTIVATE, FALSE, (LPARAM)&(m_pTipWindowImpl->m_ToolTip));
		break;
	case WM_WINDOWPOSCHANGING:
	{
		if (!GetUIEngine()->IsRTLLayout())
		{
			break;
		}

		LPWINDOWPOS pWp = (LPWINDOWPOS)lParam;
		if (pWp->flags & SWP_NOMOVE
			|| pWp->flags & SWP_NOREDRAW)
		{
			break;
		}

		RECT rt;
		GetWindowRect(GetHWND(), &rt);
		pWp->x -= rt.right - rt.left;
	}
	break;

	}
	return __super::WindowProc(message, wParam, lParam);
}
