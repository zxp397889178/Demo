#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fsmses_.E4.B8.8B.E5.8F.91.E7.9F.AD.E4.BF.A1.E9.AA.8C.E8.AF.81.E7.A0.81
    �������·�������֤��
    ����˵�����·�������֤��
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
    ����Code�б�:
    UCSDK/REQUIRE_FAILED ��������ʧ��
    UCSDK/REQUIRE_ARGUMENT ȱ�ٲ���
    UCSDK/PARSE_FAILED ���ݽ���ʧ��
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/PHONE_NUMBER_FORMAT_INVALID �ֻ������ʽ����ȷ
    UC/ORG_NOT_EXIST ָ������֯������
    UC/PHONE_HAS_REGISTER �ֻ������Ѿ�ע���
    UC/USER_NOT_EXIST ѡ�����û�������
    UC/SMS_SEND_FAILURE �����·�ʧ��
    UC/SMS_TYPE_INVALID ������֤�����Ͳ���ȷ
    UC/SMS_OVER_SUM ���ʹ����������ޣ�ÿ���ֻ�����һ�������ֻ������15������
    UC/SMS_OVER_FREQUENT ���ŷ���̫Ƶ����һ������ֻ������һ��
    UC/SMS_OVER_FREQUENT","���ŷ���̫Ƶ����һ��IPһ������ֻ������һ��
    UC/INVALID_COUNTRY_CODE�������Ÿ�ʽ����ȷ,ֻ����+��ͷ����+�����֣��16�ַ�
    UC/SESSION_EXPIRED Sessionδ�������ѹ���
    UC/SESSION_TYPE_INVALID �Ự���Ͳ���ȷ
    */
	typedef class IVOrgGetSMSes	VOrgGetSMSes;
}

