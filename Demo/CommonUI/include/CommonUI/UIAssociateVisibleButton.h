#pragma once


class CUI_API CAssociateVisibleButtonUI :
	public CButtonUI
{
	UI_OBJECT_DECLARE(CAssociateVisibleButtonUI, _T("AssociateVisibleButton"))
public:
	CAssociateVisibleButtonUI();
	virtual ~CAssociateVisibleButtonUI();

public:
	void			AssociateVisible(CContainerUI* pAssociateParent);

protected:
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

private:
	std::vector<tstring>	m_vctrShowCtrls;
	std::vector<tstring>	m_vctrHideCtrls;
};
