/*********************************************************************
 *       Copyright (C) 2011,应用软件开发
*********************************************************************/
#include "StdAfx.h"
#include "UIWrapContentLayout.h"


CWrapContentLayoutUI::CWrapContentLayoutUI()
{
	m_bResized = false;
	m_bAutoWrap = true;
}

CWrapContentLayoutUI::~CWrapContentLayoutUI()
{

}

void CWrapContentLayoutUI::Init()
{
	__super::Init();

}

void CWrapContentLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("autowrap")) == 0)
	{
		m_bAutoWrap = !!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
	
}

SIZE CWrapContentLayoutUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	if (!m_bAutoWrap)
	{
		return __super::OnChildSize(rcWinow, rcClient);
	}
	
	m_sizeChilds = __super::OnChildSize(rcWinow, rcClient);
	return m_sizeChilds;
}

void CWrapContentLayoutUI::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);
}

void CWrapContentLayoutUI::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);
}

void CWrapContentLayoutUI::CalcChildRect()
{
	__super::CalcChildRect();

	bool bVisible = IsVisible() && IsInternVisible();
	if (!bVisible)
	{
		return;
	}

	if (m_bResized)
	{
		m_bResized = false;
		return;
	}
	m_bResized = true;

	SIZE size = m_sizeChilds;

	bool bAutoFitHeight = false;
	bool bAutoFitWidth = false;

	switch (GetLayoutType())
	{
		case UILAYOUT_NORMAL:
		case UILAYOUT_GRID:
			{
				bAutoFitHeight = true;
				bAutoFitWidth = true;
			}
			break;
		case UILAYOUT_LEFTTORIGHT:
		case UILAYOUT_RIGHTTOLEFT:
		case UILAYOUT_HORZFILL:
			{
				bAutoFitWidth = true;
			}
			break;
		case UILAYOUT_TOPTOBOTTOM:
		case UILAYOUT_BOTTOMTOTOP:
		case UILAYOUT_VERTFILL:
			{
				bAutoFitHeight = true;
			}
			break;
	default:
		break;
	}

	tstring strId = GetId();

	RECT rcItem = this->GetInterRect();
	switch (this->GetAnchor())
	{
		case UIANCHOR_LT:
			{
				if (bAutoFitHeight)
				{
					rcItem.bottom = rcItem.top + size.cy;
				}

				if (bAutoFitWidth)
				{
					rcItem.right = rcItem.left + size.cx;
				}
			}
			break;
		case UIANCHOR_RB:
			{
				if (bAutoFitHeight)
				{
					rcItem.top = rcItem.bottom - size.cy;
				}

				if (bAutoFitWidth)
				{
					rcItem.left = rcItem.right - size.cx;
				}
			}
			break;
		case UIANCHOR_LB:
			{
				if (bAutoFitHeight)
				{
					rcItem.top = rcItem.bottom - size.cy;
				}

				if (bAutoFitWidth)
				{
					rcItem.right = rcItem.left + size.cx;
				}

			}
			break;
		default:

			break;
	}

	SetRect(rcItem);
}
