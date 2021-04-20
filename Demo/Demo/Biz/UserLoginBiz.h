#pragma once

class CUserLoginBiz :
	public CAsycEventObserver<UserCenterRequestAccLogin>,
	public CEventObserver<UserCenterRequestCancelLogin>,
	public CAsycEventObserver<UserCenterRequestLogout>
{
public:
	CUserLoginBiz();
	~CUserLoginBiz();
	ImplementInstance(CUserLoginBiz)

	void	OnInit();

	void	CancelLogin()	{ bCanceled = false; }
	bool	IsLogining()	{ return bLogining; }
	bool	IsCancelLogin()	{ return bCanceled; }
	bool	IsOfflineLogined()	{ return bOfflineLogined; }

    UCSDK::UCManager* GetUCManager();
private:
	//event
	virtual void OnEventInBackground(const UserCenterRequestAccLogin* pEvent);
	virtual void OnEvent(UserCenterRequestCancelLogin* pEvent);
	virtual void OnEventInBackground(const UserCenterRequestLogout* pEvent);

	void LoginBiz(std::shared_ptr<UserInfo> pUserInfo, const std::shared_ptr<vector<StudentInfo>>& pvecStudentInfo, DWORD& dwErrorCode, std::wstring& wstrErrorDescription);


private:
	bool 	bLogining;			// 登录中
	bool    bCanceled = false;	// 已取消
	bool    bOfflineLogined = false;	// 离线登录

    UCSDK::UCManager* m_pUCManager = NULL;
    std::string m_strOrganize;
	bool		m_bVirtualOrg = false;
	std::string m_strOrganizeId;
};