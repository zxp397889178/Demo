#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fsmses_.E4.B8.8B.E5.8F.91.E7.9F.AD.E4.BF.A1.E9.AA.8C.E8.AF.81.E7.A0.81
    描述：下发短信验证码
    功能说明：下发短信验证码
    */
    class IVOrgGetSMSes
    {
    public:
		virtual bool GetMobileRegisterSMSes(const char* pszMobile, const char* pszCountryCode, const char* pszOrgId, const char* pszIdentifyCode, const char* pszSessionId, const char* pszLang) = 0;
		virtual bool GetMobileResetPasswordSMSes(const char* pszMobile, const char* pszCountryCode, const char* pszOrgId, const char* pszIdentifyCode, const char* pszSessionId, const char* pszLang) = 0;
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
    UC/PHONE_NUMBER_FORMAT_INVALID 手机号码格式不正确
    UC/ORG_NOT_EXIST 指定的组织不存在
    UC/PHONE_HAS_REGISTER 手机号码已经注册过
    UC/USER_NOT_EXIST 选定的用户不存在
    UC/SMS_SEND_FAILURE 短信下发失败
    UC/SMS_TYPE_INVALID 短信验证码类型不正确
    UC/SMS_OVER_SUM 发送次数超出上限，每个手机号码一天内最多只允许发送15条短信
    UC/SMS_OVER_FREQUENT 短信发送太频繁，一分钟内只允许发送一次
    UC/SMS_OVER_FREQUENT","短信发送太频繁，一个IP一分钟内只允许发送一次
    UC/INVALID_COUNTRY_CODE国际区号格式不正确,只能以+开头包括+、数字，最长16字符
    UC/SESSION_EXPIRED Session未创建或已过期
    UC/SESSION_TYPE_INVALID 会话类型不正确
    */
	typedef class IVOrgGetSMSes	VOrgGetSMSes;
}

