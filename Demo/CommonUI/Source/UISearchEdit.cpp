#include "StdAfx.h"
#include "UIEditEx.h"
#include "UISearchEdit.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
bool FindControlTest(CControlUI* pControl, LPVOID pData, UINT uFlags)
{
	if (!pControl) return false;
	if ((uFlags & UIFIND_VISIBLE) != 0 && !pControl->IsVisible()) return false;
	if ((uFlags & UIFIND_ENABLED) != 0 && !pControl->IsEnabled()) return false;
	if ((uFlags & UIFIND_HITTEST) != 0 && !pControl->HitTest(*static_cast<LPPOINT>(pData))) return false;
	return true;
}

CSearchEditUI* CSearchEditUI::gSearchEditUI = NULL;

CSearchToolWindowUI::CSearchToolWindowUI(CSearchEditUI* pOwner)
{
	if (!pOwner)
		throw std::runtime_error("invalid SearchEdit UI pointer.");

	m_pOwner = pOwner;
	SetLayerWindow(true);
	Create(pOwner->GetWindow()->GetHWND(), _T("search_edit_hint"));
}

CSearchToolWindowUI::~CSearchToolWindowUI()
{
	m_pOwner->m_pToolWindow = NULL;
	m_pOwner->m_pList = NULL;
}

bool CSearchToolWindowUI::OnClickItem(TNotifyUI* pNotify)
{
	if (!m_pOwner)
		return false;

	ShowWindow(SW_HIDE);

	m_pOwner->SetIgnoreUinotifyEnChange(true);

	CListItemUI* pItem = (CListItemUI*)(pNotify->lParam);
	if (pItem)
	{
		m_pOwner->SetText(pItem->GetText());
	}

	m_pOwner->SendNotify(UINOTIFY_EN_LISTCLICK, pNotify->wParam, pNotify->lParam);

	return true;
}

bool CSearchToolWindowUI::OnKeyDown(TNotifyUI* pNotify)
{
	if (!pNotify)
		return false;

	CListUI* pList = dynamic_cast<CListUI*>(GetItem(_T("list")));
	CControlUI *pCurSelItem = NULL;

	if (pNotify->wParam != VK_RETURN)
	{
		if (!pList)
			return false;

		int nCount = pList->GetCount();

		if (nCount <= 0)
			return false;

		pCurSelItem = pList->GetCurSel();

		if (pNotify->wParam == VK_DOWN)
		{
			pCurSelItem = !pCurSelItem ? pList->GetItem(0) : (pCurSelItem->GetIndex() < nCount - 1) ? pList->GetItem(pCurSelItem->GetIndex() + 1) : nullptr;
		}
		else if (pNotify->wParam == VK_UP)
		{
			pCurSelItem = !pCurSelItem ? pList->GetItem(nCount - 1) : (pCurSelItem->GetIndex() > 0) ? pList->GetItem(pCurSelItem->GetIndex() - 1) : nullptr;
		}

		pList->SelectItem(pCurSelItem);
		pList->EnsureVisible(pCurSelItem);
	}
	else
	{
		pCurSelItem = pList->GetCurSel();
		if (!pCurSelItem)
			pCurSelItem = pList->GetItem(0);
		if (pCurSelItem)
		{
			if (m_pOwner)
				m_pOwner->SendNotify(UINOTIFY_EN_LISTRETURN, NULL, (LPARAM)pCurSelItem);
		}
		else
		{
			if (m_pOwner)
				m_pOwner->SendNotify(UINOTIFY_EN_KEYDOWN, pNotify->wParam, NULL);
		}
	}
	return true;
}

LRESULT CSearchToolWindowUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
							 ShowWindow(SW_HIDE);
						 }
	}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////  ////////////////////////

HHOOK g_search_edit_hook = NULL;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_LBUTTONDOWN)
		{
			if (CSearchEditUI::gSearchEditUI)
			{
				if (!CSearchEditUI::gSearchEditUI->MouseHitTest(wParam, lParam))
				{
					CSearchEditUI::gSearchEditUI->HideSearch();
				}
			}
		}
	}
	return CallNextHookEx(g_search_edit_hook, nCode, wParam, lParam);
}

bool CSearchEditUI::MouseHitTest(WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if (!pMouseStruct)
		return false;

	POINT pt1 = { pMouseStruct->pt.x, pMouseStruct->pt.y };
	POINT pt2 = { pMouseStruct->pt.x, pMouseStruct->pt.y };

	// edit control
	::ScreenToClient(GetWindow()->GetHWND(), &pt1);
	if (HitTest(pt1))
		return true;

	// drop down list control
	if (m_pToolWindow)
	{
		CListUI *list = dynamic_cast<CListUI*>(m_pToolWindow->GetItem(_T("list")));
		if (list != NULL)
		{
			::ScreenToClient(list->GetWindow()->GetHWND(), &pt2);
			if (list->HitTest(pt2))
			{
				return true;
			}
		}

	}

	return false;
}

CSearchEditUI::CSearchEditUI()
{
	//SetAttribute(_T("exceptlimit"), _T("space"));
	m_bHideAuto = false;
	SetStyle(_T("Edit"));
	m_pList = NULL;
	m_pToolWindow = NULL;
	SetAttribute(_T("inset"), _T("0,0,62,0"));
	m_bDown = false;
	m_bFirstButtonDown = true;
	m_nMaxItems = 5;
	m_bIgnoreUinotifyEnChange = false;
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS, 0);

	gSearchEditUI = this;

	g_search_edit_hook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());
}

CSearchEditUI::~CSearchEditUI()
{
	ReleaseWnd();
	for (int it = 0; it < m_items.GetSize(); it++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		if (pControl)
			delete pControl;
		pControl = NULL;
	}
	m_items.Empty();

	gSearchEditUI = NULL;
	if (g_search_edit_hook)
		UnhookWindowsHookEx(g_search_edit_hook);
}


void CSearchEditUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	__super::Render(pRenderDC, rcPaint);

	CControlUI* pControl = NULL;
	for (int i = 0; i < m_items.GetSize(); i++)
	{
		pControl = static_cast<CControlUI*>(m_items[i]);
		if (!pControl->IsVisible())
			continue;

		pControl->Render(pRenderDC, rcPaint);
	}
}

CListUI* CSearchEditUI::GetList()
{
	InitWnd();
	return m_pList;
}

void CSearchEditUI::HideSearch()
{
	if (m_pToolWindow && IsWindowVisible(m_pToolWindow->GetHWND()))
	{
		if (m_pList)
			m_pList->RemoveAll();
		m_pToolWindow->ShowWindow(SW_HIDE);
		GetWindow()->Invalidate(NULL, true);
	}
}

void CSearchEditUI::ShowSearch()
{
	if (!m_pToolWindow || !GetWindow())
		return;

	HWND hWndParent = GetWindow()->GetHWND();
	CListUI* pList = GetList();
	if (pList == NULL) return;

	int nCnt = pList->GetCount();
	if (nCnt <= 0)
	{
		m_pToolWindow->ShowWindow(SW_HIDE);
		return;
	}

	//上提示框未测试，待续。
	if (!m_bDown)
		assert(0);

	//以列表框的高度为item高度。
	RECT rcItem = GetRect();
	int nItemHeight = rcItem.bottom - rcItem.top;
	int nWidth = rcItem.right - rcItem.left;
	int nHeight = (nCnt > m_nMaxItems ? m_nMaxItems : nCnt) * nItemHeight;

	auto pt = m_bDown ? POINT{ rcItem.left, rcItem.bottom } : POINT{ rcItem.left, rcItem.top };
	::ClientToScreen(hWndParent, &pt);

	//实际提示框窗口 会有阴影效果，需要补白，实际宽度会大于编辑框。
	nWidth += DPI_SCALE(18);
	nHeight += DPI_SCALE(12);
	pt.x -= DPI_SCALE(10);
	pt.y -= m_bDown ? DPI_SCALE(2) : (nHeight + DPI_SCALE(2));

	if (GetUIEngine()->IsRTLLayout())
	{
		pt.x += DPI_SCALE(2);
	}
	//fixed:258952
	::MoveWindow(m_pToolWindow->GetHWND(), pt.x, pt.y, nWidth, nHeight, FALSE);
	m_pToolWindow->GetRoot()->Resize();
	m_pToolWindow->ShowWindow(SW_SHOWNOACTIVATE);
	m_pToolWindow->Invalidate();
}

void CSearchEditUI::InitWnd()
{
	if (m_pList == NULL)
	{
		if (m_pToolWindow)
			delete m_pToolWindow;
		m_pToolWindow = new CSearchToolWindowUI(this);
		m_pToolWindow->SetAutoDel(false);

		m_pList = dynamic_cast<CListUI*>(m_pToolWindow->GetItem(_T("list")));
		if (m_pList)
		{
			((CControlUI*)m_pList)->Init();
		}
	}
}

void CSearchEditUI::ReleaseWnd()
{
	if (m_pToolWindow)
	{
		m_pToolWindow->CloseWindow();
		delete m_pToolWindow;
	}
	m_pToolWindow = NULL;
	m_pList = NULL;
}
bool CSearchEditUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
		case UIEVENT_KILLFOCUS:
		{
			bool bIn = false;
			for (int it = 0; it < m_items.GetSize(); it++)
			{
				if (event.pSender && event.pSender == static_cast<CControlUI*>(m_items[it]))
				{
					bIn = true;
					break;
				}
			}
			if (m_pToolWindow && !bIn)
			{
				HWND hWnd = GetActiveWindow();
				CWindowBase* pWindow = CWindowBase::FromHandle(hWnd);
				if (!(pWindow && (pWindow == m_pToolWindow)))
				{
					m_pToolWindow->ShowWindow(SW_HIDE);
				}
			}
			m_bFirstButtonDown = true;
		}
		break;

		case UIEVENT_SETFOCUS:
		{
			if (!IsFocused())
			{
				m_bFocused = true;
				m_bFirstButtonDown = true;
				SelAllText();
				OnSetFocus();
				this->SendNotify(UINOTIFY_SETFOCUS, event.wParam, event.lParam);
				//return true;
			}
		}
		break;
		case UIEVENT_KEYDOWN:
		{
			if ((DWORD)event.wParam == VK_DOWN
				|| (DWORD)event.wParam == VK_UP
				|| (DWORD)event.wParam == VK_RETURN)
			{
				if (m_pList)
					m_pList->SendNotify(UINOTIFY_EN_KEYDOWN, event.wParam, NULL);
				// 				if (event.wParam == VK_RETURN && m_strText.empty())
				// 				{
				// 					ShowToolTip(_T("SEARCHKEYNOTEMPTY"));
				// 				}
				return true;
			}
		}
		break;
		case UIEVENT_IME_START:
		{
			this->SendNotify(UINOTIFY_EN_IMESTART, event.wParam, event.lParam);
			HideSearch();
		}
		break;
		case UIEVENT_IME_END:
		{
			this->SendNotify(UINOTIFY_EN_IMEEND, event.wParam, event.lParam);
		}
		break;
		case UIEVENT_IME_ING:
		{
			this->SendNotify(UINOTIFY_EN_IMEING, event.wParam, event.lParam);
			HideSearch();
		}
		break;
		case UIEVENT_BUTTONDOWN:
		{
			if (m_bFirstButtonDown){
				m_bFirstButtonDown = false;
				SelAllText();
				return true;
			}
			// 			if(m_uSelBegin!=m_uSelEnd && m_pManager){
			// 				m_pManager->SendNotify(this, UINOTIFY_EN_CHANGE);
			// 			}
		}
		break;
		case UIEVENT_SCROLLWHEEL:
		{
			if (m_pToolWindow && IsWindowVisible(m_pToolWindow->GetHWND()))
				GetList()->SendEvent(event);
			return true;
		}
		break;
	}
	return __super::Event(event);
}

bool CSearchEditUI::ChildEvent(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONUP:
		if (_tcsicmp(event.pSender->GetId(), _T("editdelete")) == 0)
		{
			LPCTSTR lpszText = GetText();
			if (lpszText && _tcslen(lpszText) > 0)
			{
				SetSelPos(-1);
				OnKeyDown((WPARAM)VK_BACK, NULL);
			}
		}
		else if (_tcsicmp(event.pSender->GetId(), _T("editclear")) == 0)
		{
			LPCTSTR lpszText = GetText();
			if (lpszText && _tcslen(lpszText) > 0)
			{
				SetText(_T(""));
				SetSelPos(-1);
			}
		}
		else if (_tcsicmp(event.pSender->GetId(), _T("editenter")) == 0)
		{
			if (m_pList && m_pToolWindow)
			{
				CControlUI* pCurSelItem = m_pList->GetCurSel();
				if (pCurSelItem == NULL)
					pCurSelItem = m_pList->GetItem(0);
				if (pCurSelItem)
				{
					m_pList->SendNotify(UINOTIFY_EN_KEYDOWN, VK_RETURN, (LPARAM)pCurSelItem);
				}
				else
				{
					this->SendNotify(UINOTIFY_EN_KEYDOWN, VK_RETURN, NULL);
				}
			}
		}
		return true;
	}
	return true;
}

bool CSearchEditUI::Add(CControlUI* pControl)
{
	pControl->OnEvent += MakeDelegate(this, &CSearchEditUI::ChildEvent);
	UICustomControlHelper::AddChildControl(pControl, this, m_items);

	HideAuto();
	return true;
}

CControlUI* CSearchEditUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if (!FindControlTest(this, pData, uFlags))
		return NULL;

	if ((uFlags & UIFIND_ME_FIRST) != 0) {
		CControlUI* pControl = __super::FindControl(Proc, pData, uFlags);
		if (pControl != NULL) return pControl;
	}

	CControlUI* pControl = NULL;
	for (int it = 0; it < m_items.GetSize(); it++)
	{
		pControl = static_cast<CControlUI*>(m_items[it]);
		pControl = pControl->FindControl(Proc, pData, uFlags);
		//if( pControl != NULL && pControl->IsVisible())
		if (pControl != NULL)
			return pControl;
	}
	return __super::FindControl(Proc, pData, uFlags);
}

void CSearchEditUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);

	CControlUI* pControl = NULL;
	RECT rcItem = GetRect();
	for (int i = 0; i < m_items.GetSize(); i++)
	{
		pControl = static_cast<CControlUI*>(m_items[i]);
		if (pControl)
		{
			pControl->OnSize(rcItem);
		}
	}
	if (m_pToolWindow && GetWindow())
	{
		HWND hWnd = m_pToolWindow->GetHWND();
		if (IsWindowVisible(hWnd))
		{
			m_pToolWindow->ShowWindow(SW_HIDE);
			GetWindow()->Invalidate();
			ShowSearch();
		}
	}
}

void CSearchEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("DownType")) == 0)
	{
		m_bDown = (bool)!!_ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("itemsensurevisible")) == 0)
	{
		m_nMaxItems = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("HideAuto")) == 0)
	{
		m_bHideAuto = (bool)!!_ttoi(lpszValue);
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}

void CSearchEditUI::OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	UINT nChar = (UINT)wParam;
	if (nChar == VK_BACK)
	{
		if (m_strText.length() == 1)
		{
			HideAuto();
			Invalidate();
		}
	}
	__super::OnKeyDown(wParam, lParam, ppt);
}

void  CSearchEditUI::OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	/*UINT nChar = (UINT)wParam;
	if (nChar == VK_SPACE)
	{
	return;
	}*/

	__super::OnChar(wParam, lParam, ppt);
	HideAuto();
}

void CSearchEditUI::SetText(LPCTSTR lpszText)
{
	__super::SetText(lpszText);
	HideAuto();
}

void CSearchEditUI::HideAuto()
{
	if (m_bHideAuto)
	{
		for (int i = 0; i < m_items.GetSize(); i++)
		{
			CControlUI * pControl = static_cast<CControlUI*>(m_items[i]);
			if (pControl)
				pControl->SetVisible(!m_strText.empty());
		}
	}
}

void CSearchEditUI::SetVisible(bool bVisible)
{
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible);
	__super::SetVisible(bVisible);
}

void CSearchEditUI::SetInternVisible(bool bVisible)
{
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible);
	__super::SetInternVisible(bVisible);
}
