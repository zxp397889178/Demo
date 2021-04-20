/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2013-03-27       xqb
*********************************************************************/
#ifndef __UICARET_H_
#define __UICARET_H_
#include "UITimer.h"
#include "UIWindow.h"

class CCaretUI
{
public:
	static CCaretUI* GetInstance();
	static void ReleaseInstance();
public:
	BOOL SetCaretBlinkTime(__in UINT uMSeconds);
	UINT GetCaretBlinkTime();
	BOOL CreateCaret(CWindowUI* pWindow, __in_opt HBITMAP hBitmap, __in int nWidth, __in int nHeight);
	BOOL SetCaretPos(__in int X, __in int Y);
	BOOL HideCaret();
	BOOL ShowCaret();
	BOOL DestroyCaret(CWindowUI* pWindow);

	BOOL GetCaretPos(LPPOINT lpPoint);
	void Render(IRenderDC* pRenderDC, RECT& rcUpdate);
private:
	CCaretUI();
	~ CCaretUI();

	void OnTimer(CUITimerBase* pTimer);
	CTimerSource m_objTimerSource;
	CWindowUI* m_pWindow;
	HWND m_hWnd;
	HBITMAP m_hBitmap;
	RECT m_rcItem;
	BOOL m_bBlinkShow;
	DibObj m_dibObj;
	bool m_bVisible;
	static CCaretUI* m_pCaretUI;
};

#endif