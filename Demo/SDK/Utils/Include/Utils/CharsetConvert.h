#pragma once

UTILS_API
StringA 
WINAPI
AnsiToUtf8(
	LPCSTR pszAnsi
	);

UTILS_API
StringW
WINAPI
AnsiToUnicode(
	LPCSTR pszAnsi
	);

UTILS_API
StringA
WINAPI
Utf8ToAnsi(
	LPCSTR pszUtf8
	);

UTILS_API
StringW
WINAPI
Utf8ToUnicode(
	LPCSTR pszUtf8
	);

UTILS_API
StringA
WINAPI
UnicodeToAnsi(
	LPCWSTR pszUnicode
	);

UTILS_API
StringA
WINAPI
UnicodeToUtf8(
	LPCWSTR pszUnicode
	);

#ifdef _UNICODE
#define  NativeToUtf8	UnicodeToUtf8
#else
#define  NativeToUtf8	AnsiToUtf8
#endif

#ifdef _UNICODE
#define  NativeToUnicode(str)	str
#else
#define  NativeToUnicode		AnsiToUnicode
#endif

#ifdef _UNICODE
#define  Utf8ToNative	Utf8ToUnicode
#else
#define  Utf8ToNative	Utf8ToAnsi
#endif
