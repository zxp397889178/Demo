#pragma once

class CUserLoginFactory
{
public:
	CUserLoginFactory();
	~CUserLoginFactory();

public:
	
	static DWORD RequestAccLogin(std::string& strUserName, std::string& strEncryptPwd, std::string& strOrganizeName, std::string& strData);
	static bool ParseLogin(std::string& strData, std::string& strUserId, std::string& strAccessToken, std::string& strMacKey, std::string& strServerTime,
		ULONGLONG& tLoginTime, DWORD& dwErrorCode, std::wstring& wstrErrorDescription);


	static DWORD RequestUserInfo(std::string& strUserId, std::string& strAccessToken, std::string& strMacKey, std::string& strServerTime,
		ULONGLONG& tLoginTime, std::string& strData);
	static bool ParseserInfo(std::string& strData, std::shared_ptr<UserInfo>& pUserInfo, DWORD& dwErrorCode, std::wstring& wstrErrorDescription);

};