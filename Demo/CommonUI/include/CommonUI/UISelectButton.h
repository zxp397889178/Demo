#pragma once

class CUI_API CSelectButtonUI
	: public CButtonUI
{
public:
	UI_OBJECT_DECLARE(CSelectButtonUI, _T("SelectButton"));

	CSelectButtonUI(void);
	virtual ~CSelectButtonUI(void);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)override;
	virtual bool Activate()override;
	int GetSelectIndex();
protected:
	void SelectButonState(int nInde);
	int		m_nCurrIndex = 0;
};
