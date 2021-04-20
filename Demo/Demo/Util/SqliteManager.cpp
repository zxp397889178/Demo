#include "stdafx.h"
#include "SqliteManager.h"
#include "Common.h"

CSqliteManager*  CSqliteManager::m_pInstance = NULL;
CSqliteManager::CSqliteManager()
{
	
}

CSqliteManager::~CSqliteManager()
{

}

CSqliteManager* CSqliteManager::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CSqliteManager;
	}
	return m_pInstance;
}

void CSqliteManager::Initialize()
{
	TCHAR wszAppDataPath[MAX_PATH * 2] = { 0 };
	GetEnvironmentVariable(_T("APPDATA"), wszAppDataPath, sizeof(wszAppDataPath));
	std::wstring wstrPath = wszAppDataPath;
	wstrPath += _T("\\101PPT\\");
	string strPath = UnicodeToUtf8(wstrPath.c_str());
	common::CreateDirectories(wstrPath);

	auto InitDB = [&](sqlite3pp::database& db, std::string& strDBName){
		try
		{
			string strDB = strPath + strDBName;
			db.connect(strDB.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX);
			db.set_busy_timeout(0x7FFFFFFF);
		}
		catch (...)
		{

		}
	};

	InitDB(m_DemoDB, m_strDemoDB);
}

sqlite3pp::database& CSqliteManager::GetDemoDB()
{
	return m_DemoDB;
}


bool CSqliteManager::SqlitExcute(sqlite3pp::database& db, string strSql)
{
	int rc = SQLITE_OK;

	try
	{
		rc = db.execute(strSql.c_str());
	}
	catch (...)
	{
		rc = SQLITE_ERROR;
	}

	if (rc == SQLITE_OK)
		return true;

	return false;
}

bool CSqliteManager::SqlitInsert(sqlite3pp::database& db, string strSql, int nCount, ...)
{
	int rc = SQLITE_OK;

	try
	{
		vector<std::string> vecKeys;
		vector<std::string> vecParams;
		va_list arg_ptr;
		va_start(arg_ptr, nCount);

		std::string str;
		for (int i = 0; i < nCount; i++)
		{
			str = va_arg(arg_ptr, LPSTR);
			vecParams.push_back(str);
		}

		int nPos = strSql.find("VALUES");
		if (nPos != string::npos)
		{
			string strKeys = strSql.substr(nPos);
			strKeys = strKeys.substr(strKeys.find("(") + 1);
			strKeys = strKeys.substr(0, strKeys.rfind(")"));
			vecKeys = common::Split(strKeys, ",");
		}

		if (vecKeys.size() != nCount)
		{
			return false;
		}

		sqlite3pp::command cmd(db, strSql.c_str());
		for (int i = 0; i < nCount; i++)
		{
			string strKey = vecKeys[i];
			strKey.erase(0, strKey.find_first_not_of(" "));
			strKey.erase(strKey.find_last_not_of(" ") + 1);
			cmd.bind(strKey.c_str(), vecParams[i].c_str(), sqlite3pp::nocopy);
		}
		rc = cmd.execute();
	}
	catch (...)
	{
		rc = SQLITE_ERROR;
	}

	if (rc == SQLITE_OK)
		return true;

	return false;
}