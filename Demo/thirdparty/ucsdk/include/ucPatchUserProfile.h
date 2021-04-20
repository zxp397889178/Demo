#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPATCH.5D.2Fusers.2F.7Buser_id.7D_.E8.AE.BE.E7.BD.AE.E7.94.A8.E6.88.B7.E4.B8.AA.E4.BA.BA.E5.B8.90.E5.8F.B7.E4.BF.A1.E6.81.AF
    描述：设置用户个人帐号信息
    功能说明：据用户id设置用户个人帐号信息
    参数说明：
    真实姓名和昵称增加姓氏多音字功能，如nick_name为“盖聂”，“盖”多音字取姓氏拼音“ge”，则nick_name_full为“genie”，nick_name_short为“gn”
    扩展信息org_exinfo和realm_exinfo：不要存储用户的敏感信息(如：身份证、手机号码等)，如果要存需加密后存入
    */
    class IPatchUserProfile
    {
    public:
        virtual bool Excute(const char* pszRealName, const char* pszMobile, const char* pszEmail, const char* pszNickName, const char* pszBirthday, const char* pszGender) = 0;
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
    UC/ORG_USER_CODE_HAS_EXIST 工号已经存在
    UC/USERNAME_INVALID 用户名格式不正确,需以字母开头,可包含字母、数字、_，最长20位
    UC/USERNAME_EXIST 用户名已存在
    UC/ORGUSERCODE_INVALID 工号或学号格式不正确,可包含字母、数字、_、.、@，但不允许是手机号码，最长50字符
    UC/INVALID_REAL_NAME 真实姓名不能为空且长度不能超过50字节
    UC/INVALID_NICK_NAME 昵称不能为空且长度不能超过50字节
    UC/INVALID_EXINFO_KEY 扩展属性key：只允许字母、数字、下划线表示，其中只能使用字母开头，最长20字符
    UC/INVALID_USER_REALM_EXINFO_KEY 用户邻域扩展key：只允许字母、数字、.、下划线表示，只能使用字母开头，其中，邻域以.连接，最长100字符
    UC/INVALID_ENABLE_STATUS 参数enable_status取值不正确,只能是0或1
    UC/NICK_NAME_CONTAIN_SENSITIVE_WORD 昵称不合法，包含敏感词
    */
    typedef class IPatchUserProfile	PatchUserProfile;
}