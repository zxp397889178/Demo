#include "stdafx.h"
#include "Utils/Macros.h"
#include "../NDHttp/NDHttp.h"
#include "Event\LoginEvent.hpp"
#include "UserLoginBiz.h"
#include <algorithm>
#include "UserLogin.h"

CUserLoginBiz::CUserLoginBiz()
{
	
}

CUserLoginBiz::~CUserLoginBiz()
{
	if (m_pUCManager)
		m_pUCManager->Release();
}

void CUserLoginBiz::OnInit()
{
	bLogining = false;
	bOfflineLogined = false;
	GetUCManager();

}

UCSDK::UCManager* CUserLoginBiz::GetUCManager()
{
	if (!m_pUCManager)
	{
		m_pUCManager = UCSDK::InitUCManager();
		if (m_pUCManager)
		{
			m_strOrganizeId = "481036505701";
			m_strOrganize = "v_org_k12_prod";
			m_bVirtualOrg = true;
		}
	}
	return m_pUCManager;
}

void CUserLoginBiz::OnEventInBackground(const UserCenterRequestAccLogin* pEvent)
{
	if (bLogining || bCanceled)
		return;

	UserCenterLogining LoginingResponse;
	LoginingResponse.Send();

	bOfflineLogined = false;
	bLogining = true;
	UserCenterLoginComplete Response;
	Response.stLoginExtra = pEvent->stLoginExtra;
	Response.tBeginTime = pEvent->tBeginTime;
	std::string strUserName, strEncryptPwd, strOrganize;
	strUserName = UnicodeToUtf8(pEvent->wstrUserName.c_str());
	strEncryptPwd = UnicodeToUtf8(pEvent->wstrPassword.c_str());

	std::string strOrganizeId = m_strOrganizeId;
	bool bVirtualOrg = m_bVirtualOrg;
	//支持nd 中国好老师
	vector<string> vecs;
	strsplit(&vecs, strUserName.c_str(), "@");
	if (vecs.size() >= 2)
	{
		std::string strTemp = vecs[vecs.size() - 1];
		transform(strTemp.begin(), strTemp.end(), strTemp.begin(), tolower);
		if (m_strOrganize == strTemp)
		{
			size_t nPos = strUserName.rfind("@");
			if (nPos != string::npos)
			{
				strUserName = strUserName.substr(0, nPos);
			}
			strOrganize = strTemp;

			if (m_bVirtualOrg)
			{
				bVirtualOrg = true;
				strOrganizeId = m_strOrganizeId;
			}
			else
				bVirtualOrg = false;
		}
	}

	if (!pEvent->bEncryptPwd)
    {
        UCSDK::Crypto* pCrypto = GetUCManager()->CreateCrypto();
        strEncryptPwd = pCrypto->PasswordCrypt(strEncryptPwd.c_str());
        pCrypto->Release();
    }

	std::shared_ptr<vector<StudentInfo>> pvecStudentInfo = make_shared<vector<StudentInfo>>();
	Response.pUserInfo = make_shared<UserInfo>();
	Response.pUserInfo->wstrUserName = pEvent->wstrUserName;
	Response.pUserInfo->tLoginBeginTime = pEvent->tBeginTime;
	Response.pUserInfo->wstrEncryptPwd = Utf8ToUnicode(strEncryptPwd.c_str());
	std::wstring wstrLoginPlat;
	wstrLoginPlat =_T("UC");
	Response.pUserInfo->wstrLoginPlat = wstrLoginPlat;
	bool bIsOfflineLogin = false;
	do 
	{
		string strNDOrganize = "nd";
		std::string strData, strUserId, strAccessToken, strMacKey, strServerTime;
		ULONGLONG tLoginTime = 0;

		DWORD dwRet = CUserLoginFactory::RequestAccLogin(strUserName, strEncryptPwd, strNDOrganize, strData);
		if (dwRet == 0)
		{
			if (CUserLoginFactory::ParseLogin(strData, strUserId, strAccessToken, strMacKey, strServerTime, tLoginTime, Response.dwErrorCode, Response.wstrErrorDescription))
			{
				Response.pUserInfo->tServerTime = tLoginTime;
				Response.pUserInfo->tLoginTime = GetTickCount();
				Response.pUserInfo->wstrUserId = Utf8ToUnicode(strUserId.c_str());
				Response.pUserInfo->wstrAccessToken = Utf8ToUnicode(strAccessToken.c_str());
				Response.pUserInfo->wstrMacKey = Utf8ToUnicode(strMacKey.c_str());
				Response.pUserInfo->wstrServerTime = Utf8ToUnicode(strServerTime.c_str());
				Response.pUserInfo->wstrLoginResponse = Utf8ToUnicode(strData.c_str());
			}
			else
				break;
		}

		dwRet = CUserLoginFactory::RequestUserInfo(strUserId, strAccessToken, strMacKey, strServerTime, tLoginTime, strData);
		if (dwRet == 0)
		{
			if (!CUserLoginFactory::ParseserInfo(strData, Response.pUserInfo, Response.dwErrorCode, Response.wstrErrorDescription))
				break;
		}
		
	} while (const_expression_cast(false));

	if (Response.dwErrorCode != 0)
	{
		do 
		{
			Response.dwErrorCode = 0;

			if (strOrganize.empty())
			{
				strOrganize = m_strOrganize;
			}
			std::string strData, strUserId, strAccessToken, strMacKey, strServerTime;
			bool bRet = false;
			if (bVirtualOrg)
			{
				UCSDK::VOrgLogin* pLogin = GetUCManager()->CreateVOrgLogin();
				bRet = pLogin->Excute(strUserName.c_str(), strEncryptPwd.c_str(), strOrganizeId.c_str());
				strData = pLogin->GetResult();
				UCSDK::User* pUser = pLogin->GetUser();
				if (bRet)
				{
					Response.pUserInfo->pUser = pUser;
					Response.pUserInfo->tServerTime = pUser->GetServerTime();
					Response.pUserInfo->tLoginTime = GetTickCount();
					Response.pUserInfo->wstrUserId = Utf8ToUnicode(pUser->GetUserId());
					Response.pUserInfo->wstrAccessToken = Utf8ToUnicode(pUser->GetAccessToken());
					Response.pUserInfo->wstrMacKey = Utf8ToUnicode(pUser->GetMacKey());
					Response.pUserInfo->wstrServerTime = Utf8ToUnicode(pUser->GetServerTimeString());
					Response.pUserInfo->wstrLoginResponse = Utf8ToUnicode(strData.c_str());
					Response.pUserInfo->wstrExpiresAt = Utf8ToUnicode(pUser->GetExpiresAt());
					Response.pUserInfo->wstrMacAlgorithm = Utf8ToUnicode(pUser->GetMacAlgorithm());
					Response.pUserInfo->wstrRefreshToken = Utf8ToUnicode(pUser->GetRefreshToken());
					if (!bCanceled)
						LoginBiz(Response.pUserInfo, pvecStudentInfo, Response.dwErrorCode, Response.wstrErrorDescription);
				}
				else
				{
					std::string strReturnCode = pLogin->GetReturnCode();
					Response.dwErrorCode = -1;
					Response.wstrErrorCode = Utf8ToNative(strReturnCode.c_str());
					Response.wstrErrorDescription = pLogin->GetReturnMessage();
				}
				pLogin->Release();
			}

		} while (false);
		
	}
	
	if (!bCanceled)
	{

		Response.Send();
	}
	
	bLogining = false;
}

void CUserLoginBiz::OnEventInBackground(const UserCenterRequestLogout* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
}

void CUserLoginBiz::OnEvent(UserCenterRequestCancelLogin* pEvent)
{
	bCanceled = pEvent->bCancel;
}

void CUserLoginBiz::LoginBiz(std::shared_ptr<UserInfo> pUserInfo, const std::shared_ptr<vector<StudentInfo>>& pvecStudentInfo, DWORD& dwErrorCode, std::wstring& wstrErrorDescription)
{
	UNREFERENCED_PARAMETER(pvecStudentInfo);
	do
	{
		std::string strData, strUserId, strAccessToken, strMacKey, strServerTime;
		strUserId = UnicodeToUtf8(pUserInfo->wstrUserId.c_str());
		strAccessToken = UnicodeToUtf8(pUserInfo->wstrAccessToken.c_str());
		strMacKey = UnicodeToUtf8(pUserInfo->wstrMacKey.c_str());
		strServerTime = UnicodeToUtf8(pUserInfo->wstrServerTime.c_str());

		DWORD dwRet = 0;
		UCSDK::VOrgGetUserProfile* pGetUserProfile = pUserInfo->pUser->CreateVOrgGetUserProfile();
		bool bRet = pGetUserProfile->Excute();
		if (true)
		{
			std::string strOrgId = pUserInfo->pUser->GetOrgId();
			pUserInfo->wstrMobile = Utf8ToUnicode(pUserInfo->pUser->GetMobile());
			pUserInfo->wstrEmail = Utf8ToUnicode(pUserInfo->pUser->GetEmail());
			pUserInfo->wstrOrgId = Utf8ToUnicode(strOrgId.c_str());
			pUserInfo->wstrBirthday = Utf8ToUnicode(pUserInfo->pUser->GetBirthday());
			pUserInfo->wstrNickName = Utf8ToUnicode(pUserInfo->pUser->GetNickName());
			pUserInfo->wstrRealName = Utf8ToUnicode(pUserInfo->pUser->GetRealName());
			
			pGetUserProfile->Release();

		}
		else
		{
			dwErrorCode = -1; 
			wstrErrorDescription = pGetUserProfile->GetReturnMessage();
			pGetUserProfile->Release();
			break;
		}

	} while (const_expression_cast(false));
}
