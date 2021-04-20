#pragma once




namespace lexical_impl
{
	template <typename To, typename From>
	struct Converter
	{
	};

	template <>
	struct Converter<StringA, int>
	{
		static StringA convert(const int& from)
		{
			char a[12] = "";
			_itoa_s(from, a, 10);
			return a;
		}
	};

	template <>
	struct Converter<StringW, int>
	{
		static StringW convert(const int& from)
		{
			wchar_t w[12] = L"";
			_itow_s(from, w, 10);
			return w;
		}
	};

	template <>
	struct Converter<StringA, unsigned long>
	{
		static StringA convert(const unsigned long& from)
		{
			char a[12] = "";
			_ultoa_s(from, a, 16);
			return a;
		}
	};

	template <>
	struct Converter<StringW, unsigned long>
	{
		static StringW convert(const unsigned long& from)
		{
			wchar_t w[12] = L"";
			_ultow_s(from, w, 16);
			return w;
		}
	};

	template <>
	struct Converter<StringA, __int64>
	{
		static StringA convert(const __int64& from)
		{
			char a[24] = "";
			_i64toa_s(from, a, _countof(a), 10);
			return a;
		}
	};

	template <>
	struct Converter<StringW, __int64>
	{
		static StringW convert(const __int64& from)
		{
			wchar_t w[24] = L"";
			_i64tow_s(from, w, _countof(w), 10);
			return w;
		}
	};

	template <>
	struct Converter<StringA, unsigned __int64>
	{
		static StringA convert(const unsigned __int64& from)
		{
			char a[24] = "";
			_ui64toa_s(from, a, _countof(a), 10);
			return a;
		}
	};

	template <>
	struct Converter<StringW, unsigned __int64>
	{
		static StringW convert(const unsigned __int64& from)
		{
			wchar_t w[24] = L"";
			_ui64tow_s(from, w, _countof(w), 10);
			return w;
		}
	};

	template <>
	struct Converter<int, StringA>
	{
		static int convert(const StringA& from)
		{
			return atoi(from.c_str());
		}
	};


	template <>
	struct Converter<int, StringW>
	{
		static int convert(const StringW& from)
		{
			return _wtoi(from.c_str());
		}
	};

	template <>
	struct Converter<int, char>
	{
		static int convert(const char* from)
		{
			return atoi(from);
		}
	};

	template <>
	struct Converter<int, wchar_t>
	{
		static int convert(const wchar_t* from)
		{
			return _wtoi(from);
		}
	};

	template <>
	struct Converter<__int64, char>
	{
		static __int64 convert(const char* from)
		{
			return _atoi64(from);
		}
	};

	template <>
	struct Converter<__int64, wchar_t>
	{
		static __int64 convert(const wchar_t* from)
		{
			return _wtoi64(from);
		}
	};

	template <>
	struct Converter<unsigned __int64, char>
	{
		static unsigned __int64 convert(const char* from)
		{
			return _strtoui64(from, NULL, 10);
		}
	};

	template <>
	struct Converter<unsigned __int64, wchar_t>
	{
		static unsigned __int64 convert(const wchar_t* from)
		{
			return _wcstoui64(from, NULL, 10);
		}
	};
};


template<typename To, typename From>
inline To lexical_nothrow_cast(const From& from)
{
	return lexical_impl::Converter<To, From>::convert(from);
}

template<typename To, typename From>
inline To lexical_nothrow_cast(const From* from)
{
	return lexical_impl::Converter<To, From>::convert(from);
}
