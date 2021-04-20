#include "stdafx.h"
#include "UIContainer.h"

/////////////////////////////////////////////////////////////////////////////////////
CContainerUI::CContainerUI()
:m_pVerticalScrollbar(NULL)
,m_pHorizontalScrollbar(NULL)
,m_bScrollProcess(false)
,m_bEnableVertical(false)
,m_bEnableHorizontal(false)
,m_bAutoFitWidth(false)
,m_bAutoFitHeight(false)
,m_pCurSelItem(NULL)
, m_bShowVertical(true)
, m_bShowHorizontal(true)
, m_bFolatVertical(false)
, m_bFolatHorizontal(false)
{
	SetMouseEnabled(false);
	ZeroMemory(&m_rcClient, sizeof(m_rcClient));
}

CContainerUI::~CContainerUI()
{
	RemoveAll();
	if( m_pVerticalScrollbar ) delete m_pVerticalScrollbar;
	m_pVerticalScrollbar = NULL;
	if( m_pHorizontalScrollbar ) delete m_pHorizontalScrollbar;
	m_pHorizontalScrollbar = NULL;
}


CControlUI* CContainerUI::GetItem(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
	return static_cast<CControlUI*>(m_items[iIndex]);
}


CControlUI* CContainerUI::GetItem(LPCTSTR lpszId)
{
	tstring strID = lpszId;
	int i = strID.find_first_of(_T('.'));
	if (i != tstring::npos)
	{
		CControlUI* pControl = GetItem(strID.substr(0, i).c_str());
		if (pControl)
		{
			strID = strID.substr(i + 1);
			return pControl->GetItem(strID.c_str());
		}
	}
	//CControlUI* pControl = NULL;
	int nCnt = m_items.GetSize();
	for( int it = 0;  it < nCnt; it++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		if (equal_icmp(pControl->GetId(), lpszId))
			return pControl;
	}
	return NULL;	
}

int CContainerUI::GetCount()
{
	return m_items.GetSize();
}

bool CContainerUI::Add(CControlUI* pControlChild)
{
	UICustomControlHelper::AddChildControl(pControlChild, this, m_items);
	return true;
}

bool CContainerUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	UICustomControlHelper::InsertChildControl(pControlChild, pControlAfter, this, m_items);
	return true;
}

bool CContainerUI::MoveItem(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	if (pControlChild->GetParent() != this)
		return false;
	int nAfterIndex = 0;
	if (pControlAfter) 
		nAfterIndex = pControlAfter->GetIndex();

	int nControlIndex = pControlChild->GetIndex();

	if (nAfterIndex == nControlIndex) 
		return false;
	bool bMoveBackward = false;
	if (nAfterIndex > nControlIndex)
		bMoveBackward = true;

	int nBegin = 0, nEnd = 0;
	if (bMoveBackward)
		m_items.Move(nControlIndex + 1, nAfterIndex, nControlIndex);
	else
		m_items.Move(nAfterIndex, nControlIndex - 1, nControlIndex);

	m_items.SetAt(nAfterIndex, pControlChild);
	nBegin = min(nControlIndex, nAfterIndex);
	nEnd = max(nControlIndex, nAfterIndex);
	for (int i = nBegin; i <= nEnd; i++)
		GetItem(i)->SetIndex(i);
	return true;
}

bool CContainerUI::Remove(CControlUI* pControlChild)
{
	if (m_pCurSelItem == pControlChild)
		m_pCurSelItem = NULL;
	return UICustomControlHelper::RemoveChildControl(pControlChild, m_items);
}

void CContainerUI::SetVisible(bool bVisible)
{
	int nCnt = GetCount();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible && IsInternVisible());
	__super::SetVisible(bVisible);

}
void CContainerUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	int nCnt = GetCount();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible && IsVisible());
}

bool CContainerUI::IsContainer()
{
	return true;
}

void CContainerUI::RemoveAll()
{
	m_pCurSelItem = NULL;
	UICustomControlHelper::RemoveAllChild(m_items);

	CScrollbarUI* pScrollbar = GetVScrollbar();
	if( pScrollbar )
		pScrollbar->SetScrollPos(0, false);

	pScrollbar = GetHScrollbar();
	if( pScrollbar )
		pScrollbar->SetScrollPos(0, false);
}


void CContainerUI::ReleaseImage()  
{
	//CControlUI* pControl = NULL;
	int nCnt = GetCount();
	for( int i = 0;  i < nCnt; i++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items.GetAt(i));
		if (pControl)
			pControl->ReleaseImage();
	}
	__super::ReleaseImage();
}

CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if (!FindControlTest(this, pData, uFlags))
		return NULL;

	CControlUI* pControl = NULL;
	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		pControl = __super::FindControl(Proc, pData, uFlags);
		if(pControl)
			return pControl;
	}

	CScrollbarUI* pVScrollbar = GetVScrollbar();
	if(m_bShowVertical && pControl == NULL && pVScrollbar != NULL) 
		pControl = pVScrollbar->FindControl(Proc, pData, uFlags);

	CScrollbarUI* pHScrollbar = GetHScrollbar();
	if(m_bShowHorizontal && pControl == NULL && pHScrollbar != NULL ) 
		pControl = pHScrollbar->FindControl(Proc, pData, uFlags);

	if (pControl != NULL)
		return pControl;

	pControl = UICustomControlHelper::FindChildControl(m_items, Proc, pData, uFlags);
	if (pControl != NULL)
		return pControl;

	return __super::FindControl(Proc, pData, uFlags);

}

CControlUI* CContainerUI::FindSubControl(LPCTSTR pstrSubControlName)
{
	return this->FindItem(pstrSubControlName);
}


bool CContainerUI::OnlyResizeChild()
{
	CalcChildRect();
	return true;
}

void CContainerUI::OnlyShowChild(CControlUI *pControl)
{
	if (Contain(pControl))
	{
		for (int i = 0; i < m_items.GetSize(); i++)
		{
			GetItem(i)->SetVisible(GetItem(i) == pControl);
		}
	}
}

void CContainerUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("HScroll")))
	{
		m_bEnableHorizontal = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("VScroll")))
	{
		m_bEnableVertical = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("AutoFitHeight")))
	{
		m_bAutoFitHeight = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("AutoFitWidth")))
	{
		m_bAutoFitWidth = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("showvscroll")))
	{
		m_bShowVertical = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("showhscroll")))
	{
		m_bShowHorizontal = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("floatvscroll")))
	{
		m_bFolatVertical = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("floathscroll")))
	{
		m_bFolatHorizontal = !!_ttoi(lpszValue);
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}

void CContainerUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{ 
	if (!this->IsVisible())
		return;
	if (GetCount() > 0)
	{
		RECT rcTemp = { 0 };
		RECT rcItem = GetRect();
		RECT rcClient = GetClientRect();
		CControlUI* pParent = GetParent();
		if (pParent)
		{
			::IntersectRect(&rcItem, pParent->GetRectPtr(), GetRectPtr());
			::IntersectRect(&rcClient, &rcItem, &rcClient);
		}
		if( !::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
			return;

		HDC hDestDC = pRenderDC->GetDC();
		//画父控件
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, rcTemp, clip);
		CControlUI::Render(pRenderDC, rcPaint);

		UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, m_items);
		if (m_bShowVertical)
		{
			UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetVScrollbar());
		}
		if (m_bShowHorizontal)
		{
			UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetHScrollbar());
		}

	}
	else __super::Render(pRenderDC, rcPaint);
}


RECT CContainerUI::GetClientRect()
{
	return m_rcClient;
}


void CContainerUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	__super::SetManager(pManager, pParent);
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		GetItem(i)->SetManager(pManager, this);
}


void CContainerUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	CalcChildRect();
}

void  CContainerUI::CalcChildRect()
{
	RECT rcItem = GetRect();
	//计算客户区大小
	CScrollbarUI* pVScrollbar = GetVScrollbar();
	CScrollbarUI* pHScrollbar = GetHScrollbar();
	m_rcClient = GetInterRect();
	if( pVScrollbar && pVScrollbar->IsVisible() )
	{	
		pVScrollbar->OnSize(rcItem);
		if( pHScrollbar && pHScrollbar->IsVisible() && m_bShowHorizontal && !m_bFolatHorizontal)
			pVScrollbar->GetRectPtr()->bottom -= DPI_SCALE(pHScrollbar->GetFixedSize().cy);
		if (m_bShowVertical && !m_bFolatVertical)
			m_rcClient.right -= DPI_SCALE(m_pVerticalScrollbar->GetFixedSize().cx);
	}
	if( pHScrollbar && pHScrollbar->IsVisible())
	{
		pHScrollbar->OnSize(rcItem);
		if( pVScrollbar && pVScrollbar->IsVisible()  && m_bShowVertical && !m_bFolatVertical)
			pHScrollbar->GetRectPtr()->right -= DPI_SCALE(pVScrollbar->GetFixedSize().cx);
		if (m_bShowHorizontal && !m_bFolatHorizontal)
			m_rcClient.bottom -= DPI_SCALE(m_pHorizontalScrollbar->GetFixedSize().cy);
	}

	if (IsRTLLayoutEnable())
	{
		RtlRect(rcItem, &m_rcClient);
	}

	RECT rcScroll = GetScrollRect(); 
	
	SIZE sz = OnChildSize(rcItem, rcScroll);
	if (m_bAutoFitHeight || m_bAutoFitWidth)
	{
		RECT rect = rcItem;
		if (m_bAutoFitWidth)
		{

			int nWidth = 0;
			if (GetUIEngine()->IsRTLLayout() 
				&& IsRTLLayoutEnable())
			{
				nWidth = (rcItem.right - m_rcClient.right) + (m_rcClient.left - rcItem.left);
				rect.left = rect.right - nWidth - (sz.cx > 0 ? sz.cx: 0);
				m_rcClient.left = m_rcClient.left - (rect.left - rcItem.left);

			}
			else
			{
				nWidth = (rcItem.right - m_rcClient.right) + (m_rcClient.left - rcItem.left);
				rect.right = rect.left + nWidth + (sz.cx > 0 ? sz.cx: 0);
				m_rcClient.right = m_rcClient.right + rect.right - rcItem.right;

			}

		}
		else if (m_bAutoFitHeight)
		{
			int nHeight = (rcItem.bottom - m_rcClient.bottom) + (m_rcClient.top - rcItem.top);
			rect.bottom = rect.top + nHeight + (sz.cy > 0 ? sz.cy: 0);
			m_rcClient.bottom = m_rcClient.bottom + rect.bottom - rcItem.bottom;
		}
		{
			CViewUI::SetRect(rect);

			int nCnt = GetStyleCount();
			for (int i = 0; i < nCnt; i++)
			{
				StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
				if (pStyleObj)
					pStyleObj->OnSize(rect);
			}
		}
	}
	OnProcessScrollbar(sz.cx, sz.cy);
}


SIZE CContainerUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	return UICustomControlHelper::LayoutChildNormal(rcWinow, rcClient, m_items, GetVScrollbar() || GetHScrollbar());
}

bool CContainerUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_HSCROLL:
	case UIEVENT_VSCROLL:
		{
			// 101ppt修改的不确定代码
			//Resize();
			CalcChildRect();
			Invalidate();
		}
		return true;

	case UIEVENT_SCROLLWHEEL:
		{
			CScrollbarUI* pVScrollbar = GetVScrollbar();
			if (pVScrollbar && pVScrollbar->IsVisible())
			{
				pVScrollbar->SendEvent(event);
				return true;
			}
            else if (GetHScrollbar() && GetHScrollbar()->IsVisible())
            {
                GetHScrollbar()->SendEvent(event);
                return true;
            }
		}
		break;

	case UIEVENT_GESTURE:
	{
		HandleGestureEvent(event);
	}
	break;
	}

	return CControlUI::Event(event);
}

void CContainerUI::HandleGestureEvent(TEventUI& event)
{
	static POINT lastPt = { 0, 0 }; // 记录上次的位置
	GESTUREINFO gi = {};
	gi.cbSize = sizeof(GESTUREINFO);
	if (get_gesture_info((HGESTUREINFO)event.lParam, &gi))
	{
		// 默认只处理滑动消息
		if (gi.dwID == GID_PAN)
		{
			if (gi.dwFlags & GF_BEGIN)
			{
				lastPt = event.ptMouse;

			}
			else if (gi.dwFlags & GF_END)
			{
				lastPt.x = lastPt.y = 0;
			}
			else
			{
				int offsetX = event.ptMouse.x - lastPt.x;
				int offsetY = event.ptMouse.y - lastPt.y;
				lastPt = event.ptMouse;
				if (IsEnableHScrollbar())
				{
					CScrollbarUI* pHSCrollBar = GetHScrollbar();
					if (pHSCrollBar && pHSCrollBar->IsVisible())
					{
						int nCurPos = pHSCrollBar->GetScrollPos();
						pHSCrollBar->SetScrollPos(nCurPos - offsetY);
					}
				}

				if (IsEnableVScrollbar())
				{
					CScrollbarUI* pVSCrollBar = GetVScrollbar();
					if (pVSCrollBar && pVSCrollBar->IsVisible())
					{
						int nCurPos = pVSCrollBar->GetScrollPos();
						pVSCrollBar->SetScrollPos(nCurPos - offsetY);
					}
				}
			}
		}
	}

}

void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
	m_bEnableVertical = bEnableVertical;
	m_bEnableHorizontal = bEnableHorizontal;
	if (!bEnableVertical)
	{
		if (m_pVerticalScrollbar)
			delete m_pVerticalScrollbar;			
		m_pVerticalScrollbar = NULL;
	}

	if (!bEnableHorizontal)
	{
		if (m_pHorizontalScrollbar)
			delete m_pHorizontalScrollbar;
		m_pHorizontalScrollbar = NULL;
	}
}

bool CContainerUI::IsEnableHScrollbar()
{
	return m_bEnableHorizontal;
}

bool CContainerUI::IsEnableVScrollbar()
{
	return m_bEnableVertical;
}

CScrollbarUI* CContainerUI::GetHScrollbar()
{
	if (!m_pHorizontalScrollbar && IsEnableHScrollbar())
	{
		m_pHorizontalScrollbar = new CScrollbarUI;
		m_pHorizontalScrollbar->SetOwner(this);
		m_pHorizontalScrollbar->SetManager(GetWindow(), this);
		m_pHorizontalScrollbar->SetHorizontal(true);
		m_pHorizontalScrollbar->SetVisible(false);
	}

	return m_pHorizontalScrollbar;
}

CScrollbarUI* CContainerUI::GetVScrollbar()
{
	if (!m_pVerticalScrollbar && IsEnableVScrollbar())
	{
		m_pVerticalScrollbar = new CScrollbarUI;
		m_pVerticalScrollbar->SetOwner(this);
		m_pVerticalScrollbar->SetManager(GetWindow(), this);
		m_pVerticalScrollbar->SetHorizontal(false);
		m_pVerticalScrollbar->SetVisible(false);
	}
	return m_pVerticalScrollbar;
}

void CContainerUI::OnProcessScrollbar(int cxRequired, int cyRequired)
{
	CScrollbarUI* pVScrollbar = GetVScrollbar();
	CScrollbarUI* pHScrollbar = GetHScrollbar();

	if (!pVScrollbar && !pHScrollbar)
		return;
	if (!m_bScrollProcess)
	{
		RECT rect = GetInterRect();
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
		
		//BOOL bChange = true;
		bool bNeedVScroll = false;
		bool bNeedHScroll = false;
		if (pVScrollbar && cyRequired > nHeight)
			bNeedVScroll = true;
		if (pHScrollbar && cxRequired > nWidth)
			bNeedHScroll = true;


		if (pVScrollbar && pHScrollbar)
		{
			if (!bNeedVScroll && bNeedHScroll && m_bShowHorizontal && !m_bFolatHorizontal)
			{
				if (cyRequired > nHeight - DPI_SCALE(pHScrollbar->GetFixedSize().cy))
				{
					bNeedVScroll = true;
				}
			}

			if (bNeedVScroll && !bNeedHScroll && m_bShowVertical && !m_bFolatVertical)
			{
				if (cxRequired > nWidth - DPI_SCALE(pVScrollbar->GetFixedSize().cx))
				{
					bNeedHScroll = true;
				}
			}
		}
	


		bool bOriLock = GetWindow()->LockUpdate();
		bool bLastHScrollVisible = false;
		bool bLastVScrollVisible = false;
		if (pHScrollbar)
			bLastHScrollVisible = pHScrollbar->IsVisible();
		if (pVScrollbar)
			bLastVScrollVisible = pVScrollbar->IsVisible();

	
		if (!bOriLock)
		{
			GetWindow()->UnLockUpdate();
		}

		if (bNeedHScroll != bLastHScrollVisible || bNeedVScroll != bLastVScrollVisible)
		{
			m_bScrollProcess = true;

			if (pVScrollbar)
				pVScrollbar->SetVisible(bNeedVScroll);

			if (pHScrollbar)
				pHScrollbar->SetVisible(bNeedHScroll);
			this->Resize();

			m_bScrollProcess = false;
			return;
		}
	}

	if (pVScrollbar && pVScrollbar->IsVisible())
	{
		/*int nPos = pVScrollbar->GetScrollPos();*/
		int nRange = cyRequired - (m_rcClient.bottom - m_rcClient.top);
		pVScrollbar->SetScrollRange(nRange);
	/*	if (nPos > pVScrollbar->GetScrollPos())
			this->Resize();*/
	}

	if (pHScrollbar && pHScrollbar->IsVisible())
	{
		/*int nPos = pHScrollbar->GetScrollPos();*/
		int nRange = cxRequired - (m_rcClient.right - m_rcClient.left);
		pHScrollbar->SetScrollRange(nRange);
		/*if (nPos > pHScrollbar->GetScrollPos())
				this->Resize();*/
	}
}

void CContainerUI::SetINotifyUI(INotifyUI* pINotify)
{
	CControlUI::SetINotifyUI(pINotify);
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		GetItem(i)->SetINotifyUI(pINotify);
}

CStdPtrArray * CContainerUI::GetItemArray()
{
	return &m_items;
}

bool CContainerUI::RemoveWithoutDelCtrl(CControlUI* pControl)
{
	if (!pControl)
		return false;
	int it = -1;
	for( it = 0;  it < m_items.GetSize(); it++ )
	{
		if( m_items[it] == pControl ) {
			break;
		}
	}
	for (int i = it + 1; i < m_items.GetSize(); i++)
		GetItem(i)->SetIndex(i - 1);
	return m_items.Remove(it);
}


bool CContainerUI::Contain(CControlUI* pControl)
{
	if (!pControl)
		return false;
	for (int i = 0; i < m_items.GetSize(); i++)
	{
		if (GetItem(i) == pControl)
			return true;
	}
	return false;
}

void CContainerUI::OnLanguageChange()
{
	CControlUI::OnLanguageChange();
	//CControlUI* pSub = NULL;
	for (int i = 0; i < GetCount(); i++)
	{
		CControlUI* pSub = GetItem(i);
		if (pSub)
			pSub->OnLanguageChange();
	}
}

void CContainerUI::SetAlpha(int val)
{
	CControlUI::SetAlpha(val);
	for (int i = 0; i < GetCount(); i++)
	{
		CControlUI* pSub = GetItem(i);
		if (pSub)
			pSub->SetAlpha(val);
	}
}

RECT CContainerUI::GetScrollRect()
{
	RECT rcScroll = m_rcClient; 
	CScrollbarUI* pScrollbar = GetVScrollbar();
	if( pScrollbar && pScrollbar->IsVisible() )
	{
		rcScroll.top -= pScrollbar->GetScrollPos();
		rcScroll.bottom -= pScrollbar->GetScrollPos();
	}

	pScrollbar = GetHScrollbar();
	if( pScrollbar && pScrollbar->IsVisible() )
	{
		if (GetUIEngine()->IsRTLLayout())
		{
			rcScroll.left += pScrollbar->GetScrollPos();
			rcScroll.right += pScrollbar->GetScrollPos();

		}
		else
		{
			rcScroll.left -= pScrollbar->GetScrollPos();
			rcScroll.right -= pScrollbar->GetScrollPos();

		}
		
	}
	return rcScroll;
}

bool CContainerUI::SelectItem(CControlUI* pControl)
{
	return CContainerUI::SelectControl(pControl);
}

CControlUI* CContainerUI::GetSelectedItem()
{
	return CContainerUI::GetCurSelControl();
}

bool CContainerUI::SelectControl(CControlUI* pItem)
{
	if (pItem && pItem->IsExceptionGroup())
	{
		return false;
	}

	if( pItem == m_pCurSelItem ) return false;

	CControlUI* pOldItem = m_pCurSelItem;
	if( m_pCurSelItem) 
	{
		m_pCurSelItem->Select(false);
		m_pCurSelItem = NULL;
	}

	if (pItem != NULL)
	{
		m_pCurSelItem = pItem;
		m_pCurSelItem->Select(true);
	}

	TEventUI event = {0};
	event.nType = UIEVENT_ITEM_SEL_CHANGE;
	event.wParam = (WPARAM)pOldItem;
	event.lParam = (LPARAM)pItem;
	this->SendEvent(event);
	return true;
}

CControlUI* CContainerUI::GetCurSelControl()
{
	return m_pCurSelItem;
}