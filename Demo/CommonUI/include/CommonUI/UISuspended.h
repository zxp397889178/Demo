#pragma once


class CWindowMediatorUI;

class CUI_API CSuspendedWindowUI :
	public CNoActiveWindowUI,
	public CWindowPtrRef
{
public:
	CSuspendedWindowUI();
	virtual ~CSuspendedWindowUI();

public:
	void				SetWindowMediator(CWindowMediatorUI* pMediator);

protected:
	virtual void		OnCreate();
	virtual void		OnClose();
	virtual LRESULT		WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


private:
	bool				m_bInited;
	CWindowMediatorUI*	m_pMediator;
};

//////////////////////////////////////////////////////////////////////////


class CControlMediatorUI;
class CUI_API CSuspendedMenuButtonUI :
	public CAutoSizeButtonUI
{
	UI_OBJECT_DECLARE(CSuspendedMenuButtonUI, _T("SuspendedMenuButton"))
public:
	CSuspendedMenuButtonUI();
	virtual ~CSuspendedMenuButtonUI();

public:
	void			SetCtrlMediator(CControlMediatorUI* pMediator);

protected:
	virtual void	Init();

	virtual void	OnDestroy();

private:
	bool				m_bInited;
	CControlMediatorUI*	m_pMediator;

};
