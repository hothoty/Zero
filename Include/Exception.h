#pragma once
#include "String.h"

namespace Zero
{
	class CException : public std::exception
	{
	protected:
		StringA msg;

	public:
	    CException();
		virtual ~CException();

		CException(const char* pFormat, ...);
		CException(std::exception& src);

		const char *what() const;
	};
}
