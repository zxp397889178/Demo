#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPUT.5D.2Fusers.2Fpassword.2Factions.2Freset.C2.A0.E5.AF.86.E7.A0.81.E9.87.8D.E7.BD.AE
    描述：密码重置
    功能说明：根据用户名、密码，为用户重置密码操作
    */
    class IMobileResetPassword
    {
    public:
        virtual bool Excute(const char* pszMobile, const char* pszNewPassword, const char* pszMobileCode, const char* pszOrgName) = 0;
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
    UC/SMS_EXPIRED 短信未下发或已经过期
    UC/SMS_INVALID 短信验证码不正确
    UC/ORG_NOT_EXIST 指定的组织不存在
    UC/SMSCODE_ERROR_OVER_SUM 短信验证码一天内输入错误次数不能超过上限(5次)，请重新获取短信验证码
    */
    typedef class IMobileResetPassword	MobileResetPassword;
}

