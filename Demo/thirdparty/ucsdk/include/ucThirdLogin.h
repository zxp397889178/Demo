#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fthird_tokens_.E7.AC.AC.E4.B8.89.E6.96.B9.E7.99.BB.E5.BD.95
    ��������������¼
    ����˵������������¼
    */
    class IThirdLogin
    {
    public:
		virtual bool Excute(const char* pszOpenId, const char* pszAppId, const char* pszSourcePlat, const char* pszThirdAccessToken, const char* pszOrgName, const char* pszVOrgId) = 0;
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
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/LOGIN_FAILURE ��¼ʧ��
    UC/THIRD_PART_AUTH_FAILURE ������ƽ̨��֤ʧ��
    UC/NODE_NOT_EXIST ָ���Ľڵ㲻����
    UC/THIRD_PART_LOGIN_FAILURE �������û���½ʧ��
    UC/THIRD_PART_USER_UNBIND �������û���δ��
    UC/ORG_NOT_EXIST","ָ������֯������
    */
    typedef class IThirdLogin	ThirdLogin;
}