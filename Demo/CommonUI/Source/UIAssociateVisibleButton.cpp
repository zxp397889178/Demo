/*********************************************************************
 *       Copyright (C) 2011,应用软件开发
*********************************************************************/
#include "StdAfx.h"
#include "UIAssociateVisibleButton.h"
#include "Common/StringOperation.h"




CAssociateVisibleButtonUI::CAssociateVisibleButtonUI()
{

}

CAssociateVisibleButtonUI::~CAssociateVisibleButtonUI()
{

}

void CAssociateVisibleButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("showctrls")) == 0)
	{
		_tcssplit(m_vctrShowCtrls, lpszValue, _T("|"));
	}
	else if (_tcsicmp(lpszName, _T("hidectrls")) == 0)
	{
		_tcssplit(m_vctrHideCtrls, lpszValue, _T("|"));
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void CAssociateVisibleButtonUI::AssociateVisible(CContainerUI* pAssociateParent)
{
	if (!pAssociateParent)
	{
		return;
	}

	for (size_t i = 0; i < m_vctrShowCtrls.size(); ++i)
	{
		CControlUI* pCtrl = pAssociateParent->FindSubControl(m_vctrShowCtrls.at(i).c_str());
		if (!pCtrl)
		{
			continue;
		}

		pCtrl->SetVisible(true);
	}

	for (size_t i = 0; i < m_vctrHideCtrls.size(); ++i)
	{
		CControlUI* pCtrl = pAssociateParent->FindSubControl(m_vctrHideCtrls.at(i).c_str());
		if (!pCtrl)
		{
			continue;
		}

		pCtrl->SetVisible(false);
	}
}
