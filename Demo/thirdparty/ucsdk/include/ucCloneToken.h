#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Ftokens.2F.7Baccess_token.7D.2Factions.2Fclone_.E4.BB.A4.E7.89.8C.E5.85.8B.E9.9A.86
    ���������ƿ�¡
    ����˵�������ƿ�¡����,�����þɵ�Mac Token��¡һ���µ�Mac Token���ɵ�Mac Token��Ȼ������
    */
    class ICloneToken
    {
    public:
		virtual bool Excute(const char* pszAccessToken, const char* pszMacToken, const char* pszNonce, const char* pszMethod, const char* pszRquestUri, const char* pszHost, const char* pszVOrgId) = 0;
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
    UC/AUTH_INVALID_TIMESTAMP ��Ч��ʱ�������ʱ�����ϵͳ�Ĳ������5����
    UC/NONCE_INVALID Nonce����Ч
    UC/AUTH_INVALID_TOKEN ��Ч����Ȩ����(���¼)��ע�����û��˳���������Լ���޸����롢ɾ���û�����4������¾ɵ�token����ʧЧ�������쳣ʱ�������µ�¼��ȡ��token��ʹ�á�
    UC/AUTH_UNAVAILABLE_TOKEN ��������Ȩ���ƣ�ע�������ѱ���Լ���ᱨ���쳣
    UC/AUTH_TOKEN_EXPIRED ��Ȩ�ѹ��ڣ�ע�����ƹ��ڻᱨ���쳣�������4.1.5������Լ�ӿڻ�ȡ��token��ʹ�á�
    UC/MAC_SIGN_INVALID Macǩ������
    */
	typedef class ICloneToken	CloneToken;
}