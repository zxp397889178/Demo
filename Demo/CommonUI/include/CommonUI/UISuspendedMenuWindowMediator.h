#pragma once

//暂时不能参加CommonUI工程编译，这边先引用编译
class CSuspendedMenuWindowMediator:
	public CWindowMediatorUI,
	public IMessageFilterUI,
	public CTargetEventObserver<RequestCloseSuspendedMenuToTarget, HWND>
{
public:
	CSuspendedMenuWindowMediator();
	virtual ~CSuspendedMenuWindowMediator();

public:
	virtual void		OnWndCreate(CWindowUI* pWindow);
	virtual void		OnWndDestory();
	virtual LRESULT		OnWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	CControlUI*			GetTargetCtrl();

protected:
	void				AttachMessageFilter(HWND hWnd);
	void				DetachMessageFilter(HWND hWnd);
	void				OnMouseLeave();

private:
	//IMessageFilterUI
	virtual LRESULT		MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual void		OnEvent(RequestCloseSuspendedMenuToTarget* pEvent);
	virtual HWND		GetTargetId();

private:
	// Can not ensure the pointer validate.just for target
	CControlUI*		m_pTargetCtrl;
	HWND			m_hTargetWnd;
};
