#include "stdafx.h"

#include "UISelectButton.h"



CSelectButtonUI::CSelectButtonUI(void)
{
	SetStyle(_T("SelectButton"));
}

CSelectButtonUI::~CSelectButtonUI(void)
{
}

void CSelectButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(_T("CurrIndex"), lpszName) == 0)
	{
		m_nCurrIndex = _ttoi(lpszValue);
		if (m_nCurrIndex > 1)
		{
			m_nCurrIndex = 1;
		}
		SelectButonState(m_nCurrIndex);
		return;
	}
	__super::SetAttribute(lpszName, lpszValue);
}

bool CSelectButtonUI::Activate()
{
	m_nCurrIndex = m_nCurrIndex == 1 ? 0 : 1;
	SelectButonState(m_nCurrIndex);
	return __super::Activate();
}

int CSelectButtonUI::GetSelectIndex()
{
	return m_nCurrIndex;
}

void CSelectButtonUI::SelectButonState(int nInde)
{
	auto pLeft = GetImageStyle(_T("bk_left"));
	auto pRight = GetImageStyle(_T("bk_right"));
	auto pLeftText = GetTextStyle(_T("txt_LT"));
	auto pRightText = GetTextStyle(_T("txt_RT"));
	if (pLeft && pRight && pLeftText && pRightText)
	{
		pLeft->SetVisible(!nInde);
		pRight->SetVisible(!!nInde);
		pLeftText->SetVisible(!nInde);
		pRightText->SetVisible(!!nInde);
	}
}
