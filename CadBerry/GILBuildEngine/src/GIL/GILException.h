#pragma once
#include <exception>

namespace GIL
{
	class GILException : public std::exception
	{
		virtual const char* what() const throw();
	};
}