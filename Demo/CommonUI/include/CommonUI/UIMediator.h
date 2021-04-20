#pragma once

class CUI_API CControlMediatorUI
{
public:
	CControlMediatorUI();
	virtual ~CControlMediatorUI();

public:
	virtual void	OnCtrlInit(CControlUI* pOwner);
	virtual void	OnCtrlDestory();
	virtual bool	OnCtrlEvent(TEventUI& msg);

public:
	CControlUI*		GetCtrl();

protected:
	CControlUI*		m_pOwner;

};


class CUI_API CWindowMediatorUI
{
public:
	CWindowMediatorUI();
	virtual ~CWindowMediatorUI();

public:
	virtual void	OnWndCreate(CWindowUI* pWindow);
	virtual void	OnWndDestory();
	virtual LRESULT	OnWndProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	CWindowUI*		GetWnd();
	void			CloseWnd();

protected:
	CWindowUI*		m_pWnd;
};
