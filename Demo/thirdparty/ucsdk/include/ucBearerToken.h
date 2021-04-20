#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fbearer_tokens_.E6.9C.8D.E5.8A.A1.E7.AB.AF.E7.99.BB.E5.BD.95.5BS.5D
    描述：服务端登录
    功能说明：服务端登录
    访问权限：
    BIZ_SERVER
    */
    class IBearerToken
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszPassword) = 0;
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
    UC/ACCOUNT_NOT_EXIST 帐号不存在
    UC/PASSWORD_NOT_CORRECT 密码不正确
    UC/LOGIN_FAILURE 登录失败
    */
    typedef class IBearerToken	BearerToken;
}