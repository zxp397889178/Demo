/*********************************************************************
 *       Copyright (C) 2011,应用软件开发
 **********************************************************************
 *   Date             Name                 Description
 *   2011-06-07       hanzp
*********************************************************************/
#pragma once

#ifndef __DIRECTUI_CONTROLWND_H_
#define __DIRECTUI_CONTROLWND_H_

/*!
    \brief    句柄绑定控件
*****************************************/
class DUI_API CControlWndUI : public CControlUI
{
	//{{
	UI_OBJECT_DECLARE(CControlWndUI, _T("ControlWnd"))
public:
	CControlWndUI(void);
	virtual ~CControlWndUI(void);
	//}}

	virtual void SetRect( RECT& rectRegion );

	virtual void SetVisible( bool bShow);
	virtual void SetInternVisible( bool bVisible );

	virtual void Render( IRenderDC* pRenderDC, RECT& rcPaint );

	void DockWnd(HWND hWnd);
	void ShowWnd(bool bShow);
protected:
	void OffsetWnd(bool bShow, bool bChangeRect);
	bool m_bShow;
	HWND m_hWindow;
	HWND m_hWindowParent;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
};

#endif //__DIRECTUI_CONTROLWND_H_