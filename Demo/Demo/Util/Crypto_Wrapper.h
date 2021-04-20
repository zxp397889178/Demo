#pragma once

namespace Crypto_Wrapper
{
	std::string MD5(std::string& strData);
	void MD5(std::string& strData, std::string& strResult);
	std::string SHA256(std::string& strData, std::string& strKey);
	std::string SHA256Array(std::string& strData, std::string& strKey);
	std::string Base64Encode(void* pData, unsigned long dwSize);
	std::string Base64Decode(const char* encoded_string);
	DWORD CRC32(void* pData, unsigned long dwSize);
	string RSAEncrypt(void* pData, unsigned long dwDataSize, LPCSTR szKey, unsigned long dwKeySize);

    DWORD		CalcBlockCRCStart(char* ptr, DWORD Size);
    DWORD		CalcBlockCRCProcess(char* ptr, DWORD Size, DWORD crcTmp);
    DWORD		CalcBlockCRCEnd(char* ptr, DWORD Size, DWORD crcTmp);
	DWORD		CalcFileCRC(LPCTSTR lptcsPath);
	DWORD		CalcCRC(char* ptr, DWORD Size);
   
}
