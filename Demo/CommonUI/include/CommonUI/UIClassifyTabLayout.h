#pragma once

class CUI_API CClassifyTabLayoutUI :
	public CLayoutUI
{
	UI_OBJECT_DECLARE(CClassifyTabLayoutUI, _T("ClassifyTabLayout"))

public:
	CClassifyTabLayoutUI();
	~CClassifyTabLayoutUI();

protected:
	virtual void		Init();

public:
	virtual bool		Add(CControlUI* pControlChild);
	virtual bool		Insert(CControlUI* pControlChild, CControlUI* pControlAfter);
	
	COptionLayoutUI*	GetOptionLayout();
	CLayoutUI *			GetControlLayout();
    bool				SelectItem(CControlUI* pControl);
    void				SelectItem(int iIndex);
	CControlUI*			GetCurSel();
	CControlUI*			GetAssocicatecurSel();
	CControlUI*			GetAssocicate(); 
	LPCTSTR				GetResourseType();//返回选中项的ID
	void				UpdateTabButton();
	void				ShowShade(bool bShow);
//	virtual int			GetCount();
// 	virtual CControlUI* GetItem(int iIndex);
// 	virtual CControlUI* GetItem(LPCTSTR lpszId);

protected:
	virtual bool	OnTabLeftNotify(TNotifyUI& msg);
	virtual bool	OnTabRightNotify(TNotifyUI& msg);
	virtual bool	OnOptionLayoutEvent(TNotifyUI& msg);
	virtual bool	OnOptionLayoutNotify(TNotifyUI& msg);
	virtual bool    OnScrollPosChange(TEventUI &evt);

private:
	COptionLayoutUI * m_pOptionLayout;
	CLayoutUI		* m_pTabControlLayout;
	CButtonUI *		  m_pBtnTabLeft;
	CButtonUI *		  m_pBtnTabRight;

};