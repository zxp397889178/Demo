#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens_.E7.94.A8.E6.88.B7.E7.99.BB.E5.BD.95
    �����������Ự
    ����˵��������һ���Ự ע��20���ӹ���
    */
    class IVOrgLogin
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszPassword, const char* pszVOrgId) = 0;
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetResult() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
        virtual User* GetUser() = 0;
    };

    /*
    ����Code�б�:
    UCSDK/REQUIRE_FAILED ��������ʧ��
    UCSDK/REQUIRE_ARGUMENT ȱ�ٲ���
    UCSDK/PARSE_FAILED ���ݽ���ʧ��
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/INVALID_LOGIN_NAME ��¼����ʽ����ȷ,ֻ�ܰ�����ĸ�����֡�_��.��@���50�ַ�
    UC/ORGNAME_INVALID ��֯����ʽ����ȷ,ֻ������ĸ�����֡�_���50�ַ�
    UC/ACCOUNT_NOT_EXIST �ʺŲ�����
    UC/PASSWORD_NOT_CORRECT ���벻��ȷ
    UC/ACCOUNT_UN_ENABLE �ʺ�δ����
    UC/LOGIN_FAILURE ��¼ʧ��
    UC/SESSION_EXPIRED Sessionδ�������ѹ���
    UC/SESSION_ID_NEED_FOR_ORG ����֯��¼��Ҫsession_id
    UC/SESSION_TYPE_INVALID �Ự���Ͳ���ȷ
    UC/IDENTIFY_CODE_REQUIRED ��Ҫ��֤���¼
    UC/IDENTIFY_CODE_INVALID ��Ч����֤��
    UC/ACCOUNT_LOCKED �ʺŵ�¼�������̫�࣬����������5���Ӻ��ٵ�¼��
    UC/ORG_NOT_EXIST","ָ������֯������
    */

	typedef class IVOrgLogin	VOrgLogin;
}

