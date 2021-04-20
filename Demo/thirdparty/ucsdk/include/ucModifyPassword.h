#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.2Factions.2Fcheck_.E5.B8.90.E5.8F.B7.E6.A0.A1.E9.AA.8C
    描述：密码修改
    功能说明：根据用户名、原密码，修改新密码
    */
    class IModifyPassword
    {
    public:
        virtual bool Excute(const char* pszOldPassword, const char* pszNewPassword) = 0;
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
    UCSDK/REQUIRE_FAILED 数据请求失败
    UCSDK/REQUIRE_ARGUMENT 缺少参数
    UCSDK/PARSE_FAILED 数据解析失败
    UC/REQUIRE_ARGUMENT 缺少参数
    UC/INVALID_ARGUMENT 无效参数(格式不对,长度过长或过短等)
    UC/ACCOUNT_NOT_EXIST 帐号不存在
    UC/PASSWORD_INVALID 密码不正确
    UC/PASSWORD_SAME 新密码与原密码相同
    */
    typedef class IModifyPassword	ModifyPassword;
}