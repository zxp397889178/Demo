#pragma once
#include <map>
#include <string>

namespace NDHttp
{
	class HttpMessage
	{
	private:
		struct nocaseLess
		{
			bool operator() (const std::string & strS1, const std::string & strS2) const { return _stricmp(strS1.c_str(), strS2.c_str()) < 0; }
		};
	public:
		typedef std::string HeaderNameType;
		typedef std::string HeaderValueType;
		typedef std::map<HeaderNameType, HeaderValueType, nocaseLess> HeaderCollection;

		HttpMessage() = default;
		explicit HttpMessage(const HttpMessage &other) : m_headers(other.m_headers), m_strPost(other.m_strPost) {}
		~HttpMessage() = default;

		const std::string GetPost()const { return m_strPost; }
		bool HasBody()const { return !m_strPost.empty(); }

		HeaderValueType GetHeader(const HeaderNameType &strName)const
		{
			auto it = m_headers.find(strName);
			if (it != m_headers.end())
				return it->second;
			else
				return std::string();
		}
		HeaderCollection GetHeaders()const { return m_headers; }

		void AddHeader(const HeaderNameType &strName, const HeaderValueType &strValue) { SetHeader(strName, strValue); }
		void SetHeader(const HeaderNameType &strName, const HeaderValueType &strValue) { m_headers[strName] = strValue; }
		void RemoveHeader(const HeaderNameType &strName) { m_headers.erase(strName); }

		void SetPost(const std::string &strPost){ m_strPost = strPost; }

	private:
		std::string m_strPost;
		HeaderCollection m_headers;
	};
}
