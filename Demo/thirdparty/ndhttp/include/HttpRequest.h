#pragma once
#include <string>
#include "include/HttpMessage.h"
#include "include/Url.h"
#include <functional>

namespace NDHttp
{
	class HttpRequest : public HttpMessage
	{
		public:
			HttpRequest() = default;
			explicit HttpRequest(const Url &url, const std::string strMethod = "GET") : HttpMessage(), m_url(url), m_strMethod(strMethod) {}
			~HttpRequest() = default;

			std::string GetMethod()const { return m_strMethod; }
			void SetMethod(std::string strMethod) { m_strMethod = strMethod; }
			void SetUrl(const Url &url) { m_url = url; }
			Url GetUrl()const { return m_url; }
			void SetTimeout(const int &nTimeout) { m_nTimeout = nTimeout; }
			int GetTimeout()const { return m_nTimeout; }
		private:
			std::string m_strMethod;
			Url m_url;
			int m_nTimeout = 60;
	};

	typedef std::function<void(const int speed, const int remaintime, const unsigned __int64 cursize, const unsigned __int64 totalsize, void *userdata)> HttpFileRequestProgressHandler;
	class HttpFileRequest : public HttpRequest
	{
	public:
		HttpFileRequest() = default;
		explicit HttpFileRequest(const Url &url, const std::wstring& wstrLocalFilePath, HttpFileRequestProgressHandler handler = nullptr, void *userdata = nullptr, const std::string strMethod = "GET") :
			HttpRequest(url, strMethod), m_handler(handler), m_userdata(userdata), m_wstrLocalFilePath(wstrLocalFilePath) {}
		~HttpFileRequest() = default;

		void	SetLocalFilePath(const std::wstring wstrLocalFilePath);
		std::wstring GetLocalFilePath()const { return m_wstrLocalFilePath; }

		void	SetProgressHandler(HttpFileRequestProgressHandler handler);
		HttpFileRequestProgressHandler& GetProgressHandler() { return m_handler; }

		void*	GetUserData() { return m_userdata; }
	private:
		HttpFileRequestProgressHandler m_handler = nullptr;
		std::wstring	m_wstrLocalFilePath;
		void *m_userdata = nullptr;
	};
}

