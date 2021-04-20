#include "stdafx.h"
#include "Common.h"
#include "Crypto_Wrapper.h"
#include <random>
#include "openssl/md5.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"
#include "openssl/bio.h"
#include "openssl/buffer.h"
#include "openssl/rsa.h"    
#include "openssl/pem.h"    

namespace common
{
	vector<string> Split(const string& str, const string& delims /*= _T("\t\n ")*/, unsigned int maxSplits /*= 0*/)
	{
		vector<string> ret;
		unsigned int numSplits = 0;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
			}
			else if (pos == string::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the tstring
				ret.push_back(str.substr(start));
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back(str.substr(start, pos - start));
				start = pos + 1;
			}
			// parse up to next real data
			start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != string::npos);
		return ret;
	}

	void PasswordCrypt(const char* pszPassword, unsigned char* pszEncryptPassword)
	{
		string strData = pszPassword;
		if (strData.length() == 0)
			return;

		static const char *salt = "fdjf,jkgfkl";
		static unsigned char bytes[4] = { 163, 172, 161, 163 };

		char szBuffer[16] = { 0 };
		memcpy(szBuffer, bytes, 4);

		strData += szBuffer;
		strData += salt;

		std::string strPassword = Crypto_Wrapper::MD5(strData);
		memcpy(pszEncryptPassword, strPassword.c_str(), strPassword.length());
	}

	void DESEncrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey, unsigned char* pszEncryptText, DWORD dwEncryptSize)
	{
		const EVP_CIPHER * evp_cipher = NULL;
		EVP_CIPHER_CTX ctx;
		EVP_CIPHER_CTX_init(&ctx);
		evp_cipher = EVP_des_ecb();
		if (EVP_CipherInit_ex(&ctx, evp_cipher, NULL, pszKey, NULL, 1) != 1)//加密
		{
			EVP_CIPHER_CTX_cleanup(&ctx);
			return;
		}

		unsigned char szEncryptText[1024];
		int nEncryptTextSize = sizeof(szEncryptText)-1;
		//加密  
		if (!EVP_CipherUpdate(&ctx, szEncryptText, &nEncryptTextSize, pszText, dwSize))
		{
			EVP_CIPHER_CTX_cleanup(&ctx);
			return;
		}


		//处理最后块填充部分  
		int nLastBufLen = 0;
		EVP_CipherFinal(&ctx, szEncryptText + nEncryptTextSize, &nLastBufLen);
		nEncryptTextSize += nLastBufLen;
		szEncryptText[nEncryptTextSize] = 0;
		std::string strDESEncryptText = Crypto_Wrapper::Base64Encode(szEncryptText, nEncryptTextSize);

		EVP_CIPHER_CTX_cleanup(&ctx);
		memcpy(pszEncryptText, strDESEncryptText.c_str(), strDESEncryptText.length());
		dwEncryptSize = strDESEncryptText.length();
	}


	void DESDecrypt(unsigned char* pszText, DWORD dwSize, unsigned char* pszKey, unsigned char* pszDecryptText, DWORD dwDecryptSize)
	{
		std::string strText = Base64Decode((const char*)pszText);

		const EVP_CIPHER * evp_cipher = NULL;
		EVP_CIPHER_CTX ctx;
		EVP_CIPHER_CTX_init(&ctx);
		evp_cipher = EVP_des_ecb();
		if (EVP_CipherInit_ex(&ctx, evp_cipher, NULL, pszKey, NULL, 0) != 1)//解密
		{
			EVP_CIPHER_CTX_cleanup(&ctx);
			return;
		}

		unsigned char szDecryptText[1024];
		int nDecryptTextSize = sizeof(szDecryptText)-1;
		//解密  
		if (!EVP_CipherUpdate(&ctx, szDecryptText, &nDecryptTextSize, (const unsigned char*)strText.c_str(), strText.length()))
		{
			EVP_CIPHER_CTX_cleanup(&ctx);
			return;
		}


		//处理最后块填充部分  
		int nLastBufLen = 0;
		EVP_CipherFinal(&ctx, szDecryptText + nDecryptTextSize, &nLastBufLen);
		nDecryptTextSize += nLastBufLen;
		szDecryptText[nDecryptTextSize] = 0;

		EVP_CIPHER_CTX_cleanup(&ctx);
		memcpy(pszDecryptText, szDecryptText, nDecryptTextSize);
	}

	static const string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
	std::string Base64Decode(const char* encoded_string)
	{
		int arr_size = strlen(encoded_string);
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];

		string ret;

		while (arr_size-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
		{
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4)
			{
				for (i = 0; i < 4; i++)
					char_array_4[i] = (unsigned char)base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;

			for (j = 0; j < 4; j++)
				char_array_4[j] = (unsigned char)base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

	std::string Base64Encode(void* pData, unsigned long dwSize)
	{
		BIO *buff, *b64f;
		BUF_MEM *ptr;

		b64f = BIO_new(BIO_f_base64());
		buff = BIO_new(BIO_s_mem());
		buff = BIO_push(b64f, buff);

		BIO_set_flags(buff, BIO_FLAGS_BASE64_NO_NL);
		BIO_set_close(buff, BIO_CLOSE);
		BIO_write(buff, pData, dwSize);
		BIO_flush(buff);

		BIO_get_mem_ptr(buff, &ptr);
		int nLen = ptr->length;
		char* base64 = (char *)malloc((nLen + 1) * sizeof(char));
		memcpy(base64, ptr->data, nLen);
		base64[nLen] = '\0';

		BIO_free_all(buff);

		std::string strResult = base64;
		free(base64);
		return strResult;
	}

	std::string SHA256Array(std::string& strData, std::string& strKey)
	{
		unsigned int output_length;
		std::string strResult;
		HMAC_CTX ctx;
		HMAC_CTX_init(&ctx);
		HMAC_Init_ex(&ctx, strKey.c_str(), strKey.length(), EVP_sha256(), NULL);

		HMAC_Update(&ctx, (unsigned char*)strData.c_str(), strData.length());        // input is OK; &input is WRONG !!!  

		unsigned char* mac = (unsigned char*)malloc(EVP_MAX_MD_SIZE);;
		HMAC_Final(&ctx, mac, &output_length);
		HMAC_CTX_cleanup(&ctx);

		for (int i = 0; i != output_length; i++)
		{
			strResult += mac[i];
		}
		free(mac);
		return strResult;
	}

	void GenerateMacToken(const char* pszHost, const char* pszUri, const char* pszMethod, const char* pszAccessToken, const char* pszMacKey, time_t tServerTime, unsigned char* pszOutText, DWORD dwOutSize)
	{
		string strNonce = GetNonce(tServerTime);

		string strMac;
		strMac += strNonce;
		strMac += "\n";
		strMac += pszMethod;
		strMac += "\n";
		strMac += pszUri;
		strMac += "\n";
		strMac += pszHost;
		strMac += "\n";

		string strSHA256 = SHA256Array(strMac, std::string(pszMacKey));
		string strOut =  Base64Encode((void*)strSHA256.c_str(), strSHA256.length());

		// Authorization header
		char szMacToken[MAX_PATH * 2] = { 0 };
		sprintf_s(szMacToken, MAX_PATH * 2 - 1, "MAC id=\"%s\",nonce=\"%s\",mac=\"%s\"", pszAccessToken, strNonce.c_str(), strOut.c_str());

		dwOutSize = strlen(szMacToken);
		memcpy(pszOutText, szMacToken, dwOutSize);
	}

	std::string GetNonce(time_t tServerTime)
	{
		LONGLONG curTime = tServerTime * 1000;

		char szCurTime[MAX_PATH] = { 0 };
		sprintf_s(szCurTime, MAX_PATH - 1, "%I64d", curTime);
		string strNonce = szCurTime;
		strNonce += ":";
		strNonce += generate_random_str(8);
		return strNonce;
	}

	std::string generate_random_str(size_t strSize)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::string strResult;

		const static char ALPHA[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int size = sizeof(ALPHA) / sizeof(ALPHA[0]) - 1;

		char* pszRandom = new char[strSize + 1];
		pszRandom[strSize] = 0;
		for (size_t i = 0; i < strSize; ++i)
		{
			pszRandom[i] = ALPHA[mt() % size];
		}

		strResult = string(pszRandom, strSize);
		delete pszRandom;

		return strResult;
	}

	ULONGLONG ExchangeServerTime(string& strServerTime)
	{
		time_t tServerTime = 0;
		int nYear, nMonth, nDate, nHour, nMin, nSec, nMilli;
		char szUTC[128];
		char szUTCNum[128];
		char szUTCSymbol[128];
		if (sscanf_s(strServerTime.c_str(), "%d-%d-%d%[^0-9]%d:%d:%d.%d%[+-]%s", &nYear, &nMonth, &nDate, szUTC, _countof(szUTC) - 1, &nHour, &nMin, &nSec, &nMilli, szUTCSymbol, _countof(szUTCSymbol) - 1, szUTCNum, _countof(szUTCNum) - 1) == 10)
		{
			struct tm sTime;
			sTime.tm_year = nYear;
			sTime.tm_mon = nMonth;
			sTime.tm_mday = nDate;
			sTime.tm_hour = nHour;
			sTime.tm_min = nMin;
			sTime.tm_sec = nSec;
			sTime.tm_year -= 1900;
			sTime.tm_mon -= 1;
			tServerTime = _mkgmtime(&sTime);
			if (_stricmp(szUTCSymbol, "+") == 0)
			{
				tServerTime -= (atoi(szUTCNum) / 100) * 60 * 60;
			}
			else if (_stricmp(szUTCSymbol, "-") == 0)
			{
				tServerTime += (atoi(szUTCNum) / 100) * 60 * 60;
			}
		}
		return tServerTime;
	}

	bool CreateDirectories(const wstring& path)
	{
		bool bRet = true;

		vector<string> direct = Split(UnicodeToUtf8(path.c_str()), "\\");
		

		wstring cur_path;
		for (vector<string>::iterator iter = direct.begin(); iter != direct.end(); ++iter)
		{
			cur_path += Utf8ToUnicode((*iter).c_str());

#if defined _WIN32 || defined _WIN64
			cur_path += L"\\";
			if (_waccess(cur_path.c_str(), 0) != 0)
			{
				bRet = _wmkdir(cur_path.c_str()) == 0;
			}
#else
			cur_path += L"/";
			string spath = unicode_to_utf8(cur_path);
			if (access(spath.c_str(), 0) != 0)
			{
				mkdir(spath.c_str(), 0777);
			}
#endif
		}

		return bRet;
	}




	bool GetRegisterStringValue(LPCTSTR lpszRegFullPath, tstring & strValue, bool b64Bit)
	{
		tstring strRegFullPath = lpszRegFullPath;

		//提取注册表根键,子键
		tstring strRegRootKey = _T("");
		tstring strRegSubKey = _T("");
		tstring strRegValueKey = _T("");


		int nPos = strRegFullPath.find(_T('\\'));
		if (nPos != std::string::npos)
		{
			strRegRootKey = strRegFullPath.substr(0, nPos);
			strRegSubKey = strRegFullPath.substr(nPos + 1);
		}

		nPos = strRegSubKey.rfind(_T('\\'));
		if (nPos > 0)
		{
			strRegValueKey = strRegSubKey.substr(nPos + 1);//substr( nPos + 1 );
			strRegSubKey = strRegSubKey.substr(0, nPos);//.substr(0, nPos );
		}


		HKEY  hRootKey = 0;
		if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CLASSES_ROOT")) == 0)
		{
			hRootKey = HKEY_CLASSES_ROOT;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CURRENT_CONFIG")) == 0)
		{
			hRootKey = HKEY_CURRENT_CONFIG;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CURRENT_USER")) == 0)
		{
			hRootKey = HKEY_CURRENT_USER;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_LOCAL_MACHINE")) == 0)
		{
			hRootKey = HKEY_LOCAL_MACHINE;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_USERS")) == 0)
		{
			hRootKey = HKEY_USERS;
		}
		else
		{
			return false;
		}

		REGSAM samDesired = KEY_READ;
		if (b64Bit)
			samDesired |= KEY_WOW64_64KEY;

		//读取
		HKEY  hKeyQuery = NULL;
		if (RegOpenKeyEx(hRootKey, strRegSubKey.c_str(), 0, samDesired, &hKeyQuery) == ERROR_SUCCESS)
		{
			TCHAR szValue[MAX_PATH] = { 0 };
			DWORD dwRead = MAX_PATH;
			DWORD dwType;
			DWORD dwLen = MAX_PATH * sizeof(TCHAR);
			long nRtQuery = RegQueryValueEx(hKeyQuery,
				strRegValueKey.empty() ? NULL : strRegValueKey.c_str() /*该参数为空时获取"默认"值*/,
				0, &dwType, (BYTE*)szValue, &dwLen);
			RegCloseKey(hKeyQuery);
			hKeyQuery = NULL;

			if (nRtQuery == ERROR_SUCCESS)//查询成功
			{
				strValue = szValue;
				return true;
			}

		}

		return false;
	}

	bool SetRegisterStringValue(LPCTSTR lpszRegFullPath, tstring strValue, bool b64Bit)
	{
		tstring strRegFullPath = lpszRegFullPath;

		//提取注册表根键,子键
		tstring strRegRootKey = _T("");
		tstring strRegSubKey = _T("");
		tstring strRegValueKey = _T("");


		int nPos = strRegFullPath.find(_T('\\'));
		if (nPos != std::string::npos)
		{
			strRegRootKey = strRegFullPath.substr(0, nPos);
			strRegSubKey = strRegFullPath.substr(nPos + 1);
		}

		nPos = strRegSubKey.rfind(_T('\\'));
		if (nPos > 0)
		{
			strRegValueKey = strRegSubKey.substr(nPos + 1);//substr( nPos + 1 );
			strRegSubKey = strRegSubKey.substr(0, nPos);//.substr(0, nPos );
		}


		HKEY  hRootKey = 0;
		if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CLASSES_ROOT")) == 0)
		{
			hRootKey = HKEY_CLASSES_ROOT;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CURRENT_CONFIG")) == 0)
		{
			hRootKey = HKEY_CURRENT_CONFIG;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_CURRENT_USER")) == 0)
		{
			hRootKey = HKEY_CURRENT_USER;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_LOCAL_MACHINE")) == 0)
		{
			hRootKey = HKEY_LOCAL_MACHINE;
		}
		else if (_tcsicmp(strRegRootKey.c_str(), _T("HKEY_USERS")) == 0)
		{
			hRootKey = HKEY_USERS;
		}
		else
		{
			return false;
		}

		//写入
		return SetRegisterStringValue(hRootKey, strRegSubKey.c_str(), strRegValueKey.empty() ? NULL : strRegValueKey.c_str(), strValue.c_str(), b64Bit);
	}

	bool SetRegisterStringValue(HKEY hRootKey, LPCTSTR lpszPath, LPCTSTR lpszkey, LPCTSTR lpszValue, bool b64Bit)
	{
		REGSAM samDesired = KEY_SET_VALUE | KEY_WRITE;
		if (b64Bit)
			samDesired |= KEY_WOW64_64KEY;

		//读取
		HKEY  hKeySet = 0;
		DWORD dwReserved = 0;
		DWORD RegResult = 0;
		if (RegOpenKeyEx(hRootKey, lpszPath, 0, samDesired, &hKeySet) != ERROR_SUCCESS)
		{
			RegCreateKeyEx(hRootKey, lpszPath, dwReserved, _T("REG_SZ"), 0, samDesired, 0, &hKeySet, &RegResult);
		}

		if (hKeySet != NULL)
		{
			DWORD dwSize = sizeof(DWORD);
			long nRt = RegSetValueEx(hKeySet, lpszkey, dwReserved, REG_SZ, (const BYTE*)(lpszValue), _tcslen(lpszValue) * sizeof(TCHAR));

			RegCloseKey(hKeySet);
			if (nRt == ERROR_SUCCESS)//查询成功
			{
				return true;
			}
		}
		return false;
	}
}