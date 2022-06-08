#include "gilpch.h"
#include "GILException.h"

const char* GIL::GILException::what() const throw()
{
	return "GIL encountered an exception and terminated compilation";
}
