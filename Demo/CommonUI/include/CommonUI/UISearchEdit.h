#pragma once

class CSearchEditUI;

class CSearchToolWindowUI : public CNoActiveWindowUI
{
public:
	CSearchToolWindowUI(CSearchEditUI* pOwner);
	virtual ~CSearchToolWindowUI();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("list"), OnClickItem)
		UI_EVENT_ID_HANDLER(UINOTIFY_RCLICK, _T("list"), OnClickItem)
		UI_EVENT_HANDLER(UINOTIFY_EN_KEYDOWN, OnKeyDown);
	UIEND_MSG_MAP

public:
	bool OnKeyDown(TNotifyUI* pNotify);
	bool OnClickItem(TNotifyUI* pNotify);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CSearchEditUI* m_pOwner;
};

class CUI_API CSearchEditUI : public CEditExUI
{
	//{{
	friend class CSearchToolWindowUI;
	UI_OBJECT_DECLARE(CSearchEditUI, _T("SearchEdit"))
public:
	CSearchEditUI(void);
	virtual ~CSearchEditUI();
	//}}

	// singleton
	static CSearchEditUI* gSearchEditUI;

	CListUI* GetList();
	void ShowSearch();
	void HideSearch();
	virtual bool Add(CControlUI* pControl);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetVisible( bool bShow);
	virtual void SetInternVisible( bool bVisible );
	//{{

	void SetIgnoreUinotifyEnChange(bool flag) { m_bIgnoreUinotifyEnChange = flag; }
	bool GetIgnoreUinotifyEnChange() { return m_bIgnoreUinotifyEnChange; }
	bool MouseHitTest(WPARAM wParam, LPARAM lParam);

protected:
	void ReleaseWnd();
	void InitWnd();
	virtual void HideAuto();
	virtual void OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);

	virtual bool Event(TEventUI& event);
	virtual bool ChildEvent(TEventUI& event);
	virtual void SetRect(RECT& rectRegion);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	CStdPtrArray m_items;
	CNoActiveWindowUI* m_pToolWindow;
	CListUI* m_pList;
	bool m_bDown;
	bool m_bFirstButtonDown;
	bool m_bHideAuto;
	int m_nMaxItems;
	//}}

	bool m_bIgnoreUinotifyEnChange;
};

