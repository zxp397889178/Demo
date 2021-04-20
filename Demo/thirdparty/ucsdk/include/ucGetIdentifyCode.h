#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fsessions.2F.7Bsession_id.7D.2Fidentify_code_.E8.8E.B7.E5.8F.96.E5.9B.BE.E7.89.87.E9.AA.8C.E8.AF.81.E7.A0.81
    描述：获取图片验证码
    功能说明：获取指定session_id的图片验证码 注：5分钟过期，图片大小：108*50
    */
    class IGetIdentifyCode
    {
    public:
        virtual bool GetMobileRegisterIdentifyCode(const wchar_t* pwszSavePath, const char* pszDeviceId, const char* pszOrgName) = 0;
        virtual bool GetMobileResetPasswordIdentifyCode(const wchar_t* pwszSavePath, const char* pszDeviceId, const char* pszOrgName) = 0;
        virtual const wchar_t* GetIdentifyCodePath() = 0;
        virtual const char* GetSessionId() = 0;
        virtual const char* GetSessionKey() = 0;
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
    };

    /*
    返回Code列表:
    UCSDK/OPENFILE_FAILED 保存文件无法打开
    UCSDK/REQUIRE_FAILED 数据请求失败
    UCSDK/REQUIRE_ARGUMENT 缺少参数
    UCSDK/PARSE_FAILED 数据解析失败
    */
    typedef class IGetIdentifyCode	GetIdentifyCode;
}

