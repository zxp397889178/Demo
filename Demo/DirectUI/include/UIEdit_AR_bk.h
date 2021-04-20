#pragma once

/*#ifdef DIRECTUI_LAYOUT_RTL*/

#if 0
#include "UIToolTip.h"

class DUI_API CMultiLineEditUI:public CRichEditUI
{
	UI_OBJECT_DECLARE(CMultiLineEditUI, _T("MultiEdit"))
public:	
	CMultiLineEditUI();
	~CMultiLineEditUI();
	LPCTSTR GetText();	
	void SetText(LPCTSTR lpszText);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);


	// 阿语新增的
	void SetTextColor(DWORD dwClrText);
	void SetFontName(LPCTSTR lpszFontName);
	void SetFontSize(int nFontSize);
	void SetBold(bool bBold);
	void SetItalic(bool bItalic);
	void SetUnderline(bool bUnderline);
	void SetAlign(UINT uAlignStyle);
	bool IsReadOnly(){return false;};
	void SetReadOnly(bool bReadOnly){};

protected:
	// 阿语新增的
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual bool Event(TEventUI& event);
	virtual void SetRect(RECT& rectRegion);
	virtual void OnKeyDown(UINT nChar, UINT nFlags);
	virtual void OnChar(UINT nChar, UINT nFlags);
	virtual void OnLButtonDown(UINT nFlags, POINT pt);
	virtual void OnRButtonUp(UINT nFlags, POINT pt);
	virtual void OnLButtonUp(UINT nFlags, POINT pt);
	virtual void OnLButtonDblClk(UINT nFlags, POINT pt);
	virtual void OnMouseMove(UINT nFlags, POINT pt);
	virtual void OnPaste();
	virtual bool ProcessNotify(TNotifyUI* pNotify);


	BOOL EnsureSelEndVisible(){return true;};//重新调整m_nFirstVisiblePixel大小，以使光标位置可见
	int GetTextPixel(){return 0;};
	void SetPreSelEndPixel(int nPixel){};
	void SetFirstVisiblePixel(int nPixel){};
	void SetPrevSelEndInfo(){};//当对光标进行定位时调用 设置m_uPrevSelEnd; m_nPrevSelEndPixel;变量
	int Point2Index(LPPOINT ppt, bool bAdjust=false){return 0;};
	bool IsCaretEnable(){return true;};
	RECT GetSelectReDrawRect(unsigned beforeMovePos){RECT rect = {0}; return rect;}; 

protected:
	virtual void Init();
	TextStyle* GetEditStyle();	

private:
	TextStyle* m_pTextStyle;
	bool m_bInit;
	bool m_bReadOnly;
};


///////////////////////////////////////////////////////

class DUI_API CEditUI:public CLabelUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CEditUI, _T("Edit"))
public:
	CEditUI();
	~CEditUI();

	void SetText(LPCTSTR lpszText);
	void SetTextWithoutNotify(LPCTSTR lpszText);  //配合SearchEdit 特增添此函数，设置text后不对外界发送消息。
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	bool IsReadOnly();
	void SetReadOnly(bool bReadOnly=true);
	bool SetSelPos(int pos=-1);
	void SelAllText();

	// 阿新增的
	tstring GetPasswordText();
protected:
	UIBEGIN_MSG_MAP
		EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT,  OnMenuItemClick)
	UIEND_MSG_MAP

	bool OnMenuItemClick(TNotifyUI* pNotify);

	virtual void Init();
	virtual void SetRect(RECT& rect);
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual bool Event(TEventUI& event);
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
	virtual void Notify(TNotifyUI* msg);
	virtual bool IsKillFocus();

	TextStyle* GetEditStyle();
	TextStyle* GetEmptyEditStyle();
	bool IsBindTextHost();
	void GetSel(int& nStartChar, int& nEndChar);
	void HideToolTip();
	void Paste();
	void Copy();
	void Cut();

private:
	TextStyle* m_pTextStyle;
	TextStyle* m_pEmptyTextStyle;

	bool m_bInit;
	bool m_bTransparent;	//0 不透明 1 透明
	bool m_bMenuEnable;
	bool m_bPassword;
	tstring m_strEmptyTips;
	bool m_bAutoSize;
	bool m_bMaxSize;
	SIZE m_cxyMaxSize;
	bool m_bRTLReading;
	bool m_bhasCaret;
	bool m_bReadOnly;
	HWND m_hParentWnd;
};

#endif 
