#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.5B.3Fauto_login.3Dfalse.5D_.E4.BD.BF.E7.94.A8.E4.B8.8B.E5.8F.91.E7.9A.84.E7.9F.AD.E4.BF.A1.E9.AA.8C.E8.AF.81.E7.A0.81.E8.BF.9B.E8.A1.8C.E5.B8.90.E5.8F.B7.E6.B3.A8.E5.86.8C
    描述：手机注册
    功能说明：
    参数auto_login(可选)：当auto_login=true时，会附带返回自动登录的token信息
    昵称增加姓氏多音字功能，如nick_name为“盖聂”，“盖”多音字取姓氏拼音“ge”，则nick_name_full为“genie”，nick_name_short为“gn”
    */
    class IVOrgMobileRegister
    {
    public:
		virtual bool Excute(const char* pszMobile, const char* pszPassword, const char* pszMobileCode, const char* pszCountryCode, const char* pszOrgId, const char* pszOrgName, const char* pszSessionId, const char* pszSessionKey) = 0;
        virtual const char* GetUserId() = 0;//用户编号
        virtual const char* GetUserName() = 0;//Token
        virtual const char* GetNickName() = 0;//Token
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
    UC/REQUIRE_ARGUMENT 缺少参数
    UC/INVALID_ARGUMENT 无效参数(格式不对,长度过长或过短等)
    UC/PHONE_HAS_REGISTER 手机号码已经注册过
    UC/SMS_EXPIRED 短信未下发或已经过期
    UC/SMS_INVALID 短信验证码不正确
    UC/USERNAME_INVALID 用户名格式不正确,需以字母开头,可包含字母、数字、_，最长20字符
    UC/USERNAME_EXIST 用户名已存在
    UC/INVALID_NICK_NAME 昵称不能为空且长度不能超过50字节
    UC/INVALID_ENABLE_STATUS 参数enable_status取值不正确,只能是0或1
    UC/INVALID_COUNTRY_CODE国际区号格式不正确,只能以+开头包括+、数字，最长16字符
    UC/SMSCODE_ERROR_OVER_SUM 短信验证码一天内输入错误次数不能超过上限(5次)，请重新获取短信验证码！
    UC/NICK_NAME_CONTAIN_SENSITIVE_WORD 昵称不合法，包含敏感词
    UC/ORG_NOT_EXIST","指定的组织不存在
    UC/SESSION_TYPE_INVALID","会话类型不正确
    */

	typedef class IVOrgMobileRegister	VOrgMobileRegister;
}

