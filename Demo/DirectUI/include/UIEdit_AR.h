#pragma once
//#define DIRECTUI_LAYOUT_RTL
//#ifdef DIRECTUI_LAYOUT_RTL

#include "UIToolTip.h"

///////////////////////////////////////////////////////
class DUI_API CEditArUI:public CLabelUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CEditArUI, _T("EditAr"))
public:
	CEditArUI();
	~CEditArUI();

	void SetText(LPCTSTR lpszText);
	virtual tstring GetPasswordText();
	virtual void SetTextWithoutNotify(LPCTSTR lpszText);  //���SearchEdit ������˺���������text�󲻶���緢����Ϣ��
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	bool IsReadOnly();
	void SetReadOnly(bool bReadOnly=true);
	virtual void SelAllText();
	virtual bool SetSelPos(int pos = -1);
	virtual void GetSel(int& nStartChar, int& nEndChar) const;

	static void EnableSystemMenu(bool bEnableSystemMenu);

	virtual bool IsMouseWhellEnabled();

	RECT		 GetFlipInterRect();
	// ��������
protected:
	virtual void Init();
	virtual void SetRect(RECT& rect);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual void OnSetFocus();
	virtual void OnKillFocus();
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnPaste(){}
	virtual bool OnLimitChar(UINT nChar);
	virtual bool Notify(TNotifyUI* msg);
	virtual bool OnMenuItemClick(TNotifyUI* pNotify);

	// �븸�๲�б���
protected:
	TextStyle* GetEditStyle();
	TextStyle* GetEmptyEditStyle();
	// �븸�๲�б���
protected:
	bool m_bInit;
	TextStyle* m_pTextStyle;
	TextStyle* m_pEmptyTextStyle;
	bool m_bTransparent;	//0 ��͸�� 1 ͸��
	bool m_bPassword;
	bool m_bPasswordVisiblity;
	bool m_bAutoSize;
	bool m_bMaxSize;
	SIZE m_cxyMaxSize;
	bool m_bReadOnly;
	HWND m_hParentWnd;
	bool m_bMenuEnable;
	tstring m_strEmptyTipsText;
	LPTSTR	m_lpszEmptyTipsKey;//����m_strEmptyTipsText��,���ڶ������л�
	int m_nMaxchar;
	LONG  m_tmHeight;

private:
	UIBEGIN_MSG_MAP
		UI_EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT, OnMenuItemClick)
	UIEND_MSG_MAP

	void HideToolTip();
	void Paste();
	void Copy();
	void Cut();
	bool IsBindTextHost() const;
	bool m_bRTLReading;
	static bool m_bEnableSystemMenu;
};

//#endif 
