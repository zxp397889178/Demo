/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UICheckBox.h"

//////////////////////////////////////////////////////////////////////////
CCheckBoxUI::CCheckBoxUI()
:m_bPartialChecked(false)
{
	SetStyle(_T("CheckBox"));
}

void CCheckBoxUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("check")))
	{
		Select(!!_ttoi(lpszValue));
	}
	else
		CButtonUI::SetAttribute(lpszName,lpszValue);
}

bool CCheckBoxUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	//case UIEVENT_DBLCLICK: //modify by hanzp qc:17818
	case UIEVENT_BUTTONUP:
		{
			SetChecked(!IsChecked());
		}
		return true;
	case UIEVENT_KEYDOWN:
		{
			if (event.wParam == VK_SPACE
				|| event.wParam == VK_RETURN)
			{
				SetChecked(!IsChecked());
			}
		}
		return true;
	default:
		return CButtonUI::Event(event);
	}
}

bool CCheckBoxUI::IsChecked()
{
	return IsSelected();
}

void CCheckBoxUI::SetChecked( bool bChecked, UINT uFlags /*= UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY*/ )
{
	if (uFlags & UIITEMFLAG_CHECKED)
	{
		if (IsSelected() != bChecked)
		{
			Select(bChecked);
			if (uFlags & UIITEMFLAG_NOTIFY)
				this->SendNotify(UINOTIFY_CLICK, (WPARAM)IsSelected());
		}
	}
}

bool CCheckBoxUI::Indef() const
{
	return m_bPartialChecked;
}

void CCheckBoxUI::Indef( bool val )
{
	m_bPartialChecked = val;
	StyleObj* pStyle = GetStyle(_T("bk"));
	if (pStyle)
	{
		pStyle->SetVisible(!m_bPartialChecked);
	}
	pStyle = GetStyle(_T("indefbk"));
	if (pStyle)
	{
		pStyle->SetVisible(m_bPartialChecked);
	}
	Invalidate();
}
