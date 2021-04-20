#include "stdafx.h"
#include "UIAutoSizeButton.h"
#include "UIAutoSizeCheckBox.h"

//////////////////////////////////////////////////////////////////////////
CAutoSizeCheckBoxUI::CAutoSizeCheckBoxUI()
:m_bPartialChecked(false)
{
	SetStyle(_T("CheckBox"));
}

void CAutoSizeCheckBoxUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("check")) == 0)
	{
		Select(!!_ttoi(lpszValue));
	}
	else
		CButtonUI::SetAttribute(lpszName,lpszValue);
}

bool CAutoSizeCheckBoxUI::Event(TEventUI& event)
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

bool CAutoSizeCheckBoxUI::IsChecked()
{
	return IsSelected();
}

void CAutoSizeCheckBoxUI::SetChecked( bool bChecked, UINT uFlags /*= UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY*/ )
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

bool CAutoSizeCheckBoxUI::Indef() const
{
	return m_bPartialChecked;
}

void CAutoSizeCheckBoxUI::Indef( bool val )
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
