#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fsessions.2F.7Bsession_id.7D.2Fidentify_code_.E8.8E.B7.E5.8F.96.E5.9B.BE.E7.89.87.E9.AA.8C.E8.AF.81.E7.A0.81
    ��������ȡͼƬ��֤��
    ����˵������ȡָ��session_id��ͼƬ��֤�� ע��5���ӹ��ڣ�ͼƬ��С��108*50
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
    ����Code�б�:
    UCSDK/OPENFILE_FAILED �����ļ��޷���
    UCSDK/REQUIRE_FAILED ��������ʧ��
    UCSDK/REQUIRE_ARGUMENT ȱ�ٲ���
    UCSDK/PARSE_FAILED ���ݽ���ʧ��
    */
    typedef class IGetIdentifyCode	GetIdentifyCode;
}

