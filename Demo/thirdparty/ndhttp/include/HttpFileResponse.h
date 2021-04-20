#pragma once

#include <string>
#include "include/HttpMessage.h"
#include "include/HttpRequest.h"

namespace NDHttp
{
	class HttpFileResponse : public HttpMessage
	{
		public:
			HttpFileResponse() = default;
			explicit HttpFileResponse(const HttpFileRequest & request) : HttpMessage(), m_httpRequest(request) {}
			HttpFileResponse(const HttpFileResponse & other)
			{
				m_nStatusCode = other.m_nStatusCode;
				m_httpRequest = other.m_httpRequest;
			}

			~HttpFileResponse() = default;
			HttpFileRequest GetRequest()const { return m_httpRequest; }
			void SetStatusCode(int nStatusCode) { m_nStatusCode = nStatusCode; }
			int GetStatusCode()const { return m_nStatusCode; }

		private:
			int m_nStatusCode = -1;
			HttpFileRequest m_httpRequest;
	};
}

