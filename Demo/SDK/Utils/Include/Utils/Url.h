#pragma once

UTILS_API
StringA 
WINAPI
EncodeUriA(
	LPCSTR lpcsUri
	);

UTILS_API
StringW
WINAPI
EncodeUriW(
	LPCWSTR lpwcsUri
	);

UTILS_API
StringA
WINAPI
EncodeUriComponentA(
	LPCSTR lpcsUriComponent
	);

UTILS_API
StringW
WINAPI
EncodeUriComponentW(
	LPCWSTR lpwcsUriComponent
	);

#ifdef _UNICODE
#define  EncodeUriComponent	EncodeUriComponentW
#else
#define  EncodeUriComponent	EncodeUriComponentA
#endif

#ifdef _UNICODE
#define  EncodeUri	EncodeUriW
#else
#define  EncodeUri	EncodeUriA
#endif
