#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPUT.5D.2Fusers.2Fpassword.2Factions.2Freset.C2.A0.E5.AF.86.E7.A0.81.E9.87.8D.E7.BD.AE
    �����������һ�����
    ����˵����ʹ������/user_id�һ�����,У�������ʺ��Ƿ���ڣ��������ʼ�
    */
    class IEmailGetBackPassword
    {
    public:
		virtual bool Excute(const char* pszEmail, const char* pszUserId, const char* pszOrgName, const char* pszDeviceId, const char* pszLang) = 0;
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
    UC/SESSION_EXPIRED Sessionδ�������ѹ���
    UC/ORG_NOT_EXIST ָ������֯������
    UC/ENCRYPT_DATA_INVALID ���Ĵ����Ϸ�
    UC/EMAIL_FORMAT_INVALID �����ʽ����ȷ
    UC/EMAIL_SEND_FAILURE �ʼ�����ʧ��
    UC/EMAIL_OVER_SUM ���ʹ����������ޣ�ÿ������һ�������ֻ������15���ʼ�
    UC/EMAIL_OVER_FREQUENT �ʼ�����̫Ƶ����һ������ֻ������һ��
    UC/ACCOUNT_NOT_EXIST �˺Ų�����
    UC/RESET_URI_INVALID ��Ч����������ҳ,������https://����http://��ͷ
    */
    typedef class IEmailGetBackPassword	EmailGetBackPassword;
}

