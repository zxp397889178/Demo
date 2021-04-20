#include "stdafx.h"
#include "Crypto_Wrapper.h"
#include "openssl/md5.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"
#include "openssl/bio.h"
#include "openssl/buffer.h"
#include "openssl/rsa.h"    
#include "openssl/pem.h"    

namespace Crypto_Wrapper
{
	std::string MD5(std::string& strData)
	{
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, strData.c_str(), strData.length());

		unsigned char digest[16] = { 0 };
		MD5_Final(digest, &ctx);

		char buf[33] = { '\0' };
		char tmp[3] = { '\0' };
		for (int i = 0; i < 16; i++){
			sprintf_s(tmp, "%02x", digest[i]);
			strcat_s(buf, tmp);
		}

		std::string result = buf;

		return result;
	}

	void MD5(std::string& strData, std::string& strResult)
	{
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, strData.c_str(), strData.length());

		unsigned char digest[16] = { 0 };
		MD5_Final(digest, &ctx);

		char buf[33] = { '\0' };
		char tmp[3] = { '\0' };
		for (int i = 0; i < 16; i++){
			sprintf_s(tmp, "%02x", digest[i]);
			strcat_s(buf, tmp);
		}

		strResult = buf;
	}

	std::string SHA256(std::string& strData, std::string& strKey)
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

		unsigned char tmp[41] = { '\0' };
		for (int i = 0; i != output_length; i++)
		{
			sprintf_s((char*)tmp, 40, "%02x", mac[i]);
			strResult += (char *)tmp;
		}
		free(mac);
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

	const unsigned long g_szCrcTable[256] =
	{
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
		0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
	};

	//
	//------------------------------------------------------------------------------
	// 功 能：CRC32校验码程序
	// 输入参数：
	// aData：要参与CRC32校验的内容buffer
	// aSize：要参与CRC32校验的字节数
	// 返 回：
	// CRC32校验结果
	//------------------------------------------------------------------------------
	DWORD CRC32(void* pData, unsigned long dwSize)
	{
		unsigned long crc32 = 0xFFFFFFFF;//CRC32校验码初值
		unsigned long tabitem;

		unsigned char * pTemp = (unsigned char *)pData;
		while (dwSize--)
		{

			tabitem = (crc32 ^ *pTemp++) & 0xFF;
			crc32 = ((crc32 >> 8) & 0xFFFFFF) ^ g_szCrcTable[tabitem];

		}
		return ~crc32;
	}

	string RSAEncrypt(void* pData, unsigned long dwDataSize, LPCSTR szKey, unsigned long dwKeySize)
	{
		std::string strResult = "";
		BIO* bio = NULL;
		RSA* pRSA = NULL;

		bio = BIO_new_mem_buf(szKey, dwKeySize);
		if (bio == NULL)
			goto _Out;

		pRSA = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
		if (pRSA == NULL)
			goto _Out;

		BYTE buffer[4096] = { 0 };

		int flen;
		int rsaSize = RSA_size(pRSA);
		int len = dwDataSize;

		if (rsaSize - 11 <= len)
			flen = rsaSize - 11;
		else
			flen = len;


		int outSize = RSA_public_encrypt(flen, (BYTE*)pData, buffer, pRSA, RSA_PKCS1_PADDING);
		if (outSize <= 0)
			goto _Out;

		strResult = Base64Encode(buffer, outSize);

	_Out:
		if (bio)
			BIO_free_all(bio);

		if (pRSA)
			RSA_free(pRSA);

		return strResult;
	}

    #define BlockSize	1024*1024*10		//40M

    DWORD  CalcFileCRC(LPCTSTR lptcsPath)
    {
        FILE* fp = NULL;
        _tfopen_s(&fp, lptcsPath, _T("rb"));
        if (!fp)
        {
            return 0;
        }

        _fseeki64(fp, 0, SEEK_END);
        UINT64 dwFileSize = _ftelli64(fp);
        _fseeki64(fp, 0, SEEK_SET);

        DWORD dwCrc;
        if (dwFileSize > BlockSize)
        {
            UINT64 dwCalcSize = 0;

            char* lpBuffer = new char[BlockSize];
            fread(lpBuffer, BlockSize, 1, fp);

            dwCrc = CalcBlockCRCStart(lpBuffer, BlockSize);

            delete[] lpBuffer;

            dwCalcSize += BlockSize;

            while (dwFileSize - dwCalcSize > BlockSize)
            {
                char* lpBuffer = new char[BlockSize];
                fread(lpBuffer, BlockSize, 1, fp);

                dwCrc = CalcBlockCRCProcess(lpBuffer, BlockSize, dwCrc);

                delete[] lpBuffer;

                dwCalcSize += BlockSize;
            }

            if (dwFileSize - dwCalcSize != 0)
            {
                char* lpBuffer = new char[unsigned int(dwFileSize - dwCalcSize)];
                fread(lpBuffer, size_t(dwFileSize - dwCalcSize), 1, fp);

                dwCrc = CalcBlockCRCEnd(lpBuffer, DWORD(dwFileSize - dwCalcSize), dwCrc);

                delete[] lpBuffer;
            }
            else
            {
                dwCrc = (dwCrc ^ 0xFFFFFFFF);
            }


        }
        else
        {
            char* lpBuffer = new char[(unsigned int)dwFileSize];
            fread(lpBuffer, (size_t)dwFileSize, 1, fp);

            dwCrc = CRC32(lpBuffer, (unsigned long)dwFileSize);

            delete[] lpBuffer;
        }

        fclose(fp);

        return dwCrc;
    }

	DWORD  CalcCRC(char* ptr, DWORD Size)
	{
		static DWORD crcTable[256];
		static bool bTableIniFlag = false;
		if (!bTableIniFlag)
		{
			DWORD crcTmp1;
			for (int i = 0; i < 256; i++)
			{
				crcTmp1 = i;
				for (int j = 8; j > 0; j--)
				{
					if (crcTmp1 & 1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
					else crcTmp1 >>= 1;
				}

				crcTable[i] = crcTmp1;
			}
			bTableIniFlag = true;
		}


		DWORD dwRet = 0;

		DWORD crcTmp2 = 0xFFFFFFFF;
		while (Size--)
		{
			crcTmp2 = ((crcTmp2 >> 8) & 0x00FFFFFF) ^ crcTable[(crcTmp2 ^ (*ptr)) & 0xFF];
			ptr++;
		}

		dwRet = (crcTmp2 ^ 0xFFFFFFFF);
		return dwRet;
	}

    DWORD CalcBlockCRCStart(char* ptr, DWORD Size)
    {
        static DWORD crcTable[256];
        static bool bTableIniFlag = false;
        if (!bTableIniFlag)
        {
            DWORD crcTmp1;
            for (int i = 0; i<256; i++)
            {
                crcTmp1 = i;
                for (int j = 8; j>0; j--)
                {
                    if (crcTmp1 & 1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
                    else crcTmp1 >>= 1;
                }

                crcTable[i] = crcTmp1;
            }
            bTableIniFlag = true;
        }

        DWORD crcTmp2 = 0xFFFFFFFF;
        while (Size--)
        {
            crcTmp2 = ((crcTmp2 >> 8) & 0x00FFFFFF) ^ crcTable[(crcTmp2 ^ (*ptr)) & 0xFF];
            ptr++;
        }

        return crcTmp2;
    }

    DWORD CalcBlockCRCProcess(char* ptr, DWORD Size, DWORD crcTmp)
    {
        static DWORD crcTable[256];
        static bool bTableIniFlag = false;
        if (!bTableIniFlag)
        {
            DWORD crcTmp1;
            for (int i = 0; i<256; i++)
            {
                crcTmp1 = i;
                for (int j = 8; j>0; j--)
                {
                    if (crcTmp1 & 1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
                    else crcTmp1 >>= 1;
                }

                crcTable[i] = crcTmp1;
            }
            bTableIniFlag = true;
        }


        DWORD dwRet = 0;

        while (Size--)
        {
            crcTmp = ((crcTmp >> 8) & 0x00FFFFFF) ^ crcTable[(crcTmp ^ (*ptr)) & 0xFF];
            ptr++;
        }

        return crcTmp;
    }

    DWORD CalcBlockCRCEnd(char* ptr, DWORD Size, DWORD crcTmp)
    {
        static DWORD crcTable[256];
        static bool bTableIniFlag = false;
        if (!bTableIniFlag)
        {
            DWORD crcTmp1;
            for (int i = 0; i<256; i++)
            {
                crcTmp1 = i;
                for (int j = 8; j>0; j--)
                {
                    if (crcTmp1 & 1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
                    else crcTmp1 >>= 1;
                }

                crcTable[i] = crcTmp1;
            }
            bTableIniFlag = true;
        }


        DWORD dwRet = 0;

        while (Size--)
        {
            crcTmp = ((crcTmp >> 8) & 0x00FFFFFF) ^ crcTable[(crcTmp ^ (*ptr)) & 0xFF];
            ptr++;
        }

        dwRet = (crcTmp ^ 0xFFFFFFFF);
        return dwRet;
    }

}



