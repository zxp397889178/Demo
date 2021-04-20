#pragma once

class DUI_API CObjectUI:public CControlUI
{
	//{{
public:
	CObjectUI(void);
	virtual ~CObjectUI(void);
	//}}
	static CControlUI* CreateObject();
	void Docked(HWND hWnd);
	virtual void SetRect(RECT& rectRegion);
	//{{
private:
	HWND m_hWnd;
	//}}
};
