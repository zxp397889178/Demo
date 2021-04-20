#pragma once

class CUcPullDownWnd :
	public CWindowUI,
	public INotifyUI,
	public CEventObserver<CompletePullDownInfo>
{
public:
	CUcPullDownWnd();
	virtual ~CUcPullDownWnd();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("pd_user"),	OnBtnCheckUser)
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("pd_del"),	OnBtnCheckDel)
	UIEND_MSG_MAP

public:
	HWND				CreateWnd(HWND hParent, CLayoutUI *pEdit);

protected:
	virtual void		OnCreate();
	virtual LRESULT		WindowProc(UINT message, WPARAM wParam, LPARAM lParam); //窗口过程函数

private:
	bool				OnBtnCheckUser(TNotifyUI* msg);
	bool				OnBtnCheckDel(TNotifyUI* msg);

private:
	virtual void	OnEvent(CompletePullDownInfo* pEvent);
};