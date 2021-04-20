#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPUT.5D.2Fusers.2Fpassword.2Factions.2Freset.C2.A0.E5.AF.86.E7.A0.81.E9.87.8D.E7.BD.AE
    描述：邮箱找回密码
    功能说明：使用邮箱/user_id找回密码,校验邮箱帐号是否存在，并发送邮件
    */
    class IEmailGetBackPassword
    {
    public:
		virtual bool Excute(const char* pszEmail, const char* pszUserId, const char* pszOrgName, const char* pszDeviceId, const char* pszLang) = 0;
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
    UC/SESSION_EXPIRED Session未创建或已过期
    UC/ORG_NOT_EXIST 指定的组织不存在
    UC/ENCRYPT_DATA_INVALID 密文串不合法
    UC/EMAIL_FORMAT_INVALID 邮箱格式不正确
    UC/EMAIL_SEND_FAILURE 邮件发送失败
    UC/EMAIL_OVER_SUM 发送次数超出上限，每个邮箱一天内最多只允许发送15条邮件
    UC/EMAIL_OVER_FREQUENT 邮件发送太频繁，一分钟内只允许发送一次
    UC/ACCOUNT_NOT_EXIST 账号不存在
    UC/RESET_URI_INVALID 无效的密码重置页,必须以https://或者http://开头
    */
    typedef class IEmailGetBackPassword	EmailGetBackPassword;
}

