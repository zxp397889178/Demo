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
	bool		CreateLoginTable(); //������¼���
	bool		InsertLoginToDBank(LoginParam &stLoginParam);//�����ݿ��в����¼��
	bool		GetUserFromDBank(const tstring &strAcct, LoginParam &stUserInfo);	//��������
	bool		GetAllLoginFromDBank(vector<LoginParam> &vLoginParam);//��ȡȫ������
	bool        DeletelLoginFromDBank(const tstring &tstrUserName);


	static bool	DBSelectResult(sqlite3pp::query& qry, vector<LoginParam>& vLoginParam);
	static bool	DBCountResult(sqlite3pp::query& qry, int& nCount);
};