#include "stdafx.h"
#include "Utils/Macros.h"
#include "../NDHttp/NDHttp.h"
#include "Event\LoginEvent.hpp"
#include "Event\UserCenterEventModel.hpp"
#include "UserCenterBiz.h"
#include "Util\SqliteManager.h"

#define Table_Name  "login"
#define  DLOGIN_USER_NAME			("user_name")
#define  DLOGIN_USER_PASSWORD		("user_password")
#define  DLOGIN_AVATAR_PATH			("avatar_path")

CUserCenterBiz::CUserCenterBiz()
{
	
}

CUserCenterBiz::~CUserCenterBiz()
{

}

void CUserCenterBiz::OnInit()
{
	CSqliteManager::GetInstance()->Initialize();
}

bool CUserCenterBiz::CreateLoginTable()
{
	std::string strSql = "create table if not exists ";
	strSql += Table_Name;
	strSql += " (user_name char(128) primary key, user_password char(128), avatar_path char(512),time timestamp not null default (datetime('now','localtime')))";

	bool bRet = CSqliteManager::GetInstance()->SqlitExcute(CSqliteManager::GetInstance()->GetDemoDB(), strSql.c_str());
	return bRet;
}

bool CUserCenterBiz::InsertLoginToDBank(LoginParam &stLoginParam)
{
	CreateLoginTable();

	bool bRet = false;
	std::string	strUserName, strUserPwd, strAvatarPath;
	strUserName = UnicodeToUtf8(stLoginParam.strUserName.c_str());
	strUserPwd = UnicodeToUtf8(stLoginParam.strUserPassword.c_str());

	string strTabelName = Table_Name;

	do
	{
		char szSql[256];
		_snprintf_s(szSql, _countof(szSql) - 1, "select count(1) from %s where user_name = '%s';", strTabelName.c_str(), strUserName.c_str());

		int iCount = 0;
		try
		{
			sqlite3pp::query qry(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
			bRet = DBCountResult(qry, iCount);
		}
		catch (...)
		{

		}
		if (!bRet)
		{
			break;
		}

#define ReplaceInto()\
	_snprintf_s(szSql, _countof(szSql) - 1, "replace into %s (user_name,user_password,avatar_path) VALUES (:user_name,:user_password,:avatar_path);", strTabelName.c_str()); \
	CSqliteManager::GetInstance()->SqlitInsert(CSqliteManager::GetInstance()->GetDemoDB(), szSql, 3, \
	strUserName.c_str(), strUserPwd.c_str(), strAvatarPath.c_str()); 


		memset(szSql, 0, sizeof(szSql));
		if (iCount > 0)//账户存在直接更新
		{
			LoginParam stLoginParamTmp;
			GetUserFromDBank(stLoginParam.strUserName, stLoginParamTmp);
			strAvatarPath = UnicodeToUtf8(stLoginParam.strAvatarPath.c_str());

			ReplaceInto();
			break;
		}

		_snprintf_s(szSql, _countof(szSql) - 1, "select count(1) from %s;", strTabelName.c_str());
		iCount = 0;
		try
		{
			sqlite3pp::query qry(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
			bRet = DBCountResult(qry, iCount);
		}
		catch (...)
		{

		}
		if (!bRet)
		{
			break;
		}

		memset(szSql, 0, sizeof(szSql));
		if (iCount < 3)	 //账户数据小于3条，插入新账户数据
		{
			ReplaceInto();
			break;
		}

		//账户数据大于3条，删除最早一条数据，并且保存新数据账号
		_snprintf_s(szSql, _countof(szSql) - 1, "delete from %s where time = (select min(time) from %s);", strTabelName.c_str(), strTabelName.c_str());
		bRet = CSqliteManager::GetInstance()->SqlitExcute(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
		if (!bRet)
		{
			break;
		}

		memset(szSql, 0, sizeof(szSql));
		ReplaceInto();

	} while (const_expression_cast(false));
	return bRet;
}

bool CUserCenterBiz::GetUserFromDBank(const tstring &strAcct, LoginParam &stUserInfo)
{
	char szSql[256];
	std::string strTableName = Table_Name;
	std::string	strUserName;
	strUserName = UnicodeToUtf8(strAcct.c_str());
	_snprintf_s(szSql, _countof(szSql) - 1, "select * from %s where user_name = '%s';", strTableName.c_str(), strUserName.c_str());

	vector<LoginParam> vLoginParam;
	bool bRet = false;
	try
	{
		sqlite3pp::query qry(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
		bRet = DBSelectResult(qry, vLoginParam);
	}
	catch (...)
	{

	}
	if (vLoginParam.size() > 0)
	{
		stUserInfo.strUserName = vLoginParam[0].strUserName;
		stUserInfo.strUserPassword = vLoginParam[0].strUserPassword;
		stUserInfo.strAvatarPath = vLoginParam[0].strAvatarPath;
	}
	return bRet/*false*/;
}

bool CUserCenterBiz::GetAllLoginFromDBank(vector<LoginParam> &vLoginParam)
{
	char szSql[256];
	std::string strTableName =Table_Name;
	_snprintf_s(szSql, _countof(szSql) - 1, "select * from %s;", strTableName.c_str());

	bool bRet = false;
	try
	{
		sqlite3pp::query qry(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
		bRet = DBSelectResult(qry, vLoginParam);
	}
	catch (...)
	{

	}
	return bRet;
}

bool CUserCenterBiz::DBSelectResult(sqlite3pp::query& qry, vector<LoginParam>& vLoginParam)
{
	bool bRet = false;
	try
	{
		sqlite3pp::query::iterator iter = qry.begin();
		for (; iter != qry.end(); ++iter)
		{
			LoginParam stLoginParam;
			int nCount = qry.column_count();
			for (int i = 0; i < nCount; ++i)
			{
				bRet = true;
				string strColumn = qry.column_name(i);

				if (strcmp(DLOGIN_USER_NAME, strColumn.c_str()) == 0)
				{
					if ((*iter).get<const char*>(i))
					{
						stLoginParam.strUserName = Utf8ToUnicode((*iter).get<const char*>(i));
					}
				}
				else if (strcmp(DLOGIN_USER_PASSWORD, strColumn.c_str()) == 0)
				{
					if ((*iter).get<const char*>(i))
					{
						stLoginParam.strUserPassword = Utf8ToUnicode((*iter).get<const char*>(i));
					}
				}
				else if (strcmp(DLOGIN_AVATAR_PATH, strColumn.c_str()) == 0)
				{
					if ((*iter).get<const char*>(i))
					{
						stLoginParam.strAvatarPath = Utf8ToUnicode((*iter).get<const char*>(i));
					}
				}
			}
			vLoginParam.push_back(stLoginParam);
		}
	}
	catch (...)
	{
		bRet = false;
	}
	return bRet;
}

bool CUserCenterBiz::DBCountResult(sqlite3pp::query& qry, int& nCount)
{
	bool bRet = false;
	try
	{
		int nSize = qry.column_count();
		for (int i = 0; i < nSize; ++i)
		{
			bRet = true;
			sqlite3pp::query::iterator iter = qry.begin();
			if (iter != qry.end())
			{
				if ((*iter).get<const char*>(i))
					nCount = atoi((*iter).get<const char*>(i));
			}
		}
	}
	catch (...)
	{
		bRet = false;
	}
	return bRet;
}

void CUserCenterBiz::OnEventInBackground(const UcRequestAccSaveInfo* pEvent)
{
	if (pEvent)
	{
		LoginParam stLoginParam;
		stLoginParam.strUserName = pEvent->wstrUserName;
		stLoginParam.strUserPassword = pEvent->wstrUserPwd;
		InsertLoginToDBank(stLoginParam);
	}
}

void CUserCenterBiz::OnEventInBackground(const UcRequestSelectUserInfo* pEvent)
{
	if (pEvent)
	{
		LoginParam stLoginParam;
		GetUserFromDBank(pEvent->wstrUserName, stLoginParam);
		CompleteSelectUserInfo userInfo;
		userInfo.m_stUserInfo = stLoginParam;
		userInfo.PostToTarget(pEvent->hwnd);
	}
}

void CUserCenterBiz::OnEventInBackground(const UcRequestLastLoginInfo* pEvent)
{
	if (!pEvent)
	{
		return;
	}

	vector<LoginParam> vLoginParam;
	GetAllLoginFromDBank(vLoginParam);

	CompleteLastLoginInfo LogInfo;
	LogInfo.nInfoCount = (int)vLoginParam.size();
	if (LogInfo.nInfoCount > 0)
	{
		LogInfo.stLoginParam = vLoginParam.back();
	}

	LogInfo.PostToTarget(pEvent->hWnd);
}

void CUserCenterBiz::OnEventInBackground(const UcRequestPullDownInfo* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	CompletePullDownInfo LogInfo;
	GetAllLoginFromDBank(LogInfo.m_vLoginParam);
	LogInfo.Post();
}

void CUserCenterBiz::OnEventInBackground(const UcRequestDelUserInfo* pEvent)
{
	if (pEvent)
	{
		CompleteDelUserInfo stDelUserInfo;
		stDelUserInfo.wstrUserName = pEvent->wstrUserName;
		DeletelLoginFromDBank(pEvent->wstrUserName);
		stDelUserInfo.Post();
	}
}

bool CUserCenterBiz::DeletelLoginFromDBank(const tstring &tstrUserName)
{
	char szSql[256];
	std::string strTableName = Table_Name;
	std::string	strUserName;
	strUserName = UnicodeToUtf8(tstrUserName.c_str());
	_snprintf_s(szSql, _countof(szSql) - 1, "delete from %s where user_name = '%s';", strTableName.c_str(), strUserName.c_str());

	bool bRet = CSqliteManager::GetInstance()->SqlitExcute(CSqliteManager::GetInstance()->GetDemoDB(), szSql);
	return bRet;
}
