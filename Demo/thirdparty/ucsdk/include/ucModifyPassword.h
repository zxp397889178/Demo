#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.2Factions.2Fcheck_.E5.B8.90.E5.8F.B7.E6.A0.A1.E9.AA.8C
    �����������޸�
    ����˵���������û�����ԭ���룬�޸�������
    */
    class IModifyPassword
    {
    public:
        virtual bool Excute(const char* pszOldPassword, const char* pszNewPassword) = 0;
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
    UC/PASSWORD_INVALID ���벻��ȷ
    UC/PASSWORD_SAME ��������ԭ������ͬ
    */
    typedef class IModifyPassword	ModifyPassword;
}