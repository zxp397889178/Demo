#pragma once

namespace Xdelte
{
	class Util
	{
	public:
		static string		Str2Utf8(const wstring& str);
		static wstring		Utf82Str(const string& str);
	};
}