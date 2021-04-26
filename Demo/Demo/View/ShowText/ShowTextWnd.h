#pragma  once

class CShowTextWnd : public CWindowUI
{
public:
	CShowTextWnd();
	virtual ~CShowTextWnd();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_close"), OnBtnClose);
	UIEND_MSG_MAP

public:
	HWND CreateWnd(HWND hParent);



private:
	bool OnBtnClose(TNotifyUI* msg);
};