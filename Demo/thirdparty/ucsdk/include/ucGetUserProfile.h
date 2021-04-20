#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fusers.2F.7Buser_id.7D.5B.3Frealm.3Dxxx.5D_.E8.8E.B7.E5.8F.96.E7.94.A8.E6.88.B7.E4.BF.A1.E6.81.AF
    描述：获取用户信息
    功能说明：根据用户id获取用户信息，仅允许同组织查询
    */
    class IGetUserProfile
    {
    public:
        virtual DWORD Excute() = 0;//返回0表示请求成功，非0表示请求失败
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
    UC/AUTH_DENIED 你没有读取该用户信息的权限
    */
    typedef class IGetUserProfile	GetUserProfile;
}

