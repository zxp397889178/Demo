#pragma once
#include "ucDef.h"
#include "ucGetUserProfile.h"
#include "ucVOrg_GetUserProfile.h"
#include "ucPatchUserProfile.h"
#include "ucVOrg_PatchUserProfile.h"
#include "ucModifyPassword.h"
#include "ucGetSession.h"
#include "ucUser.h"
#include "ucLogin.h"
#include "ucVOrg_Login.h"
#include "ucValidToken.h"
#include "ucCloneToken.h"
#include "ucValidBearerToken.h"
#include "ucValidMobileCode.h"
#include "ucThirdLogin.h"
#include "ucBearerToken.h"
#include "ucMobileRegister.h"
#include "ucVOrg_MobileRegister.h"
#include "ucEmailRegister.h"
#include "ucGetIdentifyCode.h"
#include "ucCheckUserExists.h"
#include "ucVOrg_CheckUserExists.h"
#include "ucGetSMSes.h"
#include "ucVOrg_GetSMSes.h"
#include "ucCrypto.h"
#include "ucMobileResetPassword.h"
#include "ucVOrg_MobileResetPassword.h"
#include "ucEmailGetbackPassword.h"
#include "ucGetServerTime.h"



#ifdef UCSDK_EXPORTS
#define UCSDK_EXPORTS_API extern "C" __declspec(dllexport)
#else
#define UCSDK_EXPORTS_API extern "C" __declspec(dllimport) 
#endif // UCSDK_EXPORTS_API

namespace UCSDK
{
    /*
    设置配置文件路径,支持接口域名可配置
    */
    UCSDK_EXPORTS_API bool SetConfigPath(const char* pszConfigPath);

    /*
    设置日志文件路径，路径格式为文件夹，未设置不输出日志
    */
    UCSDK_EXPORTS_API bool SetLogPath(const wchar_t* pwszLogPath);

   

    class IUCManager
    {
    public:
        virtual void Initialize() = 0;
		virtual Login* CreateLogin() = 0;
		virtual VOrgLogin* CreateVOrgLogin() = 0;
        virtual ThirdLogin* CreateThirdLogin() = 0;
		virtual ValidToken* CreateValidToken() = 0;
		virtual CloneToken* CreateCloneToken() = 0;
        virtual ValidBearerToken* CreateValidBearerToken() = 0;
        virtual ValidMobileCode* CreateValidMobileCode() = 0;
        virtual BearerToken* CreateBearerToken() = 0;
		virtual MobileRegister* CreateMobileRegister() = 0;
		virtual VOrgMobileRegister* CreateVOrgMobileRegister() = 0;
        virtual EmailRegister* CreateEmailRegister() = 0;
        virtual GetIdentifyCode* CreateGetIdentifyCode() = 0;
		virtual CheckUserExsits* CreateCheckUserExsits() = 0;
		virtual VOrgCheckUserExsits* CreateVOrgCheckUserExsits() = 0;
		virtual GetSMSes* CreateGetSMSes() = 0;
		virtual VOrgGetSMSes* CreateVOrgGetSMSes() = 0;
        virtual Crypto* CreateCrypto() = 0;
		virtual MobileResetPassword* CreateMobileResetPassword() = 0;
		virtual VOrgMobileResetPassword* CreateVOrgMobileResetPassword() = 0;
		virtual EmailGetBackPassword* CreateEmailGetBackPassword() = 0;
		virtual GetServerTime* CreateGetServerTime() = 0;
        virtual GetSession* CreateGetSession() = 0;
        virtual User* CreateUser() = 0;
        virtual void Release() = 0;
		virtual bool SetEnvironment(const char* pszEnvironment) = 0;
    };
    typedef IUCManager UCManager;

    UCSDK_EXPORTS_API UCManager* InitUCManager();
}

