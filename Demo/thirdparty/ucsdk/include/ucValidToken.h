#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens.2F.7Baccess_token.7D.2Factions.2Fvalid_.E4.BB.A4.E7.89.8C.E6.A3.80.E6.9F.A5
    描述：令牌检查
    功能说明：检查用户登录令牌的合法性（帐号中心的接口Header中有带Authorization的请求已经有令牌检查的功能，在调用前不需要再调用此接口,建议使用v0.93版本）
    */
    class IValidToken
    {
    public:
		virtual bool Excute(const char* pszAccessToken, const char* pszMacKey, const char* pszNonce, const char* pszMethod, const char* pszRquestUri, const char* pszHost, const char* pszVOrgId) = 0;
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetResult() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
        virtual User* GetUser() = 0;
    };

    /*
    返回Code列表:
    UC/REQUIRE_ARGUMENT 缺少参数
    UC/INVALID_ARGUMENT 无效参数(格式不对,长度过长或过短等)
    UC/AUTH_INVALID_TIMESTAMP 无效的时间戳，当时间戳与系统的差异大于5分钟
    UC/NONCE_INVALID Nonce串无效
    UC/AUTH_INVALID_TOKEN 无效的授权令牌(请登录)，注：当用户退出、令牌续约、修改密码、删除用户，这4钟情况下旧的token都会失效；报该异常时，需重新登录获取新token再使用。
    UC/AUTH_UNAVAILABLE_TOKEN 不可用授权令牌，注：令牌已被续约过会报该异常
    UC/AUTH_TOKEN_EXPIRED 授权已过期，注：令牌过期会报该异常，请调用4.1.5令牌续约接口获取新token再使用。
    UC/MAC_SIGN_INVALID Mac签名错误
    */
    typedef class IValidToken	ValidToken;
}