#pragma once

/*!
    \brief    容器布局
*****************************************/
class DUI_API CLayoutUI: public CContainerUI
{
	UI_OBJECT_DECLARE(CLayoutUI, _T("Layout"))
public:
	CLayoutUI();
	virtual ~CLayoutUI();

public:
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	void SetLayoutType(UITYPE_LAYOUT eLayoutType);
	UITYPE_LAYOUT GetLayoutType();

	int  GetStartX();
	void SetStartX(int nValue);

	int  GetStartY();
	void SetStartY(int nValue);

	int  GetSpaceX();
	void SetSpaceX(int nValue);

	int  GetSpaceY();
	void SetSpaceY(int nValue);

	virtual bool Add(CControlUI* pControlChild);
	virtual bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);

	virtual void SetRect(RECT& rectRegion);
protected:
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);

private:
	UITYPE_LAYOUT m_eLayoutType;
	int m_nStartX;
	int m_nStartY;
	int m_nSpaceX;
	int m_nSpaceY;
	//}}
};

/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class DUI_API COptionLayoutUI : public CLayoutUI
{
	UI_OBJECT_DECLARE(COptionLayoutUI, _T("OptionLayout"))
		COptionLayoutUI();
	~COptionLayoutUI();
public:
	void Associate(CControlUI* pControl); // 关联其它容器
	bool SelectItem(CControlUI* pControl);
	CControlUI* GetCurSel();
	CControlUI* GetAssocicatecurSel();
	CControlUI* GetAssocicate();
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	bool Add(CControlUI* pControl);
	bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);

protected:
	virtual bool Event(TEventUI& event);
	virtual void Init();

private:
	bool OptionControlEvent(TEventUI& event);
	CControlUI* m_pAssociateControl;
	tstring m_strSelectedId;
	tstring m_strAssociatedId;
};


