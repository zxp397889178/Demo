#pragma once

class CUserCenterBiz :
	public CAsycEventObserver<UcRequestAccSaveInfo>
	, public CAsycEventObserver<UcRequestSelectUserInfo>
	, public CAsycEventObserver<UcRequestLastLoginInfo>
	, public CAsycEventObserver<UcRequestPullDownInfo>
	, public CAsycEventObserver<UcRequestDelUserInfo>
{
public:
	CUserCenterBiz();
	~CUserCenterBiz();
	ImplementInstance(CUserCenterBiz)

	void	OnInit();

private:
	virtual void OnEventInBackground(const UcRequestAccSaveInfo* pEvent);
	virtual void OnEventInBackground(const UcRequestSelectUserInfo* pEvent);
	virtual void OnEventInBackground(const UcRequestLastLoginInfo* pEvent);
	virtual void OnEventInBackground(const UcRequestPullDownInfo* pEvent);
	virtual void OnEventInBackground(const UcRequestDelUserInfo* pEvent);

protected:
	bool		CreateLoginTable(); //创建登录项表
	bool		InsertLoginToDBank(LoginParam &stLoginParam);//往数据库中插入登录项
	bool		GetUserFromDBank(const tstring &strAcct, LoginParam &stUserInfo);	//单调数据
	bool		GetAllLoginFromDBank(vector<LoginParam> &vLoginParam);//获取全部数据
	bool        DeletelLoginFromDBank(const tstring &tstrUserName);


	static bool	DBSelectResult(sqlite3pp::query& qry, vector<LoginParam>& vLoginParam);
	static bool	DBCountResult(sqlite3pp::query& qry, int& nCount);
};