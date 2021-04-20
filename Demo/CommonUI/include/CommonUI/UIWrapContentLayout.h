#pragma once

//Ŀǰ֧�� UIANCHOR_RB UIANCHOR_LB UIANCHOR_LT

class CUI_API CWrapContentLayoutUI :
	public CLayoutUI
{
	UI_OBJECT_DECLARE(CWrapContentLayoutUI, _T("WrapContentLayout"))
public:
	CWrapContentLayoutUI();
	virtual ~CWrapContentLayoutUI();

protected:
	virtual void	Init();
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual SIZE	OnChildSize(RECT& rcWinow, RECT& rcClient);
	virtual	void	CalcChildRect();

	//Ŀǰ������
	virtual void	SetInternVisible(bool bVisible);
	virtual void	SetVisible(bool bVisible);

private:
	SIZE		m_sizeChilds;
	bool		m_bResized;
	bool		m_bAutoWrap;
};
