
#include "stdafx.h"
#include "UIComboBox.h"


#define _Style_ComboBox_Bk_ 0
#define _Style_ComboBox_Right_ 1

class CComboBoxDropDownWnd : public CNoActiveWindowUI,public IMessageFilterUI
{
public:
	static HWND ms_hShowInstance;//记录当前下拉窗口指针
public:
	CComboBoxDropDownWnd(CComboBoxUI* pOwner);
	virtual ~CComboBoxDropDownWnd();

	UIBEGIN_MSG_MAP
		if (!m_bLockNotify)
		{
			UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("list"), OnClickItem)
			UI_EVENT_ID_HANDLER(UINOTIFY_RCLICK, _T("list"), OnClickItem)
			UI_EVENT_ID_HANDLER(UINOTIFY_TVN_CHECK, _T("list"), OnCheckItem)
			UI_EVENT_HANDLER(UINOTIFY_EN_KEYDOWN, OnKeyDown);
		}
	UIEND_MSG_MAP

public:
	bool  OnKeyDown(TNotifyUI* pNotify);
	bool  OnCheckItem(TNotifyUI* pNotify);
	bool  OnClickItem(TNotifyUI* pNotify);
	bool  OnDbClickItem(TNotifyUI* pNotify);
	bool  IsShow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void SetLockNotify(bool bLock);//在窗口没显示时不接受事件
protected:
	virtual LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
private:
	CComboBoxUI* m_pOwner;
	bool m_bLockNotify;
};
//////////////////////////////////////////////////////////////////////////

HWND CComboBoxDropDownWnd::ms_hShowInstance = NULL;
CComboBoxDropDownWnd::CComboBoxDropDownWnd(CComboBoxUI* pOwner)
{
	SetAutoDel(false);
	m_bLockNotify = true;
	m_pOwner = pOwner;
	if (!pOwner || !pOwner->GetWindow())
	{
		return;
	}

	//if (pOwner->GetWndStyle() == _T("listboxEx"))
	//{
	//	this->SetLayerWindow(true);
	//	Create(pOwner->GetWindow()->GetHWND(), _T("listboxEx"));
	//}
	if (!pOwner->GetWndStyle().empty())
	{
		this->SetLayerWindow(true);
		Create(pOwner->GetWindow()->GetHWND(), pOwner->GetWndStyle().c_str());
	}
	else
	{
		Create(pOwner->GetWindow()->GetHWND(), _T("listbox"));
	}
	pOwner->GetWindow()->AddMessageFilter(this);
	
}

CComboBoxDropDownWnd::~CComboBoxDropDownWnd()
{
	if (m_pOwner && m_pOwner->GetWindow())
		m_pOwner->GetWindow()->RemoveMessageFilter(this);
	m_pOwner->m_pToolWindow = NULL;
}

bool  CComboBoxDropDownWnd::OnCheckItem(TNotifyUI* pNotify)
{
	if (m_pOwner)
		m_pOwner->SendNotify( UINOTIFY_CBN_CHECK, pNotify->wParam, pNotify->lParam);
	return true;
}

bool  CComboBoxDropDownWnd::OnClickItem(TNotifyUI* pNotify)
{
	CControlUI* pListControl = GetItem(_T("list"));
	CListUI* pList = static_cast<CListUI*>(pListControl);
	if (pList)
	{
		CControlUI *pItem = pList->GetCurSel();
		if (pItem)
		{
			m_pOwner->SelectItem(pItem->GetIndex());
			if (!m_pOwner->IsDrop())
				m_pOwner->SelAllText();
			m_pOwner->Hide();
		}
	}
	return true;
}

bool  CComboBoxDropDownWnd::OnKeyDown(TNotifyUI* pNotify)
{
	CControlUI *pCurSelItem = NULL;

	CControlUI* pListControl = GetItem(_T("list"));
	CListUI* pList = static_cast<CListUI*>(pListControl);
	if (pList)
	{
		int nCount = pList->GetCount();
		if(nCount != 0)
		{
			if(pNotify->wParam != VK_RETURN)
			{
				pCurSelItem = pList->GetCurSel();
				if (pCurSelItem)
				{
					int i = pCurSelItem->GetIndex();
					if (pNotify->wParam == VK_DOWN)
					{
						if(++i == nCount)
							i = 0;
					}
					else if (pNotify->wParam == VK_UP)
					{
						if(--i < 0)
							i = nCount - 1;
					}
					pList->SelectItem(pList->GetItem(i));
					pList->EnsureVisible(pList->GetCurSel());
				}
				else
				{
					pList->SelectItem(pList->GetItem(0));
					pList->EnsureVisible(pList->GetCurSel());
				}
				return true;
			}
		}
	}
	if (pNotify->wParam == VK_RETURN)
	{
		pCurSelItem = pList->GetCurSel();
		if (!pCurSelItem)
			pCurSelItem = pList->GetItem(0);
		if (pCurSelItem)
		{
			TNotifyUI Notify = { 0 };
			Notify.lParam = (LPARAM)pCurSelItem;
			OnClickItem(&Notify);
		}
		else
		{
 			if (m_pOwner)
 				m_pOwner->SendNotify(UINOTIFY_EN_KEYDOWN, pNotify->wParam, NULL);
		}
	}
	return true;
}

LRESULT CComboBoxDropDownWnd::MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			POINT pt = {(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam)};
			if (!::PtInRect(m_pOwner->GetRectPtr(), pt))
			{
				m_pOwner->Hide();
			}
		}
		break;
	}

	return S_OK;
}

LRESULT CComboBoxDropDownWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_KILLFOCUS:
		{
			HWND hWnd = GetActiveWindow();
			CWindowBase* pWindow = CWindowBase::FromHandle(hWnd);
			if (pWindow && (pWindow==this ))
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
			CControlUI* pListControl = GetItem(_T("list"));
			CListUI* pList = static_cast<CListUI*>(pListControl);
			if(pList && IsShow())
			{
				TEventUI event = { 0 };
				event.nType = UIEVENT_SCROLLWHEEL;
				event.wParam = wParam;
				event.lParam = lParam;
				CScrollbarUI *pVScrollBar = pList->GetVScrollbar();
				if(pVScrollBar) 
					pVScrollBar->SendEvent(event);
			}
		}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CComboBoxDropDownWnd::SetLockNotify( bool bLock )
{
	m_bLockNotify = bLock;
}

bool CComboBoxDropDownWnd::IsShow()
{
	return !!::IsWindowVisible(GetHWND());
}
//////////////////////////////////////////////////////////////////////////;
CComboBoxUI::CComboBoxUI()
:m_pList(NULL)
,m_pToolWindow(NULL)
,m_nArrowIndex(UISTATE_NORMAL)
,m_nItemsEnsureVisible(4)
,m_bDropList(true)
,m_bEnableCheck(false)
,m_nItemsHeight(0)
,m_strWndStyle(_T(""))
{
	RECT rect = { 5, 2, 25, 2 };
	SetInset(rect);
	SetStyle(_T("ComboBox"));
	SetAttribute(_T("caretenable"), _T("0"));
	::SetRect(&m_rcPadding, 0, 0, 0, 0);
}

CComboBoxUI::~CComboBoxUI()
{
	if (m_pToolWindow)
	{
		m_pToolWindow->CloseWindow();
		delete m_pToolWindow;
	}
	m_pToolWindow = NULL;
	m_pList = NULL;
}


CListUI* CComboBoxUI::GetList()
{
	if (m_pList == NULL)
	{
		m_pToolWindow = new CComboBoxDropDownWnd(this);
		CControlUI* pListControl = m_pToolWindow->GetItem(_T("list"));
		RT_CLASS_ASSERT(pListControl, CListUI, _T("CComboBoxDropDownWnd list"));

		CListUI* pList = static_cast<CListUI*>(pListControl);
		m_pList = pList;
		if (m_pList)
		{
			if (m_bEnableCheck)
			{
				m_pList->SetAttribute(_T("enableCheck"), _T("1"));
				m_pList->SetAttribute(_T("visibleHeaderCheck"), _T("0"));
			}
			((CControlUI*)m_pList)->Init();
		}
	}
	return m_pList;
}

bool CComboBoxUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_SETFOCUS:
		{
			if (GetUIEngine()->IsRTLLayout())
			{
				if(IsKillFocus())
				{
					m_bFocused = true;
					if(IsDrop())
					{
						if (GetState() != UISTATE_OVER)
							SetState(UISTATE_OVER);
						this->Invalidate();
						return true;
					}
					else
					{
						if (GetState() != UISTATE_OVER)
							SetState(UISTATE_OVER);
						__super::Event(event);
						// xqb del 阿拉伯版本暂时去掉全选，否则登录框会很奇怪
						//SelectAllText();
						return true;
					}
				}

			}
			else
			{
				if(IsKillFocus())
				{
					m_bFocused = true;
					this->OnSetFocus();
					if(IsDrop())
					{
						if (GetState() != UISTATE_OVER)
							SetState(UISTATE_OVER);
						this->Invalidate();
					}
					else
					{
						if (GetState() != UISTATE_OVER)
							SetState(UISTATE_OVER);
						SelAllText();
					}
				}
				return  true;

			}
			
		}
		break;
	case UIEVENT_KILLFOCUS:
		{
			if (GetUIEngine()->IsRTLLayout())
			{
				m_bFocused = false;

				m_nArrowIndex = UISTATE_NORMAL;
				SetState(UISTATE_NORMAL);
				this->Invalidate();
				HWND hWnd = GetActiveWindow();
				CWindowBase* pWindow = CWindowBase::FromHandle(hWnd);
				if (pWindow && (pWindow == m_pToolWindow))
				{
					return true;
				}
				Hide();

			}
			else
			{
				m_bFocused = false;

				m_nArrowIndex = UISTATE_NORMAL;
				SetState(UISTATE_NORMAL);
				this->Invalidate();
				this->OnKillFocus();
				HWND hWnd = GetActiveWindow();
				CWindowBase* pWindow = CWindowBase::FromHandle(hWnd);
				if (pWindow && (pWindow == m_pToolWindow))
				{
					return true;
				}
				Hide();

			}
			
		}
		break;

	//以下事件不用判定阿拉伯版本
	case UIEVENT_BUTTONDOWN:
		{
			if (PtInRect(&GetInterRect(), event.ptMouse))
			{
				if(IsDrop())
				{
					if (IsShow())
						Hide();
					else
					{
						SendNotify(UINOTIFY_CBN_BEFOREDROPDOWN, NULL, NULL);
						Show();
					}
					this->Invalidate(true);
					return true;
				}
				else
				{
					if (IsShow())
					{
						Hide();
						this->Invalidate(true);
						return true;
					}
				}
				break;
			}
			{
				StyleObj* pStyle = GetStyle(_Style_ComboBox_Right_);
				if (pStyle == NULL)
					break;
				if(pStyle->HitTest(event.ptMouse))
				{
					m_nArrowIndex = UISTATE_DOWN;
					if (IsShow())
					{
						Hide();
						this->Invalidate(true);
					}
					else
					{
						this->SendNotify(UINOTIFY_CBN_BEFOREDROPDOWN, NULL, NULL);
						Show();
						this->Invalidate(true);
					}
					if (!IsDrop())
					{
						if (IsShow())
							this->SelAllText();
					}
				}
				return true;
			}
		}
		break;
	case UIEVENT_LDBCLICK:
	case UIEVENT_RBUTTONUP:
	case UIEVENT_RBUTTONDOWN:
		{
			if(IsDrop()) 
			{
				return true;
			}
		}
		break;
	case UIEVENT_SETCURSOR:
		{	
			StyleObj* pStyle = GetStyle(_Style_ComboBox_Right_);
			if (pStyle == NULL)
				break;
			if(pStyle->HitTest(event.ptMouse))
			{
				::SetCursor(LoadCursor(NULL, IDC_ARROW));
				return true;
			}
			else
			{
				if(IsDrop())
				{
					::SetCursor(LoadCursor(NULL, IDC_ARROW));
					return true;
				}
				else if(PtInRect(&GetInterRect(), event.ptMouse))
				{
				}
			}
		}
		break;
	
	case UIEVENT_SCROLLWHEEL:
		{
			if (IsShow() && m_pToolWindow)
			{
				::SendMessage(m_pToolWindow->GetHWND(),WM_MOUSEWHEEL,event.wParam, event.lParam);
			}
		}
		break;
	
	case UIEVENT_MOUSEMOVE:
	case UIEVENT_MOUSEENTER:
		{
			if (!IsShow())
			{
				StyleObj* pStyle = GetStyle(_Style_ComboBox_Right_);
				if (pStyle == NULL)
					break;
				bool bFlag = false;
				if (pStyle->HitTest(event.ptMouse))
				{
					if (m_nArrowIndex != UISTATE_OVER)
					{
						m_nArrowIndex = UISTATE_OVER;
						bFlag = true;
					}
				}
				else
				{
					if (m_nArrowIndex != UISTATE_NORMAL)
					{
						m_nArrowIndex = UISTATE_NORMAL;
						bFlag = true;
					}
				}

				if (GetState() != UISTATE_OVER)
				{
					bFlag = true;
					SetState(UISTATE_OVER);
				}
				if (bFlag)
				{
					this->Invalidate();
				}
				if(IsDrop())
				{
					return true;
				}
			}
		}
		break;
	case UIEVENT_MOUSELEAVE:
		{
			if (IsKillFocus())
			{
				m_nArrowIndex = UISTATE_NORMAL;
				SetState(UISTATE_NORMAL);
				this->Invalidate();
			}
			else if(m_nArrowIndex != UISTATE_DOWN)
			{
				m_nArrowIndex = UISTATE_NORMAL;
				SetState(UISTATE_OVER);
				this->Invalidate();
			}
		}
		break;
	case UIEVENT_KEYDOWN:
		{
			if (IsDrop())
			{
				if (m_pList)
				{
					m_pList->SendNotify(UINOTIFY_EN_KEYDOWN, event.wParam, event.lParam);
				}
			}
		}
		break;
	}
	return __super::Event(event);
}

bool CComboBoxUI::Add(CControlUI* pControl)
{
	if (GetList())
	{
		return m_pList->Add(pControl);
	}
	return false;
}

void CComboBoxUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	StyleObj* pStyle = NULL;
	pStyle = GetStyle(_Style_ComboBox_Right_);
	if (pStyle)
		pStyle->SetState(IsShow()?UISTATE_DOWN:IsDrop()?GetState():m_nArrowIndex);

	__super::Render(pRenderDC, rcPaint);
}

void CComboBoxUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("ComboType")))
	{
		if(equal_icmp(lpszValue, _T("DropList")))
		{
			m_bDropList = true;
			SetAttribute(_T("caretenable"), _T("0"));
		}
		else if(equal_icmp(lpszValue, _T("DropDown")))
		{
			m_bDropList = false;
			SetAttribute(_T("caretenable"), _T("1"));
		}
	}
	else if (equal_icmp(lpszName, _T("enableCheck")))
	{
		m_bEnableCheck = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("ItemsEnsureVisible")))
	{
		m_nItemsEnsureVisible = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("WndStyle")))
	{
		m_strWndStyle = lpszValue;
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
		if (IsRTLLayoutEnable()
			&& GetUIEngine()->IsRTLLayout())
		{
			m_rcPadding.left = pos[2];
			m_rcPadding.right = pos[0];
		}
	}
	else if (equal_icmp(lpszName, _T("ItemsHeight")))
	{
		m_nItemsHeight = _ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName,lpszValue);
	}
}

CControlUI* CComboBoxUI::GetCurSel()
{
	if (GetList() == NULL)
		return NULL;
	return m_pList->GetCurSel();
}


CControlUI* CComboBoxUI::GetItem(int index)
{
	if (GetList() == NULL)
		return NULL;
	return m_pList->GetItem(index);
}

CControlUI* CComboBoxUI::FindFirstItem(LPCTSTR lpszText)
{
	//CControlUI* pControl = NULL;
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
	{
		CControlUI* pControl = GetItem(i);
		if (equal_icmp(pControl->GetText(), lpszText))
		{
			return pControl;
		}
	}
	return NULL;
}

void CComboBoxUI::SelectItem(CControlUI* pControl)
{
	if (GetList() == NULL)
		return;
	if (pControl)
	{
		//CControlUI* pCtr = NULL;
		bool bFlag = false;
		int nCnt = GetCount();
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pCtr = GetItem(i);
			if (pCtr && pCtr == pControl)
			{
				SelectItem(i, true);
				bFlag = true;
				break;
			}
		}
		if (!bFlag)
			SelectItem(-1, false);
	}
	else
	{
		SelectItem(-1, false);
	}
}

void CComboBoxUI::SelectItem(int index, bool bTextChange)
{
	if (GetList() == NULL)
		return;
	CControlUI* pControl = m_pList->GetItem(index);
	if (pControl)
	{
		if (bTextChange)
		{
			SetTextWithoutNotify(pControl->GetText());
			SetToolTip(pControl->GetToolTip());;
			copy_str(m_lpszTextKey, pControl->GetTextKey());
			copy_str(m_lpszToolTipsKey, pControl->GetTooltipKey());
			
		}
		m_pList->SelectItem(pControl);
		//CWindowUI * pManager = this->GetWindow();
	}
	else
	{
		if (bTextChange)
		{
			SetTextWithoutNotify(_T(""));
			SetToolTip(_T(""));

			copy_str(m_lpszTextKey, NULL);
			copy_str(m_lpszToolTipsKey, NULL);
		}
		m_pList->SelectItem(NULL);
	}
	SendNotify(UINOTIFY_SELCHANGE);
	this->Invalidate();
}

void CComboBoxUI::SelectItem(int index)
{
	if (IsDrop())
		SelectItem(index, !IsEnableCheck());
	else
		SelectItem(index, true);
	
}

void CComboBoxUI::SetText(LPCTSTR lpszText)
{
	__super::SetText(lpszText);
	SetToolTip(lpszText);
}

bool CComboBoxUI::RemoveAll()
{
	if (GetList() == NULL)
		return false;
	if (m_pList)
		m_pList->RemoveAll();
	return true;
}

tstring &CComboBoxUI::GetWndStyle()
{
	return m_strWndStyle;
}

bool CComboBoxUI::Remove(CControlUI* pControl)
{
	if (GetList() == NULL)
		return false;
	if (m_pList)
		return m_pList->Remove(pControl);
	return false;
}

int CComboBoxUI::GetCount()
{
	if (GetList() == NULL)
		return 0;
	return m_pList->GetCount();
}

RECT CComboBoxUI::GetArrowRect()
{
	StyleObj* pStyle = GetStyle(_Style_ComboBox_Right_);
	if (pStyle)
		return pStyle->GetRect();
	return GetRect();
}

#define _Style_ListItem_Txt 2
void CComboBoxUI::Show()
{
	if (NULL == GetList())
		return;
	if (NULL == m_pToolWindow)
		return;
	HWND hToolWindow = CComboBoxDropDownWnd::ms_hShowInstance;
	if (hToolWindow)
	{
		if ( ::IsWindow(hToolWindow) && m_pToolWindow && m_pToolWindow->GetHWND() != hToolWindow && ::IsWindowVisible(hToolWindow) )
			ShowWindow(hToolWindow, SW_HIDE);
	}
	if (m_pToolWindow)
	{
		CComboBoxDropDownWnd* pWnd = static_cast<CComboBoxDropDownWnd*>(m_pToolWindow);
		if (pWnd)
			pWnd->SetLockNotify(true);
	}
	ShowDrop();

	if (m_pToolWindow)
	{
		RECT rcWindow;
		::GetWindowRect(m_pToolWindow->GetHWND(), &rcWindow);
		//int nMaxWidth = rcWindow.right - rcWindow.left;
		CListItemUI* pItem = NULL;
		//CControlUI* pControl = NULL;
		TextStyle* pText = NULL;
		int nCnt = GetCount();
		for (int i = 0; i <nCnt; i++)
		{
			CControlUI* pControl = GetItem(i);
			pItem = static_cast<CListItemUI*>(pControl);
			if (pItem)
			{
				pText = dynamic_cast<TextStyle*>((StyleObj*)pItem->GetStyle(_Style_ListItem_Txt));
				if (pText)
				{
					LPCTSTR lpszText = pText->GetText();
					if (!lpszText) 
						lpszText = pItem->GetText();
					if (_tcslen(lpszText) > 0)
					{
						RECT rect = pText->GetRect();
						RECT rect1 = rect;
						::OffsetRect(&rect1, -rect1.left, -rect1.top);

						pText->GetFontObj()->CalcText(m_pToolWindow->GetPaintDC(), rect1, lpszText);
						if (rect1.right - rect1.left > rect.right - rect.left)
							pItem->SetToolTip(lpszText);
						else
							pItem->SetToolTip(_T(""));

					}
				}
			}
		}
	}
	if (m_pToolWindow)
	{
		RECT rcClient;
		::GetClientRect(m_pToolWindow->GetHWND(), &rcClient);
		m_pToolWindow->GetRoot()->OnSize(rcClient);
		CControlUI* pControl = m_pList->GetCurSel();
		if (pControl)
			m_pList->EnsureVisible(pControl);

		CComboBoxDropDownWnd* pWnd = static_cast<CComboBoxDropDownWnd*>(m_pToolWindow);
		if (pWnd)
			pWnd->SetLockNotify(false);
	}
	if (m_pToolWindow)
	{
		CComboBoxDropDownWnd::ms_hShowInstance = m_pToolWindow->GetHWND();
	}
}

void CComboBoxUI::ShowDrop()
{
	int nMinItems = m_pList->GetCount();
	nMinItems = nMinItems > m_nItemsEnsureVisible ? m_nItemsEnsureVisible : nMinItems;
	if (nMinItems)
	{
		RECT rect = GetRect();
		POINT pt = {rect.left, rect.bottom};
		::ClientToScreen(GetWindow()->GetHWND(), &pt);
		if (!m_bEnableCheck)
		{
			CControlUI* pControl = GetCurSel();
			if (pControl == NULL)
				pControl = FindFirstItem(GetText());
			if (pControl)
				m_pList->SelectItem(pControl);
		}

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

		int iHeight;
		if (m_nItemsHeight > 0)
		{
			iHeight = nMinItems * m_nItemsHeight + 4;
		}
		else
		{
			iHeight = nMinItems * 28 + 4;
		}
		
		iHeight = DPI_SCALE(iHeight);
		if (pt.y+iHeight > rcDesktop.bottom)
		{
			pt.x = rect.left;
			pt.y = rect.top;
			::ClientToScreen(GetWindow()->GetHWND(), &pt);
			m_pToolWindow->SetWindowPos(HWND_DESKTOP, pt.x - DPI_SCALE(m_rcPadding.left), pt.y - iHeight - DPI_SCALE(m_rcPadding.top), rect.right - rect.left + DPI_SCALE(m_rcPadding.right), iHeight + DPI_SCALE(m_rcPadding.bottom), SWP_NOACTIVATE);
		}
		else
		{
			m_pToolWindow->SetWindowPos(HWND_DESKTOP, pt.x - DPI_SCALE(m_rcPadding.left), pt.y - DPI_SCALE(m_rcPadding.top), rect.right - rect.left + DPI_SCALE(m_rcPadding.right), iHeight + DPI_SCALE(m_rcPadding.bottom), SWP_NOACTIVATE);
		}

		//m_pToolWindow->SetWindowPos(HWND_DESKTOP, pt.x, pt.y, rect.right - rect.left, nMinItems * 28 + 4, SWP_NOACTIVATE);
		m_pToolWindow->ShowWindow(SW_SHOWNOACTIVATE);
		m_pToolWindow->GetRoot()->OnlyResizeChild();
		m_pToolWindow->GetRoot()->Invalidate();
	}
}

void CComboBoxUI::Hide()
{
	if (m_pToolWindow)
	{
		m_pToolWindow->ShowWindow(SW_HIDE);
	}

	SetState(UISTATE_NORMAL);
	this->Invalidate();
}

void CComboBoxUI::HideDropList()
{
	this->Hide();
}

bool CComboBoxUI::IsShow()
{
	if (m_pToolWindow)
		return !!::IsWindowVisible(m_pToolWindow->GetHWND());
	return false;
}

void CComboBoxUI::OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt /*= NULL*/)
{
	if (IsDrop())
		return;
	__super::OnChar(wParam, lParam, ppt);
}

void CComboBoxUI::OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt /*= NULL*/)
{
	if (IsDrop())
	{
		if (wParam == VK_BACK || wParam == VK_DELETE)
			return;
	}
	__super::OnKeyDown(wParam, lParam, ppt);
}

void CComboBoxUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	Hide();
}

void CComboBoxUI::CheckItem(bool bCheck, int nIndex/* = -1*/)
{
	if (GetList())
	{
		m_pList->CheckItem(bCheck, nIndex);
	}
}

bool CComboBoxUI::IsCheckItem(int nIndex)
{
	if (GetList())
	{
		return m_pList->IsCheckItem(nIndex);
	}
	return false;
}

bool CComboBoxUI::IsEnableCheck()
{
	return m_bEnableCheck;
}

bool CComboBoxUI::IsDrop()
{
	return m_bDropList;
}

bool CComboBoxUI::IsKillFocus()
{
	if (IsDrop())
	{
		return !IsShow();
	}
	else
	{
		return !IsFocused();
	}
}

void CComboBoxUI::OnLanguageChange()
{
	__super::OnLanguageChange();
	if (m_pList) ((CControlUI*)m_pList)->OnLanguageChange();

}