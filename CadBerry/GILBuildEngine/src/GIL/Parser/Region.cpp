#include "gilpch.h"
#include "Region.h"

namespace GIL
{
	namespace Parser
	{
		void Region::Add(int num)
		{
			this->Start += num;
			this->End += num;
		}
	}
}