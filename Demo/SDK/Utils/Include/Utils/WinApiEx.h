#pragma once

UTILS_API
bool
WINAPI
WidecharToMultibyte(
	_In_z_			LPCWSTR		lpWidechar,
	_In_			int			nDstCodePage,
	_Inout_			StringA&	rStrMultibyte);

UTILS_API
bool
WINAPI
MultibyteToWidechar(
	_In_z_			LPCSTR		lpMultibyte,
	_In_			int			nSrcCodePage,
	_Inout_			StringW&	rStrWidechar);


UTILS_API
StringA
WINAPI
GetModuleFileDirA(
	_In_			HMODULE		hModule);


UTILS_API
StringW
WINAPI
GetModuleFileDirW(
	_In_			HMODULE		hModule);


#ifdef _UNICODE
#define  GetModuleFileDir	GetModuleFileDirW
#else
#define  GetModuleFileDir	GetModuleFileDirA
#endif

