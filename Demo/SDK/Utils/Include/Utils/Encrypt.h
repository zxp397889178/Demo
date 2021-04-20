#pragma once

//bPem = true .Insert a newline character every 64 characters
UTILS_API
StringA
WINAPI
base64_encode_bufferA(
	const void*		pData,
	unsigned long	dwSize,
	bool			bPem = false);

UTILS_API
StringW
WINAPI
base64_encode_bufferW(
	const void*		pData,
	unsigned long	dwSize,
	bool			bPem = false);

#ifdef _UNICODE
#define  base64_encode_buffer	base64_encode_bufferW
#else
#define  base64_encode_buffer	base64_encode_bufferA
#endif


UTILS_API
StringA
WINAPI
base64_encode_strA(
	LPCSTR			lpstrData,
	bool			bPem = false);


UTILS_API
StringW
WINAPI
base64_encode_strW(
	LPCWSTR			lpstrData,
	bool			bPem = false,
	int				nCodePage = CP_UTF8);

//Pay attention to character encoding when using unicode version
#ifdef _UNICODE
#define  base64_encode_str	base64_encode_strW
#else
#define  base64_encode_str	base64_encode_strA
#endif


//////////////////////////////////////////////////////////////////////////

UTILS_API
StringA
WINAPI
base64_decode_strA(
	LPCSTR			lpstrData,
	bool			bPem = false);

UTILS_API
StringW
WINAPI
base64_decode_strW(
	LPCWSTR			lpwcsData,
	bool			bPem = false,
	int				nCodePage = CP_UTF8);

//Pay attention to character encoding when using unicode version
#ifdef _UNICODE
#define  base64_decode_str	base64_decode_strW
#else
#define  base64_decode_str	base64_decode_strA
#endif
