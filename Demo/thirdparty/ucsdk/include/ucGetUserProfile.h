#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fusers.2F.7Buser_id.7D.5B.3Frealm.3Dxxx.5D_.E8.8E.B7.E5.8F.96.E7.94.A8.E6.88.B7.E4.BF.A1.E6.81.AF
    ��������ȡ�û���Ϣ
    ����˵���������û�id��ȡ�û���Ϣ��������ͬ��֯��ѯ
    */
    class IGetUserProfile
    {
    public:
        virtual DWORD Excute() = 0;//����0��ʾ����ɹ�����0��ʾ����ʧ��
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
    UC/AUTH_DENIED ��û�ж�ȡ���û���Ϣ��Ȩ��
    */
    typedef class IGetUserProfile	GetUserProfile;
}

