#pragma once

#ifndef def_tstring
	#define def_tstring	

	#ifndef _STRING_
		#include <string>
	#endif
	using std::string;
	using std::wstring;

	typedef std::string		StringA;
	typedef std::wstring	StringW;

	#ifdef _UNICODE
		typedef std::wstring tstring;
	#else
		typedef std::string tstring;
	#endif

	#define std_base_string(Ch) std::basic_string<Ch, std::char_traits<Ch>, std::allocator<Ch> >


#endif
