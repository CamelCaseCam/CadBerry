#include <utilspch.h>
#include "PrintWarning.h"

namespace utils
{
	namespace Operations
	{
		std::pair<std::vector<GIL::Parser::Region>, std::string> PrintWarning::Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
		{
			if (((GIL::InnerCode*)Params["$InnerCode"].Seq)->m_InnerCode.second.size() == 0)
			{
				CDB_BuildWarning("Warning called by utils module");
			}
			else
			{
				CDB_BuildWarning(((GIL::InnerCode*)Params["$InnerCode"].Seq)->m_InnerCode.second);
			}
			return std::pair<std::vector<GIL::Parser::Region>, std::string>();
		}

		GIL::Sequence* PrintWarning::self = nullptr;
		GIL::Sequence* PrintWarning::GetPtr()
		{
			if (self == nullptr)
			{
				self = new PrintWarning();
			}
			return self;
		}
	}
}