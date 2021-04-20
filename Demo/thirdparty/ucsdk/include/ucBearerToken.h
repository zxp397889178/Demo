#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fbearer_tokens_.E6.9C.8D.E5.8A.A1.E7.AB.AF.E7.99.BB.E5.BD.95.5BS.5D
    ����������˵�¼
    ����˵��������˵�¼
    ����Ȩ�ޣ�
    BIZ_SERVER
    */
    class IBearerToken
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszPassword) = 0;
        virtual const char* GetBearerToken() = 0;
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
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/ACCOUNT_NOT_EXIST �ʺŲ�����
    UC/PASSWORD_NOT_CORRECT ���벻��ȷ
    UC/LOGIN_FAILURE ��¼ʧ��
    */
    typedef class IBearerToken	BearerToken;
}