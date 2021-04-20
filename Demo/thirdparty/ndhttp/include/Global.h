#ifndef NDHTTP_GLOBAL_H_
#define NDHTTP_GLOBAL_H_

#include "include/Config.h"

namespace NDHttp
{
	// For exposing resources over extern "C" functions
#if _MSC_VER < 1800
	typedef void* ndhttp_handle_t;
	typedef void* ndhttpfile_handle_t;
#else
	using ndhttp_handle_t = void*;
	using ndhttpfile_handle_t = void*;
#endif
}
#endif // !NDHTTP_GLOBAL_H_
