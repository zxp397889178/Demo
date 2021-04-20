#pragma once

namespace UCSDK
{
    class IUser
    {
    public:
        virtual PatchUserProfile* CreatePatchUserProfile() = 0;
        virtual GetUserProfile* CreateGetUserProfile() = 0;
        virtual ModifyPassword* CreateModifyPassword() = 0;

		virtual VOrgPatchUserProfile* CreateVOrgPatchUserProfile() = 0;
		virtual VOrgGetUserProfile* CreateVOrgGetUserProfile() = 0;

        virtual void SetUserId(const char* pszUserId) = 0;//用户编号
        virtual void SetAccessToken(const char* pszAccessToken) = 0;//Token
        virtual void SetExpiresAt(const char* pszExpiresAt) = 0;//Token过期时间
        virtual void SetRefreshToken(const char* pszRefreshToken) = 0;//刷新用Token
        virtual void SetServerTimeString(const char* pszServerTime) = 0;//服务器时间
        virtual void SetMacKey(const char* pszMacKey) = 0;//Mac key值
        virtual void SetMacAlgorithm(const char* pszMacAlgorithm) = 0;//Mac算法
        virtual void SetServerTime(time_t tServerTime) = 0;//服务器时间
        virtual void SetWarningCode(const char* pszWarningCode) = 0;//警告码，该属性在用户拥有重要角色的情况下才会出现；取值：1) PWD_EXPIRED：密码过期 2) CRUCIAL_ROLE_WITHDRAW:重要角色被收回，修改密码后自动归还
        virtual void SetRealName(const char* pszRealName) = 0;
        virtual void SetGender(const char* pszGender) = 0;
        virtual void SetBirthday(const char* pszBirthday) = 0;
        virtual void SetMobile(const char* pszMobile) = 0;
        virtual void SetCountryCode(const char* pszCountryCode) = 0;
        virtual void SetEmail(const char* pszEmail) = 0;
        virtual void SetOrgId(const char* pszOrgId) = 0;
        virtual void SetNickName(const char* pszNickName) = 0;
		virtual void BindMobile(bool bBindMobile) = 0;
		virtual void BindEmail(bool bBindEmail) = 0;

        virtual const char* GetUserId() = 0;//用户编号
        virtual const char* GetAccessToken() = 0;//Token
        virtual const char* GetExpiresAt() = 0;//Token过期时间
        virtual const char* GetRefreshToken() = 0;//刷新用Token
        virtual const char* GetServerTimeString() =0;//服务器时间
        virtual const char* GetMacKey() = 0;//Mac key值
        virtual const char* GetMacAlgorithm() = 0;//Mac算法
        virtual time_t GetServerTime() = 0;//服务器时间
        virtual const char* GetWarningCode() = 0;//警告码，该属性在用户拥有重要角色的情况下才会出现；取值：1) PWD_EXPIRED：密码过期 2) CRUCIAL_ROLE_WITHDRAW:重要角色被收回，修改密码后自动归还
        virtual const char* GetRealName() = 0;
        virtual const char* GetGender() = 0;
        virtual const char* GetBirthday() = 0;
        virtual const char* GetMobile() = 0;
        virtual const char* GetCountryCode() = 0;
        virtual const char* GetEmail() = 0;
        virtual const char* GetOrgId() = 0;
        virtual const char* GetNickName() = 0;
		virtual const bool IsBindMobile() = 0;
		virtual const bool IsBindEmail() = 0;
        virtual void Release() = 0;
    };

    typedef class IUser	User;
}

