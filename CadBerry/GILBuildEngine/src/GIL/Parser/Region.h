#pragma once
#include <gilpch.h>

namespace GIL
{
	namespace Parser
	{
		class Region
		{
		public:
			Region() {}
			Region(std::string name, int start, int end) : Name(name), Start(start), End(end) {}

			void Add(int num);

			std::string Name;
			int Start = 0;
			int End = 0;
		};

		inline std::ostream& operator<<(std::ostream& os, const Region& r)
		{
			return os << r.Name << ": " << r.Start << r.End;
		}
	}
}