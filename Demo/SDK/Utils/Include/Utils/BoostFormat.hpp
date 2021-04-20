#pragma once
#include "boost/format.hpp"

namespace tiny_boost
{
	template<class TFirst>
	void boost_format(tiny_boost::wformat& fmt, TFirst&& first)
	{
		fmt % first;
	}

	template<class TFirst, class... TOther>
	void boost_format(tiny_boost::wformat& fmt, TFirst&& first, TOther&&... other)
	{
		fmt % first;
		boost_format(fmt, other...);
	}


	template<class TFirst>
	void boost_format(tiny_boost::format& fmt, TFirst&& first)
	{
		fmt % first;
	}

	template<class TFirst, class... TOther>
	void boost_format(tiny_boost::format& fmt, TFirst&& first, TOther&&... other)
	{
		fmt % first;
		boost_format(fmt, other...);
	}
}

template<class TFirst, class... TOther>
StringW wcsformat(const wchar_t* format, TFirst&& first, TOther&&... other)
{
	try
	{
		tiny_boost::wformat fmt(format);
		tiny_boost::boost_format(fmt, first, other...);
		return fmt.str();
	}
	catch (...)
	{
		return _T("");
	}
}

template<class TFirst, class... TOther>
StringA strformat(const char* format, TFirst&& first, TOther&&... other)
{
	try
	{
		tiny_boost::format fmt(format);
		tiny_boost::boost_format(fmt, first, other...);
		return fmt.str();
	}
	catch (...)
	{
		return "";
	}
}

#ifdef _UNICODE
#define _tcsformat  wcsformat
#else
#define _tcsformat  strformat
#endif

#ifdef _UNICODE
typedef	tiny_boost::wformat tformat;
#else
typedef	tiny_boost::format tformat;
#endif
