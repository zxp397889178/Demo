#pragma once
#include <string>

namespace NDHttp
{
	class Error
	{
	public:
		Error() = default;
		Error(const std::string strErrorCode, const std::string strErrorMessage) : m_strErrorCode(strErrorCode), m_strErrorMessage(strErrorMessage) {}
		~Error() = default;

		std::string GetErrorCode()const { return m_strErrorCode; }
		std::string GetErrorMessage() const { return m_strErrorMessage; }
		void SetErrorCode(const std::string &strErrorCode) { m_strErrorCode = strErrorCode; }
		void SetErrorMessage(const std::string& strErrorMessage) { m_strErrorMessage = strErrorMessage; }

	private:
		std::string m_strErrorCode;
		std::string m_strErrorMessage;
	};
}