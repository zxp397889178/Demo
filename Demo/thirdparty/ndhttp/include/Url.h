#pragma once
#include <string>
#include <sstream>

namespace NDHttp
{
	class Url
	{
	public:
		Url() = default;
		explicit Url(const std::string &strUrl) {  FromString(strUrl); }
		~Url() = default;

		bool operator==(const Url &url) const
		{
			return m_strScheme == url.m_strScheme
				&& m_strHost == url.m_strHost
				&& m_strPath == url.m_strPath
				&& m_nPort == url.m_nPort
				&& m_strQuery == url.m_strQuery
				&& m_strFragment == url.m_strFragment;
		}

		bool operator!=(const Url &url) const { return !(*this == url); }

		void Clear()
		{
			m_strScheme.clear();
			m_strHost.clear();
			m_strPath.clear();
			m_strQuery.clear();
			m_strFragment.clear();
			m_nPort = -1;
		}

		void FromString(const std::string &strUrl)
		{
			Clear();
			if (strUrl.empty())
				return;

			std::string str = strUrl;
			std::string::size_type pos = 0;
			std::string strAuthority, strFragment, strPath, strQuery, strScheme;

			pos = str.find("://");
			if (pos != str.npos) {
				strScheme = str.substr(0, pos);
				str.erase(0, pos + 3);
			}
			else
			{
				strScheme = "http";
			}

			pos = str.find('#');
			if (pos != str.npos) {
				strFragment = str.substr(pos + 1);
				str.erase(pos);
			}

			pos = str.find('?');
			if (pos != str.npos) {
				strQuery = str.substr(pos + 1);
				str.erase(pos);
			}

			pos = str.find('/');
			if (pos != str.npos) {
				strPath = str.substr(pos);
				str.erase(pos);
			}
			else
				strPath = "/";

			strAuthority = str;

			SetAuthority(strAuthority);
			SetScheme(strScheme);
			SetPath(strPath);
			SetQuery(strQuery);
			SetFragment(strFragment);
		}

		bool HasFragment() const { return !m_strFragment.empty(); }
		bool HasQuery() const { return !m_strQuery.empty(); }

		bool IsEmpty() const
		{
			return m_strScheme.empty()
				&& m_strHost.empty()
				&& m_strPath.empty()
				&& m_strQuery.empty()
				&& m_strFragment.empty();
		}

		bool IsValid() const
		{
			if (IsEmpty())
				return false;

			if (m_strHost.empty())
				return false;

			return true;
		}

		std::string ToString()const
		{
			if (!IsValid())
				return std::string();

			std::ostringstream out;
			if (!m_strScheme.empty())
				out << m_strScheme << "://";
			std::string str = GetAuthority();
			if (!str.empty())
				out << GetAuthority();
			if (m_strPath.empty())
				out << "/";
			else
				out << m_strPath;
			if (HasQuery())
				out << "?" << m_strQuery;
			if (HasFragment())
				out << "#" << m_strFragment;
			return out.str();
		}
		
		void SetAuthority(const std::string & strAuthority)
		{
			if (strAuthority.empty()) {
				SetHost("");
				SetPort(-1);
				return;
			}

			std::string userinfo, host, port;
			std::string::size_type pos = 0, prevpos = 0;

			pos = strAuthority.find('@');
			if (pos != strAuthority.npos) {
				userinfo = strAuthority.substr(0, pos);
				prevpos = pos + 1;
			}
			else {
				pos = 0;
			}

			pos = strAuthority.find(':', prevpos);
			if (pos == strAuthority.npos)
				host = strAuthority.substr(prevpos);
			else {
				host = strAuthority.substr(prevpos, pos - prevpos);
				port = strAuthority.substr(pos + 1);
			}

			SetHost(host);
			SetPort(!port.empty() ? atoi(port.c_str()) : -1);
		}

		std::string GetAuthority() const
		{
			if (!IsValid())
				return std::string();

			std::ostringstream out;
			out << m_strHost;
			if (m_nPort != -1)
				out << ":" << m_nPort;
			return out.str();
		}

		void SetScheme(const std::string &strScheme) { m_strScheme = strScheme; }
		std::string GetScheme() const { return m_strScheme; }

		void SetHost(const std::string &strHost) { m_strHost = strHost; }
		std::string GetHost()const { return m_strHost; }

		void SetPath(const std::string &strPath) { m_strPath = strPath; }
		std::string GetPath() const { return m_strPath; }

		void SetQuery(const std::string &strQuery) { m_strQuery = strQuery; }
		std::string GetQuery() const { return m_strQuery; }

		void SetFragment(const std::string &strFragment) { m_strFragment = strFragment; }
		std::string GetFragment() const { return m_strFragment; }

		void SetPort(int nPort) { m_nPort = nPort; }
		int GetPort()const { return m_nPort; }
	private:
		std::string m_strScheme;
		std::string m_strHost;
		std::string m_strPath;
		std::string m_strQuery;
		std::string m_strFragment;
		int m_nPort = -1;
	};
}

