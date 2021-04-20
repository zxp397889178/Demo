#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens.2F.7Baccess_token.7D.2Factions.2Fvalid_.E4.BB.A4.E7.89.8C.E6.A3.80.E6.9F.A5
    描述：校验短信验证码
    功能说明：验证短信验证码，注：仅限于找回密码使用
    */
    class IValidMobileCode
    {
    public:
        virtual bool Excute(const char* pszUserId, const char* pszMobileCode, const char* pszSessionId) = 0;
        virtual const char* GetNewMobileCode() = 0;
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetResult() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
    };

    /*
    返回Code列表:
    UC/ACCOUNT_NOT_EXIST 帐号不存在
    UC/SESSION_EXPIRED Session未创建或已过期
    UC/SMS_INVALID 短信验证码不正确
    */
    typedef class IValidMobileCode	ValidMobileCode;
}