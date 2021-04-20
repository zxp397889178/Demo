#include "stdafx.h"
#include "Utils/Macros.h"
#include "Event/UserCenterEventModel.hpp"
#include "Event\LoginEvent.hpp"
#include "../NDHttp/NDHttp.h"
#include "UserLogin.h"
#include <random>
#include "Util/Common.h"
#include "openssl/hmac.h"

using namespace rapidjson;
using namespace rjwrapper;

DWORD CUserLoginFactory::RequestAccLogin(std::string& strUserName, std::string& strEncryptPwd, std::string& strOrganizeName, std::string& strData)
{
	string strUrl = "/v0.93/tokens";

	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();
	Value root(kObjectType);

	root.AddMember("login_name", strUserName, allocator);
	root.AddMember("password", strEncryptPwd, allocator);
	root.AddMember("org_name", strOrganizeName, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string str = buffer.GetString();

	std::string strHost = "aqapi.101.com";
	CHttpRequestDataModel Model;
	Model.strHost = strHost.c_str();
	Model.strUrl = strUrl;
	Model.strMethod = "POST";
	Model.nPort = 443;
	Model.strHeader = "Content-Type: application/json";
	Model.strHeader += "\r\nsdp-app-id:b4fb92a0-af7f-49c2-b270-8f62afac1133";
	Model.strHeader += "\r\nUC-COLL: Õ¯¡˙99”Œ/V8.9.0.1112(Microsoft Windows 10;;1680*1050;;94-DE-80-AE-B3-03;ND--20170124ZZS)";
	Model.strPost = str.c_str();

	HttpRequestData* pRequest = RequestDataInit(&Model);
	if (!pRequest)
	{
		return -1;
	}

	DWORD dwRet = pRequest->Excute();
	if (dwRet == 0 && pRequest->GetLength() > 0)
	{
		strData = pRequest->GetResult();
	}
	CleanUpData(pRequest);

	return dwRet;
}

bool CUserLoginFactory::ParseLogin(std::string& strData, std::string& strUserId, std::string& strAccessToken, std::string& strMacKey, std::string& strServerTime,
	ULONGLONG& tLoginTime, DWORD& dwErrorCode, std::wstring& wstrErrorDescription)
{
	Document document;
	if (document.Parse((char *)strData.c_str()).HasParseError())
	{
		dwErrorCode = -1;
		wstrErrorDescription = _T("JsonΩ‚Œˆ ß∞‹");
		return false;
	}

	string strCode = GetStringByKey(document, "code");
	if (!strCode.empty())
	{
		dwErrorCode = -2;
		wstrErrorDescription = Utf8ToUnicode(GetStringByKey(document, "message").c_str());
		return false;
	}

	strUserId = lexical_nothrow_cast<string, ULONGLONG>(GetUint64ByKey(document, "user_id"));;
	strMacKey = GetStringByKey(document, "mac_key");
	strAccessToken = GetStringByKey(document, "access_token");
	strServerTime = GetStringByKey(document, "server_time");
	tLoginTime = GetTickCount();

	return true;
}

DWORD CUserLoginFactory::RequestUserInfo(std::string& strUserId, std::string& strAccessToken, std::string& strMacKey, std::string& strServerTime,
	ULONGLONG& tLoginTime, std::string& strData)
{
	string strUrl = "/v0.93/users/";
	strUrl += strUserId;
	strUrl += "?realm=uc.sdp.nd";

	std::string strHost = "aqapi.101.com";
	CHttpRequestDataModel Model;
	Model.strHost = strHost.c_str();
	Model.strUrl = strUrl;
	Model.strMethod = "GET";
	Model.nPort = 443;
	Model.strHeader = "Accept: application/json";
	Model.strHeader += "\r\nsdp-app-id:b4fb92a0-af7f-49c2-b270-8f62afac1133";

	try
	{
		unsigned char szMacToken[1024] = { 0 };
		common::GenerateMacToken(strHost.c_str(), strUrl.c_str(), Model.strMethod.c_str(), strAccessToken.c_str(),
			strMacKey.c_str(), tLoginTime, szMacToken, sizeof(szMacToken)-1);
		Model.strHeader += "\r\nAuthorization: ";
		Model.strHeader += (char *)szMacToken;
	}
	catch (...)
	{
		
	}

	HttpRequestData* pRequest = RequestDataInit(&Model);
	if (!pRequest)
	{
		return -1;
	}

	DWORD dwRet = pRequest->Excute();
	if (dwRet == 0 && pRequest->GetLength() > 0)
	{
		strData = pRequest->GetResult();
	}
	CleanUpData(pRequest);

	return dwRet;
}

bool CUserLoginFactory::ParseserInfo(std::string& strData, std::shared_ptr<UserInfo>& pUserInfo, DWORD& dwErrorCode, std::wstring& wstrErrorDescription)
{
	tstring wstrData = Utf8ToUnicode(strData.c_str());
	Document document;
	if (document.Parse((char *)strData.c_str()).HasParseError())
	{
		dwErrorCode = -1;
		wstrErrorDescription = _T("JsonΩ‚Œˆ ß∞‹");
		return false;
	}

	string strCode = GetStringByKey(document, "code");
	if (!strCode.empty())
	{
		dwErrorCode = -2;
		wstrErrorDescription = Utf8ToUnicode(GetStringByKey(document, "message").c_str());
		return false;
	}

	pUserInfo->wstrNickName = Utf8ToUnicode(GetStringByKey(document, "nick_name").c_str());
	pUserInfo->wstrBirthday = Utf8ToUnicode(GetStringByPointer(document, "/org_exinfo/birthday").c_str());

	return true;
}
