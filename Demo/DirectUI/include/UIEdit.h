/*********************************************************************
*            Copyright (C) 2010, �����������Ӧ�������
**********************************************************************
*   Date        Name        Description
*   9/17/2010   hanzp		Create.
*   9/20/2010   hanzp		Add Class CEditUI.
*   10/27/2010  hanzp		Add MultiLineEdit
*********************************************************************/
#pragma once

class DUI_API CMultiLineEditUI:public CRichEditUI
{
	UI_OBJECT_DECLARE(CMultiLineEditUI, _T("MultiEdit"))
public:	
	CMultiLineEditUI();
	~CMultiLineEditUI();
	LPCTSTR GetText();	
	void SetText(LPCTSTR lpszText);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

protected:
	virtual void Init();
	TextStyle* GetEditStyle();	
	TextStyle* GetEmptyEditStyle();
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual bool Event(TEventUI& event);

private:
	TextStyle* m_pTextStyle;
	bool m_bTransparent;
	TextStyle* m_pEmptyTextStyle;
	tstring m_strEmptyTipsText;
};



#include "UIEdit_AR.h"

#ifndef DIRECTUI_LAYOUT_RTL
//////////////////////////////////////////////////////////////////////////
/*!
    \brief    ���б༭��ؼ�
*****************************************/
class DUI_API CEditUI:public CEditArUI
{
	UI_OBJECT_DECLARE(CEditUI, _T("Edit"))
public:
	CEditUI();
	virtual ~CEditUI();
	void SetText(LPCTSTR lpszText);
	tstring GetPasswordText();
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	void SetTextWithoutNotify(LPCTSTR lpszText);

	void SelAllText();
	bool SetSelPos(int pos = -1);
	void GetSel(int& nStartChar, int& nEndChar) const;

	virtual void SetInternVisible(bool bVisible);
	virtual void SetVisible(bool bVisible); // ���ؿؼ��ɼ���

public:
	//bool IsReadOnly();
	//void SetReadOnly(bool bReadOnly = true);
	// ����๲���麯��
protected:
	virtual void Init();
	virtual void SetRect(RECT& rectRegion);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual void OnSetFocus();
	virtual void OnKillFocus();
	virtual void OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnPaste();
	virtual bool OnLimitChar(UINT nChar);
	virtual	bool OnMenuItemClick(TNotifyUI* pNotify);
protected:
	virtual void OnLanguageChange();
	int GetLeftClientWidthIndex(HDC hDC, int nPos);
	int GetRightClientWidthIndex(HDC hDC, int nPos);
	LPCTSTR GetDrawText(tstring& strTempUse);
	void DrawRange(IRenderDC* pRenderDC, const unsigned &index1, const unsigned &index2);
	tstring m_strPasswdText;//������Ĥ���ַ�����������������һֱ���������ŵ���m_tcPassChar�ַ�
	tstring m_strRef;
	int m_uSelBegin;
	int m_uSelEnd;
	bool m_bCaretEnable;
private:
	UIBEGIN_MSG_MAP
		UI_EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT,  OnMenuItemClick)
	UIEND_MSG_MAP
	void SetFirstVisiblePixel(int nPixel);
	void SetPreSelEndPixel(int nPixel);
	void SetPrevSelEndInfo();//���Թ����ж�λʱ���� ����m_uPrevSelEnd; m_nPrevSelEndPixel;����
	BOOL EnsureSelEndVisible();//���µ���m_nFirstVisiblePixel��С����ʹ���λ�ÿɼ�
	int Point2Index(LPPOINT ppt, bool bAdjust=false);
	POINT Index2Point(int index);
	int GetTextPixel();
	int	DelSelText();
	RECT GetSelectReDrawRect(unsigned beforeMovePos); 

	void Copy();
	void Paste();
	void HideToolTip();
	void SetStyleObjRect(const RECT& rc);
	tstring GetSelText();
	int Insert(LPCTSTR lpszText);
	
	int GetCharsPixels(HDC hDC, int nCounts);
	int GetCharsCounts(HDC hDC, int nPixels, bool bAdjust=false);

	void SetPrevSelEndInfo(int offsetPixels); //�����в������ɾ��ʱ, ��m_uPrevSelEnd; m_nPrevSelEndPixel;���ô��ݵĲ����ǵ���insert��DelSelRange�ķ���ֵ��
	RECT GetInsertReDrawRect(unsigned beforeInsertPos);
	RECT GetDeleteReDrawRect();
	void CreateCaret();
	void SetCaretPos();
	void DestroyCaret();
	
	
	int m_uPrevSelEnd;
	HWND m_hToolTipWnd;
	TCHAR m_tcPassChar;//��������������������ַ�
	
	int m_nPasswdCharWidth;
	int m_nAveCharWidth;
	int m_nLineHeight;
	int m_cy;
	RECT m_rcPrevItem;
	int m_nTextPixel;//�ܵ��ı��ַ���ռ�����ش�С֮��
	int m_nFirstVisiblePixel;
	int m_nPrevSelEndPixel;
	int m_nMaxUTF8Char;
	int m_nCurUTF8Char;
	
	bool m_bUseCss;


//	// ����๲�б����Ƶ�����
//private:
//	TextStyle* GetEditStyle();	
//	TextStyle* GetEmptyEditStyle();
//
//	// ����๲�б����Ƶ�����
//private:
//	bool m_bReadOnly;// ��Ҫ
//	bool m_bAutoSize;// ��Ҫ
//	bool m_bMenuEnable;// ��Ҫ
//	bool m_bTransparent;// ��Ҫ
//	bool m_bPassword;// ��Ҫ
//	bool m_bInit; // ��Ҫ
//	bool m_bMaxSize;// ��Ҫ
//	TextStyle* m_pTextStyle;// ��Ҫ
//	TextStyle* m_pEmptyTextStyle;// ��Ҫ
//	tstring m_strEmptyTipsText;// ��Ҫ
//	SIZE m_cxyMaxSize;// ��Ҫ
//	HWND m_hParentWnd;// ��Ҫ
//  int m_nMaxchar;
//	// ������˽�к����ͱ���
};

class DUI_API CCssEditUI :public CEditUI
{
	UI_OBJECT_DECLARE(CCssEditUI, _T("CssEdit"))
public:
	CCssEditUI();
	~CCssEditUI();
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	void SetEmptyText(const tstring & sEmptyText);
	virtual void OnSetFocus();
	
private:
	enum { MAXLINK = 8 };
	int m_iLinks;
	int m_iHoverLink;
	RECT m_rcLinks[MAXLINK];
	tstring m_strLinks[MAXLINK];
	tstring m_sHoverLink;
	DWORD m_dwHAlign;
	DWORD m_dwVAlign;
	DWORD m_dwSingleLine;

};
#endif 

