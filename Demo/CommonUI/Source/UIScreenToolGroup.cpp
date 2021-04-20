#include "stdafx.h"
#include "UIScreenToolGroup.h"

CScreenToolGroupUI::CScreenToolGroupUI()
{
	m_pGroupTypeName = NULL;
	m_pItemList      = NULL;
	m_pLayoutEmpty   = NULL;
	m_nCategoryId    = 0;
}

CScreenToolGroupUI::~CScreenToolGroupUI()
{

}

void CScreenToolGroupUI::Init()
{
	m_pItemList      = dynamic_cast<CLayoutUI*>(FindSubControl(_T("lay_itemList")));
	m_pGroupTypeName = FindSubControl(_T("lab_TypeName"));
	m_pLayoutEmpty   = dynamic_cast<CLayoutUI*>(FindSubControl(_T("lay_empty")));
}

LPCTSTR CScreenToolGroupUI::GetGroupName()
{
	if (!m_pGroupTypeName)
	{
		return NULL;
	}

	return m_pGroupTypeName->GetText();
}

void CScreenToolGroupUI::SetGroupName(LPCTSTR lstrGroupName)
{
	if (lstrGroupName
		&& m_pGroupTypeName)
	{
		m_pGroupTypeName->SetText(lstrGroupName);
	}	
}

void CScreenToolGroupUI::SetGroupId(int nCategory)
{
	m_nCategoryId = nCategory;
}

int CScreenToolGroupUI::GetGroupId()
{
	return m_nCategoryId;
}

void CScreenToolGroupUI::AddItem(CControlUI* pItem)
{
	
}

CLayoutUI* CScreenToolGroupUI::GetItemList()
{
	return m_pItemList;
}

CLayoutUI* CScreenToolGroupUI::GetEmptyContent()
{
	return m_pLayoutEmpty;
}

void CScreenToolGroupUI::SetItemListVisible(bool bVisible)
{
	if (m_pItemList)
	{
		m_pItemList->SetVisible(bVisible);
	}
}

void CScreenToolGroupUI::SetEmptyContentVisible(bool bVisible)
{
	if (m_pLayoutEmpty)
	{
		m_pLayoutEmpty->SetVisible(bVisible);
	}
}


