#pragma once

//return null if alloc failed
#ifndef new_nothrow
#define new_nothrow  new(std::nothrow)
#endif

//OutputDebugString for debug.
#ifndef OutputDebugFormatStringA
	#ifdef _DEBUG
		#define OutputDebugFormatStringA(format, ...) {\
			va_list va;\
			va_start(va, format);\
			char szContent[MAX_PATH * 2];\
			_vsnprintf_s(szContent, _countof(szContent) - 1, format, va);\
			OutputDebugStringA(szContent);\
			va_end(va);\
		}
	#else
		#define OutputDebugFormatStringA(format, ...)
	#endif
#endif


#ifndef OutputDebugFormatStringW
	#ifdef _DEBUG
		#define OutputDebugFormatStringW(format, ...) {\
			va_list va;\
			va_start(va, format);\
			wchar_t szContent[MAX_PATH * 2];\
			_vsnwprintf_s(szContent, _countof(szContent) - 1, format, va);\
			OutputDebugStringW(szContent);\
			va_end(va);\
		}
	#else
		#define OutputDebugFormatStringW(format, ...)
	#endif
#endif

#ifdef _UNICODE
#define OutputDebugFormatString OutputDebugFormatStringW
#else
#define OutputDebugFormatString OutputDebugFormatStringA
#endif


//read only instance, but don't release
#define ImplementInstance(cls)\
public:\
	static cls*	cls::GetInstance()\
	{\
		static cls* m_pInstance = NULL; \
		if (!m_pInstance)\
		{\
			m_pInstance = new_nothrow cls; \
		}\
		return m_pInstance; \
	}

//access to  release instance
#define DeclareAccessInstance(cls)\
private:\
	static cls* m_pInstance;\
public:\
	static void cls::CreateInstance(); \
	static cls*	cls::GetInstance(); \
	static void cls::ReleaseInstance();


#define ImplementAccessInstance(cls)\
	cls* cls::m_pInstance = NULL;\
	void cls::CreateInstance()\
	{\
		if (!m_pInstance)\
		{\
			m_pInstance = new_nothrow cls; \
		}\
	}\
	cls* cls::GetInstance()\
	{\
		return m_pInstance; \
	}\
	void cls::ReleaseInstance()\
	{\
		if (m_pInstance)\
		{\
			delete m_pInstance; \
			m_pInstance = NULL;\
		}\
	}

#ifndef IntAsBool
#define IntAsBool(i)   ( i ? true : false )
#endif

#ifndef BoolAsInt
#define BoolAsInt(b)   ( b ? 1 : 0 )
#endif

#ifndef const_expression_cast
#define const_expression_cast(b)   ( 0,(b))
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)          \
	/*lint -save -e527 -e530 */ \
	{ \
	(P) = (P); \
	}
#endif

#ifndef Auto_Release
#define Auto_Release

template<class T>
class CAutoRelease
{
public:
	CAutoRelease(T* pClass){
		m_pClass = pClass;
	}
	~CAutoRelease(){
		if (m_pClass){
			delete m_pClass;
		}
	}
private:
	T* m_pClass;
};
#endif

#ifndef AutoNull
class CAutoNull
{
public:
	CAutoNull(void* pData){
		m_pData = pData;
	}
	~CAutoNull(){
		if (m_pData){
			*(unsigned long*)m_pData = 0;
		}
	}
private:
	void* m_pData;
};

#define AutoNull(pData) CAutoNull tmpAutoNull(pData)
#endif

#if !defined(AutoCloseHandle) && defined(_WINBASE_)
class CAutoCloseHandle
{
public:
	CAutoCloseHandle(HANDLE* pHandle){
		m_pHandle = (HANDLE*)pHandle;
	}
	~CAutoCloseHandle(){
		if (m_pHandle
			&& *m_pHandle != NULL
			&& *m_pHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(*m_pHandle);
			*m_pHandle = NULL;
		}
	}
private:
	HANDLE* m_pHandle;
};

#define AutoCloseHandle(pHandle) CAutoCloseHandle tmpAutoCloseHandle(pHandle)
#endif