#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fthird_tokens_.E7.AC.AC.E4.B8.89.E6.96.B9.E7.99.BB.E5.BD.95
    描述：第三方登录
    功能说明：第三方登录
    */
    class IThirdLogin
    {
    public:
		virtual bool Excute(const char* pszOpenId, const char* pszAppId, const char* pszSourcePlat, const char* pszThirdAccessToken, const char* pszOrgName, const char* pszVOrgId) = 0;
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
    UC/LOGIN_FAILURE 登录失败
    UC/THIRD_PART_AUTH_FAILURE 第三方平台认证失败
    UC/NODE_NOT_EXIST 指定的节点不存在
    UC/THIRD_PART_LOGIN_FAILURE 第三方用户登陆失败
    UC/THIRD_PART_USER_UNBIND 第三方用户尚未绑定
    UC/ORG_NOT_EXIST","指定的组织不存在
    */
    typedef class IThirdLogin	ThirdLogin;
}