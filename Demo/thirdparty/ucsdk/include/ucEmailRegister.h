#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D_.2Femails.2Factions.2Fregister.3Fauto_login.3Dfalse.26lang.3D.7Blang.7D_.E4.BD.BF.E7.94.A8.E9.82.AE.E7.AE.B1.E8.BF.9B.E8.A1.8C.E5.B8.90.E5.8F.B7.E6.B3.A8.E5.86.8C
    ������ʹ����������ʺ�ע�ᣬע��������״̬Ϊ����֤��������ȷ���ʼ���
    ����˵����
    auto_login(��ѡ)����auto_login=trueʱ���ḽ�������Զ���¼��token��Ϣ
    lang(��ѡ�����ʼ��������Բ�����Ŀǰ��֧��lang=zh_cn��lang=en
    */
    class IEmailRegister
    {
    public:
        virtual bool Excute(const char* pszEmail, const char* pszPassword, const char* pszOrgName, const char* pszDeviceId, const char* pszLang) = 0;
        virtual const char* GetUserId() = 0;//�û����
        virtual const char* GetEmail() = 0;//�����ַ
        virtual const char* GetCreateTime() = 0;//�û�����ʱ��
        /*----------�����������������auto_login = trueʱ����--------------------*/
        virtual const char* GetAccessToken() = 0;//Token
        virtual const char* GetExpiresAt() = 0;//Token����ʱ��
        virtual const char* GetRefreshToken() = 0;//ˢ����Token
        virtual const char* GetMacKey() = 0;//Mac keyֵ
        virtual const char* GetMacAlgorithm() = 0;//Mac�㷨
        virtual time_t GetServerTime() = 0;//������ʱ��
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
    UC/SESSION_ID_INVALID ��Ч��session_id
    UC/EMAIL_FORMAT_INVALID �����ʽ����ȷ
    UC/EMAIL_HAS_REGISTER ���������Ѿ�ע���
    UC/ORG_NOT_EXIST ָ������֯������
    UC/ENCRYPT_DATA_INVALID ���Ĵ����Ϸ�
    UC/NO_PERMISSION_JOIN_USER ����֯����������û�������ϵUC������������û�Ȩ�ޣ�
    UC/REGISTER_OVER_SUM ���豸ע�ᳬ����������
    UC/DEFAULT_ORG_NOT_CONFIG ������֯������������֯��Ĭ����֯δ����
    */

    typedef class IEmailRegister	EmailRegister;
}

