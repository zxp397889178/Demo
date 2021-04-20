#pragma once

namespace UCSDK
{
    /*
    */
    class ICrypto
    {
    public:
        virtual const char* PasswordCrypt(const char* pszPassword) = 0;//√‹¬Îº”√‹
        virtual const char* DESEncrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey) = 0;
        virtual const char* DESDecrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey) = 0;
		virtual stMacToken* GenerateMacToken(const char* pszHost, const char* pszUri, const char* pszMethod, const char* pszAccessToken, const char* pszMacKey, time_t tServerTime) = 0;
		virtual const char* GenerateAuthorization(const char* pszHost, const char* pszUri, const char* pszMethod, const char* pszAccessToken, const char* pszMacKey, time_t tServerTime) = 0;
        virtual void Release() = 0;
    };

    /*
    */
    typedef class ICrypto	Crypto;
}

