#pragma once

namespace common
{
	vector<string> Split(const string& str, const string& delims = "\t\n ", unsigned int maxSplits = 0);
	void PasswordCrypt(const char* pszPassword, unsigned char* pszEncryptPassword);

	std::string Base64Encode(void* pData, unsigned long dwSize);
	std::string Base64Decode(const char* encoded_string);
	std::string SHA256Array(std::string& strData, std::string& strKey);
	std::string	GetNonce(time_t tServerTime);
	std::string generate_random_str(size_t strSize);

	void PasswordCrypt(const char* pszPassword, unsigned char* pszEncryptPassword);
	void DESEncrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey, unsigned char* pszEncryptText, DWORD dwEncryptSize);
	void DESDecrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey, unsigned char* pszDecryptText, DWORD dwDecryptSize);
	void GenerateMacToken(const char* pszHost, const char* pszUri, const char* pszMethod, const char* pszAccessToken, const char* pszMacKey, time_t tServerTime,
		unsigned char* pszOutText, DWORD dwOutSize);

	ULONGLONG ExchangeServerTime(string& strServerTime);
	bool CreateDirectories(const wstring& path);


	//获取注册表值
	bool GetRegisterStringValue(LPCTSTR lpszRegFullPath, tstring& strValue, bool b64Bit = false);
	bool SetRegisterStringValue(LPCTSTR lpszRegFullPath, tstring strValue, bool b64Bit = false);
	bool SetRegisterStringValue(HKEY hRootKey, LPCTSTR lpszPath, LPCTSTR lpszkey, LPCTSTR lpszValue, bool b64Bit = false);
}