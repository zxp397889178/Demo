/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIRadio.h"


//////////////////////////////////////////////////////////////////////////


CRadioUI::CRadioUI()
{
	m_strGroupName = _T("");
	SetStyle(_T("Radio"));
}

CRadioUI::~CRadioUI()
{

}


void CRadioUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//重载基类
{

	if (equal_icmp(lpszName, _T("group")))
	{
		m_strGroupName = lpszValue;
	}
	else
	{
		CCheckBoxUI::SetAttribute(lpszName, lpszValue);
	}
}



void CRadioUI::SetChecked( bool bChecked, UINT uFlags /*= UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY*/ )
{
	if (bChecked)
	{
		CControlUI* pParent = GetParent();
		if (pParent)
		{
			CRadioUI* pRadio = NULL;
			int nCount = pParent->GetCount();
			for (int i = 0; i < nCount; i++)
			{
				CControlUI* pControl = pParent->GetItem(i);
				if (this != pControl && dynamic_cast<CRadioUI*>(pControl))
				{
					pRadio = static_cast<CRadioUI*>(pControl);
					if (pRadio->m_strGroupName == m_strGroupName && pRadio->IsChecked())
					{
						pRadio->SetChecked(false);
						pRadio->SendNotify(UINOTIFY_SELCHANGE, (WPARAM)false);
						break;
					}

				}
			}
		}
	}

	if (IsSelected() != bChecked)
	{
		Select(bChecked);
		Invalidate();
		if (uFlags & UIITEMFLAG_NOTIFY)
		{
			SendNotify(UINOTIFY_SELCHANGE, (WPARAM)bChecked);
		}
	}
}

bool CRadioUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONUP:
		{
			if (!IsSelected())
			{
				SetChecked(true);
				this->SendNotify(UINOTIFY_CLICK,IsSelected());
			}
		}
		return true;
	default:
		return CCheckBoxUI::Event(event);
	}
}
