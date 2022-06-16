#pragma once
#include <exception>

#include "Core.h"

namespace GIL
{
	class GILAPI GILException : public std::exception
	{
		virtual const char* what() const throw();
	};
}