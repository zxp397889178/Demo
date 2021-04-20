#pragma once

class  CUI_API CStringFormator
{
public:
	CStringFormator(LPCTSTR lpctOriginal);
	virtual ~CStringFormator();

	void Bind(LPCTSTR lpctKey, LPCTSTR lpctValue);
	void Bind(LPCTSTR lpctKey, DWORD dwValue);
	LPCTSTR Str();

private:
	tstring m_strGenerate;
};