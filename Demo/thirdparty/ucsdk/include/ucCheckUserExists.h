#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.2Factions.2Fcheck_.E5.B8.90.E5.8F.B7.E6.A0.A1.E9.AA.8C
    描述：帐号校验
    功能说明：帐号校验是否存在
    参数说明：个人用户，org_name参数不用传，或者传空，如org_name:""
    */
    class ICheckUserExsits
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszOrgName, const char* pszDeviceId) = 0;
        virtual const char* GetUserId() = 0;//用户编号
        virtual const char* GetEmail() = 0;//email
        virtual const char* GetMobile() = 0;//手机号
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
    UC/SESSION_TYPE_INVALID 会话类型不正确
    */
    typedef class ICheckUserExsits	CheckUserExsits;
}