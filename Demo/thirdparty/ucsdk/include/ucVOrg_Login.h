#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens_.E7.94.A8.E6.88.B7.E7.99.BB.E5.BD.95
    描述：创建会话
    功能说明：创建一个会话 注：20分钟过期
    */
    class IVOrgLogin
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszPassword, const char* pszVOrgId) = 0;
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
    UCSDK/REQUIRE_FAILED 数据请求失败
    UCSDK/REQUIRE_ARGUMENT 缺少参数
    UCSDK/PARSE_FAILED 数据解析失败
    UC/REQUIRE_ARGUMENT 缺少参数
    UC/INVALID_ARGUMENT 无效参数(格式不对,长度过长或过短等)
    UC/INVALID_LOGIN_NAME 登录名格式不正确,只能包括字母、数字、_、.、@，最长50字符
    UC/ORGNAME_INVALID 组织名格式不正确,只允许字母、数字、_，最长50字符
    UC/ACCOUNT_NOT_EXIST 帐号不存在
    UC/PASSWORD_NOT_CORRECT 密码不正确
    UC/ACCOUNT_UN_ENABLE 帐号未启用
    UC/LOGIN_FAILURE 登录失败
    UC/SESSION_EXPIRED Session未创建或已过期
    UC/SESSION_ID_NEED_FOR_ORG 该组织登录需要session_id
    UC/SESSION_TYPE_INVALID 会话类型不正确
    UC/IDENTIFY_CODE_REQUIRED 需要验证码登录
    UC/IDENTIFY_CODE_INVALID 无效的验证码
    UC/ACCOUNT_LOCKED 帐号登录错误次数太多，被锁定！请5分钟后再登录！
    UC/ORG_NOT_EXIST","指定的组织不存在
    */

	typedef class IVOrgLogin	VOrgLogin;
}

