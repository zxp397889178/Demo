#pragma once


class CMainViewWnd : public CWindowUI,
	public INotifyUI
{
public:
	CMainViewWnd();
	virtual ~CMainViewWnd();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_close"), OnBtnClose);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_LoginPanel"), OnBtnLoginPanel);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_reg"), OnBtnReg);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_socketserver"), OnBtnSocketServer);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_socketclient"), OnBtnSocketClient);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_udpserver"), OnBtnUDPServer);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_udpclient"), OnBtnUDPClient);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_fun1"), OnBtnfun1);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_fun2"), OnBtnfun2);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_fun3"), OnBtnfun3);
	UIEND_MSG_MAP

public:
	HWND				CreateWnd(HWND hParent);
	void				OnCreate();
	void				OnClose();

private:
	bool				OnBtnClose(TNotifyUI* msg);
	bool				OnBtnLoginPanel(TNotifyUI* msg);
	bool				OnBtnReg(TNotifyUI* msg);
	bool				OnBtnSocketServer(TNotifyUI* msg);
	bool				OnBtnSocketClient(TNotifyUI* msg);
	bool				OnBtnUDPServer(TNotifyUI* msg);
	bool				OnBtnUDPClient(TNotifyUI* msg);

	bool				OnBtnfun1(TNotifyUI* msg);
	bool				OnBtnfun2(TNotifyUI* msg);
	bool				OnBtnfun3(TNotifyUI* msg);
};