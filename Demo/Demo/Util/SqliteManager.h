#pragma once

class CSqliteManager
{
public:
	CSqliteManager();
	~CSqliteManager();
	static CSqliteManager* GetInstance();

public:
	void Initialize();

	sqlite3pp::database& GetDemoDB();

	bool SqlitExcute(sqlite3pp::database& db, string strSql);
	bool SqlitInsert(sqlite3pp::database& db, string strSql, int nCount, ...);
private:
	sqlite3pp::database m_DemoDB;
	string m_strDemoDB = "demo.db";

private:
	static CSqliteManager* m_pInstance;
};