#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fbearer_tokens.2F.7Bbearer_token.7D.2Factions.2Fvalid_.E6.9C.8D.E5.8A.A1.E7.AB.AF.28BearerToken.29.E4.BB.A4.E7.89.8C.E6.A3.80.E6.9F.A5.5BS.5D
    描述：服务端(BearerToken)令牌检查
    功能说明：检查服务端登录令牌(BearerToken)的合法性
    */
    class IValidBearerToken
    {
    public:
        virtual bool Excute(const char* pszBearerToken) = 0;
        virtual const char* GetBearerToken() = 0;
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
    UC/REQUIRE_ARGUMENT 缺少参数
    UC/INVALID_ARGUMENT 无效参数(格式不对,长度过长或过短等)
    UC/AUTH_INVALID_TOKEN 无效的授权
    UC/AUTH_TOKEN_EXPIRED 授权已过期
    */
    typedef class IValidBearerToken	ValidBearerToken;
}