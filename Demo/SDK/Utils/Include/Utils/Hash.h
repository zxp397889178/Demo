#pragma once

UTILS_API
unsigned long
crc32_buffer(
	const void*		pData,
	unsigned long	dwSize);

UTILS_API
unsigned long
crc32_fileW( 
	LPCWSTR		lpwcsPath);


//////////////////////////////////////////////////////////////////////////

UTILS_API
StringA
md5_bufferA(
	const void*		pData,
	unsigned long	dwSize);

UTILS_API
StringW
md5_bufferW(
	const void*		pData,
	unsigned long	dwSize);

#ifdef _UNICODE
#define  md5_buffer	md5_bufferW
#else
#define  md5_buffer	md5_bufferA
#endif

UTILS_API
StringA
md5_fileA(
	LPCWSTR lpwcsPath);

UTILS_API
StringW
md5_fileW(
	LPCWSTR lpwcsPath);

#ifdef _UNICODE
#define  md5_file	md5_fileW
#else
#define  md5_file	md5_fileA
#endif

//////////////////////////////////////////////////////////////////////////
UTILS_API
StringA
hmac_sha256_bufferA(
	const void*		pData,
	unsigned long	dwSize,
	const void*		pKey,
	unsigned long	dwKeySize);

UTILS_API
StringW
hmac_sha256_bufferW(
	const void*		pData,
	unsigned long	dwSize,
	const void*		pKey,
	unsigned long	dwKeySize);

#ifdef _UNICODE
#define  hmac_sha256_buffer	hmac_sha256_bufferW
#else
#define  hmac_sha256_buffer	hmac_sha256_bufferA
#endif

//output digest directly
UTILS_API
StringA
hmac_sha256_buffer_ex(
	const void*		pData,
	unsigned long	dwSize,
	const void*		pKey,
	unsigned long	dwKeySize);
