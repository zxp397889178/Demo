#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fsmses_.E4.B8.8B.E5.8F.91.E7.9F.AD.E4.BF.A1.E9.AA.8C.E8.AF.81.E7.A0.81
    描述：下发短信验证码
    功能说明：下发短信验证码
    */
    class IGetSession
    {
    public:
        virtual bool Excute(DWORD dwSessionType, const char* pszDeviceId, const char* pszOrgName) = 0;
        virtual const char* GetSessionId() = 0;
        virtual const char* GetSessionKey() = 0;
        virtual DWORD GetOpCount() = 0;
        virtual bool GetIsNormal() = 0;
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

    */
    typedef class IGetSession	GetSession;
}

