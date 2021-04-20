#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens.2F.7Baccess_token.7D.2Factions.2Fclone_.E4.BB.A4.E7.89.8C.E5.85.8B.E9.9A.86
    描述：令牌克隆
    功能说明：令牌克隆，即,可以用旧的Mac Token克隆一份新的Mac Token，旧的Mac Token依然可以用
    */
    class ICloneToken
    {
    public:
		virtual bool Excute(const char* pszAccessToken, const char* pszMacToken, const char* pszNonce, const char* pszMethod, const char* pszRquestUri, const char* pszHost, const char* pszVOrgId) = 0;
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
	typedef class ICloneToken	CloneToken;
}