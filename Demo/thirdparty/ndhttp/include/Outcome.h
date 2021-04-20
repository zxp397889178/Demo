#pragma once

#include <string>
#include <utility>
#include "include/Error.h"
#include "include/HttpResponse.h"
#include "include/HttpFileResponse.h"

namespace NDHttp
{
	template<typename E, typename R>
	class Outcome
	{
	public:
		Outcome() :
			m_bSuccess(true), m_error(), m_result() { }
		explicit Outcome(const E &e) :
			m_error(e), m_bSuccess(false), m_result() { }
		explicit Outcome(const R &r) :
			m_result(r), m_bSuccess(true), m_error() { }
		Outcome(const Outcome &other) :
			m_bSuccess(other.m_bSuccess),
			m_error(other.m_error),
			m_result(other.m_result)
		{ }
		Outcome(Outcome &&other)
		{
			*this = std::move(other);
		}
		Outcome & operator=(const Outcome &other)
		{
			if (this != &other) {
				m_bSuccess = other.m_bSuccess;
				m_error = other.m_error;
				m_result = other.m_result;
			}
			return *this;
		}
		Outcome & operator=(Outcome &&other)
		{
			if (this != &other)
			{
				m_bSuccess = other.m_bSuccess;
				m_result = other.m_result;
				m_error = other.m_error;
			}
			return *this;
		}

		bool IsSuccess()const { return m_bSuccess; }
		E& GetError() { return m_error; }
		R& GetResult() { return m_result; }
	private:
		bool m_bSuccess;
		E m_error;
		R m_result;
	};

	typedef Outcome<Error, HttpResponse> HttpResponseOutcome;
	typedef Outcome<Error, HttpFileResponse> HttpFileResponseOutcome;

	const char CANCELCODE[] = "CANCELCODE";
	const char FAILEDCODE[] = "FAILEDCODE";
}
