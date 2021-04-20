#pragma once

namespace UCSDK
{
	static const std::string strEnvironment[7] = { "release", "beta", "pre", "aws", "awsca", "awshk", "snwjt" };
	static const std::string strHost[7] = { "aqapi.101.com", "ucbetapi.101.com", "ucbetapi.101.com", "awsuc.101.com", "uc-awsca.101.com", "uchk.101.com", "uc.sneduyun.com.cn" };
	static const std::string strVOrg_Host[7] = { "ucvorg.101.com", "ucvorg-beta.101.com", "ucvorg-beta.101.com", "awsvorg.101.com", "vorg-awsca.101.com", "vorghk.101.com", "vorg.sneduyun.com.cn" };
    static const std::string strGetSmsesUrl = "/%s/smses";
	static const std::string strLoginUrl = "/%s/tokens";
    static const std::string strThirdLoginUrl = "/%s/third_tokens";
	static const std::string strValidTokenUrl = "/%s/tokens/%s/actions/valid";
	static const std::string strCloneTokenUrl = "/%s/tokens/%s/actions/clone";
    static const std::string strValidMobileCodeUrl = "/%s/users/%s/mobile_code/actions/valid";
    static const std::string strValidBearerTokenUrl = "/%s/bearer_tokens/%s/actions/valid";
    static const std::string strBearerTokenUrl = "/%s/bearer_tokens";
    static const std::string strMobileRegisterUrl = "/%s/users?auto_login=%s";
    static const std::string strEmailRegisterUrl = "/%s/emails/actions/register?auto_login=%s&lang=%s";
    static const std::string strGetUserProfileUrl = "/%s/users/%s";
    static const std::string strPatchUserProfile = "/%s/users/%s";
    static const std::string strGetSessionUrl = "/%s/session";
    static const std::string strGetIdentifyCode = "/%s/sessions/%s/identify_code";
	static const std::string strCheckUserExsits = "/%s/users/actions/check";
    static const std::string strModifyPassword = "/%s/users/%s/password/actions/modify";
    static const std::string strMobileResetPassword = "/%s/users/password/actions/reset";
    static const std::string strEmailGetBackPassword = "/%s/emails/password/actions/reset?lang=%s";
    static const std::string strEmailResetPassword = "/%s/users/emails/password/actions/reset";
	static const std::string strGetServerTimeUrl = "/%s/server/time";

	static const std::string strVOrg_LoginUrl = "/%s/virtual_organizations/%s/tokens";
	static const std::string strVOrg_MobileRegisterUrl = "/%s/virtual_organizations/%s/users?auto_login=%s";
	static const std::string strVOrg_CheckUserExsits = "/%s/virtual_organizations/%s/users/actions/check";
	static const std::string strVOrg_GetSmsesUrl = "/%s/virtual_organizations/%s/smses";
	static const std::string strVOrg_GetUserProfileUrl = "/%s/virtual_organizations/%s/users/%s";
	static const std::string strVOrg_MobileResetPassword = "/%s/virtual_organizations/%s/users/password/actions/reset";

	struct stMacToken
	{
		std::string strMacToken;
		std::string strNonce;
	};
}
