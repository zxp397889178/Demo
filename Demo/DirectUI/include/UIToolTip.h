/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************
*   Date             Name                 Description
*   2010-12-15       hanzp				  Create.
*********************************************************************/
#pragma once


//BL bottom-left
//BR bottom-right
//LB left-bottom
//LT left-top
//RB right-bottom
//RT right-top
//TL top-left
//TR top-right
//{{
typedef STDMETHODIMP ToolTip_Callback(CWindowUI* pWindow, WPARAM wParam, LPARAM lParam);
//}}
/*!
    \brief    带指向的tooltip
*****************************************/
class DUI_API CToolTipUI : public CNoActiveWindowUI
{
public:
	//{{
	CToolTipUI(LPCTSTR lpszContent, UITYPE_TOOLTIP type = UITOOLTIP_Info, HWND hParentWnd = HWND_DESKTOP);
	~CToolTipUI();
	static CToolTipUI* GetInstance();
	static CToolTipUI* GetInstance(LPCTSTR lpszContent, UITYPE_TOOLTIP type = UITOOLTIP_Info);
	//}}

	void OnShowTimer(CUITimerBase* pTimer);

	void SetLockTimer(bool bLock);
	void SetPointTo(POINT pt);
	void SetAlign(LPCTSTR lpszAlign);
	void SetWidth(int nWidth);
	void SetElapse(int nElapse);
	void SetContent(LPCTSTR lpszContent);
	void SetToolTipType(UINT uType) {m_uIconType = uType;}
	void ShowTootipWindow(int nCmdShow = SW_SHOWNOACTIVATE);
	//{{
	void SetCallBack(ToolTip_Callback* pCallBack, WPARAM wParam, LPARAM lParam);
protected:
	tstring  GetModeValueFromStr(LPCTSTR lpszValue);
	void ChangePosFromStr(LPCTSTR lpszValue);
private:
	ToolTip_Callback* m_pCallBack;//关闭前的回调
	WPARAM m_pCallBackWParam;
	LPARAM m_pCallBackLParam;
	POINT m_ptPointTo;
	RECT m_rcOffset;
	UINT m_nMaxWidth;
	UINT m_nElapse;
	UINT m_uIconType;
	bool m_bLockTimer;
	CTimerSource m_objTimer;
	//}}
};


class TipWindowImpl;
/*!
    \brief    系统提示框
*****************************************/
class DUI_API TipWindow : public CWindowBase
{
public:
	//{{
	TipWindow();
	~TipWindow();
	//}}
	bool CreateTip(HWND hParentWnd, int nMaxWidth = 260);
	void ShowTip(bool bShow, LPCTSTR lpszText = _T(""), LPRECT pRect = NULL);
	LRESULT Send( UINT Msg, WPARAM wParam = NULL, LPARAM lParam = NULL );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	//{{
	TipWindowImpl* m_pTipWindowImpl;
	//}}
};

