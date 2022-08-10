#pragma once
#include <gilpch.h>

namespace lug
{
	struct syntax_position;
}

namespace GIL
{
	namespace Parser
	{
		struct ParserPosition
		{
			ParserPosition() : Line(-1), Column(-1) {}
			ParserPosition(lug::syntax_position pos);
			
			std::ostream& operator<<(std::ostream& os) const
			{
				os << "(" << Line << ", " << Column << ")";
			}
			
			int Line, Column;
		};
	}
}