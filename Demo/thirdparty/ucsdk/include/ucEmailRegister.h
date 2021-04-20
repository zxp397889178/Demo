#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D_.2Femails.2Factions.2Fregister.3Fauto_login.3Dfalse.26lang.3D.7Blang.7D_.E4.BD.BF.E7.94.A8.E9.82.AE.E7.AE.B1.E8.BF.9B.E8.A1.8C.E5.B8.90.E5.8F.B7.E6.B3.A8.E5.86.8C
    描述：使用邮箱进行帐号注册，注册后的邮箱状态为待验证，并发送确认邮件。
    功能说明：
    auto_login(可选)：当auto_login=true时，会附带返回自动登录的token信息
    lang(可选）：邮件内容语言参数，目前仅支持lang=zh_cn和lang=en
    */
    class IEmailRegister
    {
    public:
        virtual bool Excute(const char* pszEmail, const char* pszPassword, const char* pszOrgName, const char* pszDeviceId, const char* pszLang) = 0;
        virtual const char* GetUserId() = 0;//用户编号
        virtual const char* GetEmail() = 0;//邮箱地址
        virtual const char* GetCreateTime() = 0;//用户创建时间
        /*----------以下属性在请求参数auto_login = true时返回--------------------*/
        virtual const char* GetAccessToken() = 0;//Token
        virtual const char* GetExpiresAt() = 0;//Token过期时间
        virtual const char* GetRefreshToken() = 0;//刷新用Token
        virtual const char* GetMacKey() = 0;//Mac key值
        virtual const char* GetMacAlgorithm() = 0;//Mac算法
        virtual time_t GetServerTime() = 0;//服务器时间
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
    UC/SESSION_ID_INVALID 无效的session_id
    UC/EMAIL_FORMAT_INVALID 邮箱格式不正确
    UC/EMAIL_HAS_REGISTER 电子邮箱已经注册过
    UC/ORG_NOT_EXIST 指定的组织不存在
    UC/ENCRYPT_DATA_INVALID 密文串不合法
    UC/NO_PERMISSION_JOIN_USER 该组织不允许加入用户，请联系UC开启允许加入用户权限！
    UC/REGISTER_OVER_SUM 该设备注册超过次数限制
    UC/DEFAULT_ORG_NOT_CONFIG 个人组织不属于虚拟组织且默认组织未配置
    */

    typedef class IEmailRegister	EmailRegister;
}

