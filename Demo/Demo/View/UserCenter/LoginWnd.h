#pragma once


class CLoginWnd : public CWindowUI,
	public INotifyUI, 
	public CEventObserver<UserCenterLoginComplete>,
	public CTargetEventObserver<CompleteLastLoginInfo, HWND>,
	public CTargetEventObserver<CompleteSelectUserInfo, HWND>
{
public:
	CLoginWnd();
	virtual ~CLoginWnd();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_close_login"), OnBtnCloseLogin);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_login"), OnBtnLogin);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_login"), OnBtnCancelLogin);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_loginout"), OnBtnLoginOut);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_hide_pwd"), OnBtnHidePassword);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_show_pwd"), OnBtnShowPassword);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_pull_down"), OnBtnPullDown);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_drop_down"), OnBtnDropDown);
	UIEND_MSG_MAP

public:
	HWND				CreateWnd(HWND hParent);
	void				OnCreate();
	void				OnClose();

private:
	virtual void		OnEvent(UserCenterLoginComplete* pEvent);
	virtual void		OnEvent(CompleteLastLoginInfo* pEvent);
	virtual void		OnEvent(CompleteSelectUserInfo* pEvent);

	virtual HWND		GetTargetId();

private:
	bool				OnBtnCloseLogin(TNotifyUI* msg);
	bool				OnBtnLogin(TNotifyUI* msg);
	bool				OnBtnCancelLogin(TNotifyUI* msg);
	bool				OnBtnLoginOut(TNotifyUI* msg);
	bool				OnBtnHidePassword(TNotifyUI* msg);
	bool				OnBtnShowPassword(TNotifyUI* msg);
	bool				OnBtnPullDown(TNotifyUI* msg);
	bool				OnBtnDropDown(TNotifyUI* msg);

	bool				OnAccountEditNotify(TNotifyUI& notify);
	bool				OnPwdEditNotify(TNotifyUI& notify);

private:
	void				SwitchContent(bool bLogining);
	void				SetUserInfo(std::shared_ptr<UserInfo>& pUserInfo);

private:
	bool				m_bIsLogin = false;
	tstring				m_strEncrptPwd;

	CContainerUI*		m_pLoginContent = NULL;
	CEditLayoutUI*		m_pEditAccount = NULL;
	CEditLayoutUI*		m_pEditPwd = NULL;
	CLabelUI*			m_pLoginTip = NULL;
	CContainerUI*		m_pLoginingContent = NULL;
	CLayoutUI*			m_pLoginFrameA = NULL;
	CLayoutUI*			m_pUserInfoPanel = NULL;
	CLabelUI*			m_pLoginError = NULL;

	CButtonUI*			m_pBtnHidePwd = NULL;
	CButtonUI*			m_pBtnShowPwd = NULL;
	CButtonUI*			m_pBtnPullDown = NULL;
	CButtonUI*			m_pBtnDropDown = NULL;

	//UserInfoPanel
	CLabelUI*			m_pUserInfoId = NULL;
	CLabelUI*			m_pUserInfoUserName = NULL;
	CLabelUI*			m_pUserInfoRealName = NULL;
	CLabelUI*			m_pUserInfoNickName = NULL;
	CLabelUI*			m_pUserInfoBirthday = NULL;
	CLabelUI*			m_pUserInfoSex = NULL;

};