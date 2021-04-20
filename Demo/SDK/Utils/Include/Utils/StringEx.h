#pragma once

UTILS_API
int
WINAPI
strsplit(
	Vector<StringA>*			pVctrDest,
	LPCSTR						lpcsSrc,
	LPCSTR						lpcsPattern);

UTILS_API
int
WINAPI
wcssplit(
	Vector<StringW>*			pVctrDest,
	LPCWSTR						lpwcsSrc,
	LPCWSTR						lpwcsPattern);


#ifdef _UNICODE
#define  _tcssplit	wcssplit
#else
#define  _tcssplit	strsplit
#endif

UTILS_API
StringA&
WINAPI
strtrim(
	StringA&	wstrSrc);

UTILS_API
StringA
WINAPI
strtrim(
	LPCSTR	lpcsSrc);


UTILS_API
StringW&
WINAPI
wcstrim(
	StringW&	wstrSrc);


UTILS_API
StringW
WINAPI
wcstrim(
	LPCWSTR	lpwcsSrc);


#ifdef _UNICODE
#define  _tcstrim	wcstrim
#else
#define  _tcstrim	strtrim
#endif


UTILS_API
StringA&
WINAPI
strreplace_first(
	StringA&		str,
	const StringA&	old_value,
	const StringA&	new_value);


UTILS_API
StringW&
WINAPI
wcsreplace_first(
	StringW&		str,
	const StringW&	old_value,
	const StringW&	new_value);


#ifdef _UNICODE
#define  _tcsreplace_first	wcsreplace_first
#else
#define  _tcsreplace_first	strreplace_first
#endif

UTILS_API
StringA&
WINAPI
strreplace_all(
	StringA&		str,
	const StringA&	old_value,
	const StringA&	new_value);


UTILS_API
StringW&
WINAPI
wcsreplace_all(
	StringW&		str,
	const StringW&	old_value,
	const StringW&	new_value);

#ifdef _UNICODE
#define  _tcsreplace_all	wcsreplace_all
#else
#define  _tcsreplace_all	strreplace_all
#endif

