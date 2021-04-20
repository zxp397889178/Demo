/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************
*   Date             Name                 Description
*   2010-11-26       hanzp				  Create.
*********************************************************************/
#pragma once



class DUI_API CTabHeaderUI : public CLayoutUI
{
	UI_OBJECT_DECLARE(CTabHeaderUI, _T("TabHeader"))
public:
	//{{
	CTabHeaderUI();
	virtual ~CTabHeaderUI();
	//}}

	virtual CControlUI*  GetCurSel(){return m_pCurSel;}
	virtual bool SelectItem(CControlUI* pControl);

	virtual void RemoveAll();
	virtual bool Remove(CControlUI* pControl);
	virtual bool Add(CControlUI* pControl);

	//{{
protected:

private:
	bool ChildEvent(TEventUI& event);
	CControlUI* m_pCurSel;

	//}}
};

class DUI_API CTabButtonUI : public CButtonUI
{
	friend class CTabHeaderUI;
	UI_OBJECT_DECLARE(CTabButtonUI, _T("TabButton"))
public: 
	//{{
	CTabButtonUI(void);
	virtual ~CTabButtonUI(void);
	//}}

	bool IsShowClose(){return m_bShowClose;}
	void ShowClose(bool bShow);
	void SetCloseState(UITYPE_STATE nCloseState){m_nCloseState = nCloseState;}
	UITYPE_STATE GetCloseState(){return m_nCloseState;}
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	//{{
protected:
	//virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	UITYPE_STATE m_nCloseState;
	bool m_bShowClose;
	StyleObj* GetBkStyle();
	StyleObj* GetIconStyle();
	StyleObj* GetTextStyle();
	StyleObj* GetCloseStyle();

	StyleObj* m_pBkStyle;
	StyleObj* m_pIconStyle;
	StyleObj* m_pTextStyle;
	StyleObj* m_pCloseStyle;

	//}}
};

/*!
    \brief    tab控件
*****************************************/
class DUI_API CTabUI : public CContainerUI
{
	UI_OBJECT_DECLARE(CTabUI, _T("TabCtrl"))
public:
	//{{
	CTabUI(void);
	virtual ~CTabUI(void);
	//}}
public:
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual bool Add(CControlUI* pControl);
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll();
	bool		SelectItem(int nIndex);
	bool		SelectItem(LPCTSTR szID);
	int			GetCurSel();
	virtual void SetRect(RECT& rect);
	CControlUI*	GetHeader();

	bool		SetTabHeaderVisible(int nIndex, bool bVisible = true);             //设置Tab选项卡是否可见
	bool		SetTabHeaderVisible(LPCTSTR lpszPageID, bool bVisible = true);     //设置Tab选项卡是否可见
	bool		IsHeaderItemVisible(int nIndex);
	void		SetTabHeaderAttribute(LPCTSTR lpszPageID, LPCTSTR lpszName, LPCTSTR lpszValue);
	void		SetTabHeaderAttribute(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue);
	void		SetHeaderInitPos(int l, int t, int r, int b, UITYPE_ANCHOR a);
	void		SetBodyInitPos(int l, int t, int r, int b, UITYPE_ANCHOR a);
	//{{
protected:
	virtual void OnLanguageChange();
 	virtual void Init();
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	//}}
	Coordinate  m_cdHeader;
private:
	CTabHeaderUI*	m_pHeaderList;
	bool		m_bHeaderHide;
	bool		m_bHeaderVisible;
	tstring		m_szCurSel;
	int m_nMaxWidth;
	int m_nMinWidth;
	Coordinate  m_cdBody;
};

