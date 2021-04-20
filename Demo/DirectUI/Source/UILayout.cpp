#include "stdafx.h"
#include "UILayout.h"

UITYPE_LAYOUT  GetLayoutModeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_icmp(lpszValue, _T("normal")))
		return UILAYOUT_NORMAL;
	if (equal_icmp(lpszValue, _T("lefttoright")))
		return UILAYOUT_LEFTTORIGHT;
	if (equal_icmp(lpszValue, _T("righttoleft")))
		return UILAYOUT_RIGHTTOLEFT;
	if (equal_icmp(lpszValue, _T("toptobottom")))
		return UILAYOUT_TOPTOBOTTOM;
	if (equal_icmp(lpszValue, _T("bottomtotop")))
		return UILAYOUT_BOTTOMTOTOP;
	if (equal_icmp(lpszValue, _T("horzfill")))
		return UILAYOUT_HORZFILL;
	if (equal_icmp(lpszValue, _T("vertfill")))
		return UILAYOUT_VERTFILL;
	if (equal_icmp(lpszValue, _T("grid")))
		return UILAYOUT_GRID;
	return UILAYOUT_NORMAL;
}

CLayoutUI::CLayoutUI()
{
	m_eLayoutType = UILAYOUT_NORMAL;
	m_nStartX = 0;
	m_nStartY = 0;
	m_nSpaceX = 0;
	m_nSpaceY = 0;
	EnableScrollBar(true, true);
}

CLayoutUI::~CLayoutUI()
{

}

void CLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("mode")))
	{
		m_eLayoutType = GetLayoutModeValueFromStr(lpszValue);

		switch(m_eLayoutType)
		{
		case UILAYOUT_VERTFILL:
			{
				EnableScrollBar(false, IsEnableHScrollbar());
			}
			break;
		case UILAYOUT_HORZFILL:
			{
				EnableScrollBar(IsEnableVScrollbar(), false);
			}
			break;
		}
	}
	else if (equal_icmp(lpszName, _T("startx")))
		SetStartX( _ttoi(lpszValue));
	else if (equal_icmp(lpszName, _T("starty")))
		SetStartY(_ttoi(lpszValue));
	else if (equal_icmp(lpszName, _T("spacex")))
		SetSpaceX(_ttoi(lpszValue));
	else if (equal_icmp(lpszName, _T("spacey")))
		SetSpaceY(_ttoi(lpszValue));
	else
		CContainerUI::SetAttribute(lpszName,lpszValue);
}

void CLayoutUI::SetLayoutType(UITYPE_LAYOUT eLayoutType)
{
	m_eLayoutType = eLayoutType;
}

UITYPE_LAYOUT CLayoutUI::GetLayoutType()
{
	return m_eLayoutType;
}

int  CLayoutUI::GetStartX()
{
	return m_nStartX;
}
void CLayoutUI::SetStartX(int nValue)
{
	m_nStartX =nValue;

}

int  CLayoutUI::GetStartY()
{
	return m_nStartY;
}

void CLayoutUI::SetStartY(int nValue)
{
	m_nStartY = nValue;
}

int  CLayoutUI::GetSpaceX()
{
	return m_nSpaceX;

}

void CLayoutUI::SetSpaceX(int nValue)
{
	m_nSpaceX = nValue;
}

int  CLayoutUI::GetSpaceY()
{
	return m_nSpaceY;

}

void CLayoutUI::SetSpaceY(int nValue)
{
	m_nSpaceY = nValue;
}

inline bool is_conntrol_auto_size(CControlUI* pControl)
{
	if (/*pControl->GetCoordinate()->anchor == UIANCHOR_LTRB &&*/ 
		pControl->GetCoordinate()->rect.left  ==  0 &&
		pControl->GetCoordinate()->rect.top  ==  0 &&
		pControl->GetCoordinate()->rect.right  ==  0 &&
		pControl->GetCoordinate()->rect.bottom  ==  0)
		return true;
	return false;

}

bool CLayoutUI::Add(CControlUI* pControlChild)
{
	bool ret = __super::Add(pControlChild);
	if (GetLayoutType() == UILAYOUT_HORZFILL || GetLayoutType() == UILAYOUT_VERTFILL)
	{
		if (is_conntrol_auto_size(pControlChild))
			pControlChild->SetAutoSize(true);
		
	}
	return ret;
}
bool CLayoutUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	bool ret = __super::Insert(pControlChild, pControlAfter);
	if (GetLayoutType() == UILAYOUT_HORZFILL || GetLayoutType() == UILAYOUT_VERTFILL)
	{
		if (is_conntrol_auto_size(pControlChild))
			pControlChild->SetAutoSize(true);


	}
	return ret;
}

SIZE CLayoutUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	SIZE sz = {0, 0};
	CControlUI* pControl = NULL;
	int nIndex = 0;
	int nCount = this->GetCount();
	if (nCount == 0)
		return sz;

	UICustomControlHelper::LayoutParams params;
	params.nStartX_ = DPI_SCALE(m_nStartX);
	params.nStartY_ = DPI_SCALE(m_nStartY);
	params.nSpaceX_ = DPI_SCALE(m_nSpaceX);
	params.nSpaceY_ = DPI_SCALE(m_nSpaceY);
	params.bEnableScroll_ = (GetHScrollbar() || GetVScrollbar());
	CStdPtrArray* pItems = GetItemArray();
	switch(m_eLayoutType)
	{
	case UILAYOUT_LEFTTORIGHT:
		sz = UICustomControlHelper::LayoutChildLeftToRight(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_RIGHTTOLEFT:
		sz = UICustomControlHelper::LayoutChildRightToLeft(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_TOPTOBOTTOM:
		sz = UICustomControlHelper::LayoutChildTopToBottom(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_BOTTOMTOTOP:
		sz = UICustomControlHelper::LayoutChildBottomToTop(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_HORZFILL:
		sz = UICustomControlHelper::LayoutChildHorz(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_VERTFILL:
		sz = UICustomControlHelper::LayoutChildVert(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_GRID:
		sz = UICustomControlHelper::LayoutChildGrid(rcWinow,rcClient, params, *pItems);
		break;
	case UILAYOUT_NORMAL:
		sz = UICustomControlHelper::LayoutChildNormal(rcWinow,rcClient, *pItems, params.bEnableScroll_);
		break;
	default:
		sz = UICustomControlHelper::LayoutChildNormal(rcWinow,rcClient, *pItems, params.bEnableScroll_);
		break;
	}
	return sz;
}

void CLayoutUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	RECT rcItem = GetRect();
	if (IsSizeWithContent())
	{
		if (GetLayoutType() == UILAYOUT_LEFTTORIGHT)
		{
			if (GetCount() > 0)
			{
				CControlUI* pLastChild = GetItem(GetCount() - 1);
				RECT rcLastChild = pLastChild->GetRect();
				RECT rcInset = GetInset();
				DPI_SCALE(&rcInset);
				if (!GetUIEngine()->IsRTLLayout())
					rcItem.right = rcLastChild.right + rcInset.right;
				else
					rcItem.left = rcLastChild.left - rcInset.left;

				CViewUI::SetRect(rcItem);

			}

		}
	}
}


//////////////////////////////////////////////////////////////////////////
COptionLayoutUI::COptionLayoutUI()
{
	SetLayoutType(UILAYOUT_LEFTTORIGHT);
	m_pAssociateControl = NULL;
}

COptionLayoutUI::~COptionLayoutUI()
{

}

void COptionLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("sel")) == 0)
	{
		m_strSelectedId = lpszValue;
	}
	if (_tcsicmp(lpszName, _T("associate")) == 0)
	{
		m_strAssociatedId = lpszValue;
	}
	else __super::SetAttribute(lpszName, lpszValue);
}

bool COptionLayoutUI::Add(CControlUI* pControl)
{
	pControl->OnEvent += MakeDelegate(this, &COptionLayoutUI::OptionControlEvent);
	return __super::Add(pControl);
}

bool COptionLayoutUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
    pControlChild->OnEvent += MakeDelegate(this, &COptionLayoutUI::OptionControlEvent);
    return __super::Insert(pControlChild, pControlAfter);
}

bool COptionLayoutUI::OptionControlEvent(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
			SelectControl(event.pSender);
			if (!m_pAssociateControl && !m_strAssociatedId.empty())
			{
				m_pAssociateControl = GetParent()->GetItem(m_strAssociatedId.c_str());
				if (NULL == m_pAssociateControl)
				{
					CControlUI* pAssociate = GetWindow()->FindControl(m_strAssociatedId.c_str(), NULL);
					if (pAssociate)
					{
						m_pAssociateControl = pAssociate;
					}
				} 
			}
			if (m_pAssociateControl && event.pSender->GetIndex() < m_pAssociateControl->GetCount())
			{
				GetWindow()->LockUpdate();
				for (int i = 0; i <  m_pAssociateControl->GetCount(); i++)
				{
					if (i == event.pSender->GetIndex())	
					{
						m_pAssociateControl->GetItem(i)->SetVisible(true);
						m_pAssociateControl->GetItem(i)->Resize();
					}
					else
					{
						m_pAssociateControl->GetItem(i)->SetVisible(false);
					}
						
				}
				GetWindow()->UnLockUpdate();
				m_pAssociateControl->Invalidate();
			}
		}
		break;
	}
	return true;
}

void COptionLayoutUI::Init()
{
	CControlUI* pControl = GetItem(m_strSelectedId.c_str());
	if (pControl)
		SelectControl(pControl);

}

bool COptionLayoutUI::Event(TEventUI& event)
{

	switch (event.nType)
	{
	case UIEVENT_ITEM_SEL_CHANGE:
		{
			this->SendNotify(UINOTIFY_SELCHANGE, event.wParam, event.lParam);
		}
		break;
	}
	return __super::Event(event);

}

void COptionLayoutUI::Associate(CControlUI* pControl)// 关联其它容器
{
	m_pAssociateControl = pControl; 
}

bool COptionLayoutUI::SelectItem(CControlUI* pControl)
{
	if (!m_pAssociateControl && !m_strAssociatedId.empty())
	{
		m_pAssociateControl = GetParent()->GetItem(m_strAssociatedId.c_str());
		if (NULL == m_pAssociateControl)
		{
			CControlUI* pAssociate = GetWindow()->FindControl( m_strAssociatedId.c_str(), NULL);
			if (pAssociate)
			{
				m_pAssociateControl = pAssociate;
			}
		}
	}

	bool bRet = SelectControl(pControl);
	
	if (m_pAssociateControl && pControl->GetIndex() < m_pAssociateControl->GetCount())
	{
		GetWindow()->LockUpdate();
		for (int i = 0; i <  m_pAssociateControl->GetCount(); i++)
		{
			if (i == pControl->GetIndex())	
			{
				m_pAssociateControl->GetItem(i)->SetVisible(true);
				m_pAssociateControl->GetItem(i)->Resize();
			}
			else
			{
				m_pAssociateControl->GetItem(i)->SetVisible(false);
			}

		}
		GetWindow()->UnLockUpdate();
		m_pAssociateControl->Invalidate();
	}

	return bRet;
}

CControlUI* COptionLayoutUI::GetCurSel()
{
	return __super::GetCurSelControl();
}

CControlUI* COptionLayoutUI::GetAssocicatecurSel()
{
	if (!m_pAssociateControl && !m_strAssociatedId.empty())
	{
		m_pAssociateControl = GetParent()->GetItem(m_strAssociatedId.c_str());
		if (NULL == m_pAssociateControl)
		{
			CControlUI* pAssociate = GetWindow()->FindControl(m_strAssociatedId.c_str(), NULL);
			if (pAssociate)
			{
				m_pAssociateControl = pAssociate;
			}
		}
	}

	CControlUI* pAssocicatecurSel = NULL;
	if (m_pAssociateControl)
	{
		CControlUI* pCurselHeader = GetCurSel();
		if (pCurselHeader)
		{
			int i = pCurselHeader->GetIndex();

			pAssocicatecurSel = m_pAssociateControl->GetItem(i);
		}
	}
	return pAssocicatecurSel;

}

CControlUI* COptionLayoutUI::GetAssocicate()
{
	return m_pAssociateControl;
}
