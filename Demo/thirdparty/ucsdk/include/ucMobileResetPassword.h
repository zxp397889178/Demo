#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPUT.5D.2Fusers.2Fpassword.2Factions.2Freset.C2.A0.E5.AF.86.E7.A0.81.E9.87.8D.E7.BD.AE
    ��������������
    ����˵���������û��������룬Ϊ�û������������
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
    ����Code�б�:
    UCSDK/REQUIRE_FAILED ��������ʧ��
    UCSDK/REQUIRE_ARGUMENT ȱ�ٲ���
    UCSDK/PARSE_FAILED ���ݽ���ʧ��
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/ACCOUNT_NOT_EXIST �ʺŲ�����
    UC/SMS_EXPIRED ����δ�·����Ѿ�����
    UC/SMS_INVALID ������֤�벻��ȷ
    UC/ORG_NOT_EXIST ָ������֯������
    UC/SMSCODE_ERROR_OVER_SUM ������֤��һ�����������������ܳ�������(5��)�������»�ȡ������֤��
    */
    typedef class IMobileResetPassword	MobileResetPassword;
}

