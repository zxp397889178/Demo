#pragma once

#include <string>
#include "include/HttpMessage.h"
#include "include/HttpRequest.h"

namespace NDHttp
{
	class HttpResponse : public HttpMessage
	{
		public:
			HttpResponse() = default;
			explicit HttpResponse(const HttpRequest & request) : HttpMessage(), m_httpRequest(request) {}
			HttpResponse(const HttpResponse & other)
			{
				m_nStatusCode = other.m_nStatusCode;
				m_httpRequest = other.m_httpRequest;
				SetResult(other.m_szData, other.m_nDataSize);
			}

			HttpResponse& operator=(HttpResponse & other)
			{
				if (this != &other)
				{
					m_nStatusCode = other.m_nStatusCode;
					m_httpRequest = other.m_httpRequest;
					SetResult(other.m_szData, other.m_nDataSize);
				}
				return *this;
			}

			~HttpResponse() 
			{ 
				if (m_szData)
				{
					free(m_szData);
				}
					
			}

			HttpRequest GetRequest()const { return m_httpRequest; }
			void SetStatusCode(int nStatusCode) { m_nStatusCode = nStatusCode; }
			int GetStatusCode()const { return m_nStatusCode; }
			const char* GetData()const { return m_szData; }
			size_t GetDataSize()const { return m_nDataSize; }
			void SetResult(const char* data, const size_t& size)
			{
				if (data && size > 0)
				{
					m_szData = (char*)malloc(size + 1);
					if (m_szData)
					{
						memcpy_s(m_szData, size, data, size);
						m_nDataSize = size;
						*(m_szData + m_nDataSize) = '\0';
					}
				}	
			}
		private:
			int m_nStatusCode = -1;
			char *m_szData = nullptr;
			size_t m_nDataSize = 0;
			HttpRequest m_httpRequest;
	};
}

