#include "stdafx.h"
#include "WindowPtrRef.h"
#include "UINewComboBoxEx.h"
#include "WindowPtrMap.h"
#include "WindowPtrMapImpl.h"


#define _Style_ComboBox_Bk_    0
#define _Style_ComboBox_Right_ 1

CListLayoutUI::CListLayoutUI()
{

}

CListLayoutUI::~CListLayoutUI()
{

}

void CListLayoutUI::Init()
{
	CScrollbarUI* pScrollbar = GetVScrollbar();
	if (pScrollbar)
	{
		pScrollbar->SetScrollPos(0);
		pScrollbar->SetCoordinate(20, 4, 6, 11, UIANCHOR_RTRB);
		pScrollbar->SetStyle(_T("ScrollBarEx"));
	}	
}

void CListLayoutUI::EnsureVisible(CControlUI* pControl)
{
	CScrollbarUI* pScrollbar = GetVScrollbar();
	if (pScrollbar && pScrollbar->IsVisible() && pControl)
	{		
		Resize();
		RECT rcItem = pControl->GetRect();
		RECT rcClient = GetClientRect();
		if (rcItem.bottom > rcClient.bottom)
			pScrollbar->SetScrollPos(pScrollbar->GetScrollPos() + rcItem.bottom - rcClient.bottom);
		else if (rcItem.top < rcClient.top)
			pScrollbar->SetScrollPos(pScrollbar->GetScrollPos() + rcItem.top - rcClient.top);	
	}
}

CNewComboBoxDropDownWnd::CNewComboBoxDropDownWnd(CNewComboBoxExUI* pOwner)
{
	SetAutoDel(false);
	m_pOwner = pOwner;

	if (!pOwner || !pOwner->GetWindow())
	{
		return;
	}

	SetLayerWindow(true);
	Create(pOwner->GetWindow()->GetHWND(), _T("NewComboBoxDropDownWnd"));
	pOwner->GetWindow()->AddMessageFilter(this);
}

CNewComboBoxDropDownWnd::~CNewComboBoxDropDownWnd()
{
	if (m_pOwner && m_pOwner->GetWindow())
	{
		m_pOwner->GetWindow()->RemoveMessageFilter(this);		
	}	
	
	m_pOwner->m_pToolWindow = NULL;
}

bool CNewComboBoxDropDownWnd::OnClickItem(TNotifyUI* pNotify)
{
	CListLayoutUI* pList = dynamic_cast<CListLayoutUI*>(GetItem(_T("list")));
	if (pList)
	{
		CControlUI *pItem = pNotify->pSender;
		if (pItem)
		{
 			m_pOwner->SelectItem(pItem->GetIndex());
 			m_pOwner->Hide();
		}
	}

	return true;
}

LRESULT CNewComboBoxDropDownWnd::MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		if (IsWindowVisible(GetHWND()))
		{
			POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
			if (!::PtInRect(m_pOwner->GetRectPtr(), pt))
			{
				m_pOwner->Hide();
			}
		}	
		
	}
	break;
	}

	return S_OK;
}

LRESULT CNewComboBoxDropDownWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KILLFOCUS:
		{
			HWND hWnd = GetActiveWindow();
			CWindowBase* pWindow = CWindowBase::FromHandle(hWnd);
			if (pWindow && (pWindow == this))
			{
				return S_OK;
			}
			else
			{
				m_pOwner->Hide();
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			CListLayoutUI* pList = dynamic_cast<CListLayoutUI*>(GetItem(_T("list")));
			if (pList && IsShow())
			{
				TEventUI event = { 0 };
				event.nType = UIEVENT_SCROLLWHEEL;
				event.wParam = wParam;
				event.lParam = lParam;
				CScrollbarUI *pVScrollBar = pList->GetVScrollbar();
				if (pVScrollBar)
					pVScrollBar->SendEvent(event);
			}
		}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

bool CNewComboBoxDropDownWnd::IsShow()
{
	return !!::IsWindowVisible(GetHWND());
}

///////////////////////////////////////
CNewComboBoxExUI::CNewComboBoxExUI()
	:m_pList(NULL)
	, m_pToolWindow(NULL)
	, m_nItemsEnsureVisible(4)
	, m_nItemsHeight(0)
	, m_bUseSelectItemTag(false)
{
	UI_OBJECT_REGISTER(CListLayoutUI);

	SetStyle(_T("ComboBox"));
	::SetRect(&m_rcPadding, 0, 0, 0, 0);

}

CNewComboBoxExUI::~CNewComboBoxExUI()
{
	if (m_pToolWindow)
	{
		m_pToolWindow->CloseWindow();
		delete m_pToolWindow;
	}
	m_pToolWindow = NULL;
	m_pList = NULL;	
}


CListLayoutUI* CNewComboBoxExUI::GetList()
{
	if (m_pList == NULL)
	{
		m_pToolWindow = new_nothrow CNewComboBoxDropDownWnd(this);
		m_pList = dynamic_cast<CListLayoutUI*>(m_pToolWindow->GetItem(_T("list")));
	}
	return m_pList;
}

bool CNewComboBoxExUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_KILLFOCUS:
		{
			if (IsShow())
			{
				if (m_pToolWindow
					&& GetActiveWindow() != m_pToolWindow->GetHWND())
				{
					Hide();
				}
			}		
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{	
			if (PtInRect(GetRectPtr(), event.ptMouse))
			{			
				if (IsShow())
				{
					Hide();				
				}			
				else
				{		
					Show();				
				}		
			}
		}
		break;
	case UIEVENT_SCROLLWHEEL:
		{
			if (IsShow() && m_pToolWindow)
			{
				::SendMessage(m_pToolWindow->GetHWND(), WM_MOUSEWHEEL, event.wParam, event.lParam);
			}			
		}
		break;	
	default:
		break;
	}
	return __super::Event(event);
}

bool CNewComboBoxExUI::Add(CControlUI* pControl)
{
	if (GetList())
	{
		pControl->SetId(_T("NewComboBoxListItem")); //用来接收消息
		return m_pList->Add(pControl);
	}
	return false;
}

void CNewComboBoxExUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("ItemsEnsureVisible")))
	{
		m_nItemsEnsureVisible = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("WndPadding")))
	{
		tstring spliter = lpszValue;
		short pos[4] = { 2, 2, 2, 2 };
		int i = spliter.find_first_of(_T(','));
		unsigned char index = 0;
		while (i >= 0)
		{
			pos[index] = _ttoi(spliter.substr(0, i).c_str());
			spliter = spliter.substr(i + 1);
			i = spliter.find_first_of(_T(','));
			index++;
		}
		pos[index] = _ttoi(spliter.c_str());
		m_rcPadding.left = pos[0];
		m_rcPadding.top = pos[1];
		m_rcPadding.right = pos[2];
		m_rcPadding.bottom = pos[3];
	}
	else if (equal_icmp(lpszName, _T("ItemsHeight")))
	{
		m_nItemsHeight = _ttoi(lpszValue);
	}
	if (equal_icmp(lpszName, _T("UseTagText")))
	{
		m_bUseSelectItemTag = (bool)!!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName,lpszValue);
	}
}

CControlUI* CNewComboBoxExUI::GetItem(int index)
{
	if (GetList() == NULL)
		return NULL;
	return m_pList->GetItem(index);
}


void CNewComboBoxExUI::SelectItem(CControlUI* pControl)
{
	if (GetList() == NULL)
		return;
	if (pControl)
	{	
		bool bFlag = false;
		int nCnt = GetCount();
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pCtr = GetItem(i);
			if (pCtr && pCtr == pControl)
			{
				SelectItem(i);
				bFlag = true;
				break;
			}
		}
		if (!bFlag)
			SelectItem(-1);
	}
	else
	{
		SelectItem(-1);
	}
}

void CNewComboBoxExUI::SelectItem(int index)
{
	if (GetList() == NULL)
		return;
	CControlUI* pControl = m_pList->GetItem(index);
	if (pControl)
	{
		LPCTSTR lpszText = m_bUseSelectItemTag ? pControl->GetTag() : pControl->GetText();
		
		SetAttribute(_T("text"), lpszText);
		SetAttribute(_T("ToolTip"), lpszText);

		SendNotify(UINOTIFY_SELCHANGE, NULL, (LPARAM)pControl);
	}	
	
	this->Invalidate();	
}

void CNewComboBoxExUI::SetText(LPCTSTR lpszText)
{
	__super::SetText(lpszText);
	SetToolTip(lpszText);
}

bool CNewComboBoxExUI::RemoveAll()
{
	if (GetList() == NULL)
		return false;
	if (m_pList)
		m_pList->RemoveAll();
	return true;
}

bool CNewComboBoxExUI::Remove(CControlUI* pControl)
{
	if (GetList() == NULL)
		return false;
	if (m_pList)
		return m_pList->Remove(pControl);
	return false;
}

int CNewComboBoxExUI::GetCount()
{
	if (GetList() == NULL)
		return 0;
	return m_pList->GetCount();
}

void CNewComboBoxExUI::Show()
{
	if (NULL == GetList())
		return;
	if (NULL == m_pToolWindow)
		return;
	
	ShowDrop();
}

void CNewComboBoxExUI::ShowDrop()
{
	int nMinItems = m_pList->GetCount();
	nMinItems = nMinItems > m_nItemsEnsureVisible ? m_nItemsEnsureVisible : nMinItems;
	if (nMinItems)
	{
		RECT rect = GetRect();
		POINT pt = { rect.left, rect.bottom };
		::ClientToScreen(GetWindow()->GetHWND(), &pt);

		RECT rcDesktop;
		HMONITOR hMonitor = ::MonitorFromWindow(GetWindow()->GetHWND(), MONITOR_DEFAULTTONEAREST);
		if (hMonitor)
		{
			MONITORINFOEX monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);
			rcDesktop = monitorInfo.rcWork;
		}
		else
		{
			::GetWindowRect(::GetDesktopWindow(), &rcDesktop);
		}

		RECT rt = m_pList->GetInset();

		int iHeight;
		if (m_nItemsHeight > 0)
		{
			iHeight = nMinItems * m_nItemsHeight + 4;
		}
		else
		{
			iHeight = nMinItems * 28 + 4;
		}

		iHeight = rt.top + rt.bottom + iHeight;
		iHeight = DPI_SCALE(iHeight);
		if (pt.y + iHeight > rcDesktop.bottom)
		{
			pt.x = rect.left;
			pt.y = rect.top;
			::ClientToScreen(GetWindow()->GetHWND(), &pt);
			m_pToolWindow->SetWindowPos(HWND_DESKTOP, pt.x - m_rcPadding.left, pt.y - iHeight - m_rcPadding.top, rect.right - rect.left + m_rcPadding.right, iHeight + m_rcPadding.bottom, SWP_NOACTIVATE);
		}
		else
		{
			m_pToolWindow->SetWindowPos(HWND_DESKTOP, pt.x - m_rcPadding.left, pt.y - m_rcPadding.top, rect.right - rect.left + m_rcPadding.right, iHeight + m_rcPadding.bottom, SWP_NOACTIVATE);
		}	
		
		
		m_pToolWindow->ShowWindow(SW_SHOWNOACTIVATE);
		m_pToolWindow->GetRoot()->OnlyResizeChild();
		m_pToolWindow->GetRoot()->Invalidate();
	}
}

void CNewComboBoxExUI::Hide()
{
	if (m_pToolWindow)
	{		
		m_pToolWindow->ShowWindow(SW_HIDE);		
		
	}
}

bool CNewComboBoxExUI::IsShow()
{
	if (m_pToolWindow)
		return !!::IsWindowVisible(m_pToolWindow->GetHWND());
	return false;
}


void CNewComboBoxExUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	Hide();
}



bool CNewComboBoxExUI::IsKillFocus()
{
	return !IsShow();
}

void CNewComboBoxExUI::OnLanguageChange()
{
	__super::OnLanguageChange();
	if (m_pList) ((CControlUI*)m_pList)->OnLanguageChange();

}



