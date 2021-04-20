#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fbearer_tokens.2F.7Bbearer_token.7D.2Factions.2Fvalid_.E6.9C.8D.E5.8A.A1.E7.AB.AF.28BearerToken.29.E4.BB.A4.E7.89.8C.E6.A3.80.E6.9F.A5.5BS.5D
    �����������(BearerToken)���Ƽ��
    ����˵����������˵�¼����(BearerToken)�ĺϷ���
    */
    class IValidBearerToken
    {
    public:
        virtual bool Excute(const char* pszBearerToken) = 0;
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
    UC/AUTH_INVALID_TOKEN ��Ч����Ȩ
    UC/AUTH_TOKEN_EXPIRED ��Ȩ�ѹ���
    */
    typedef class IValidBearerToken	ValidBearerToken;
}