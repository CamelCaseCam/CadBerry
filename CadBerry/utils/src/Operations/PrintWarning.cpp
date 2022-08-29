#include <utilspch.h>
#include "PrintWarning.h"

namespace utils
{
	namespace Operations
	{
		void PrintWarning::Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& Context)
		{
			if (((GIL::InnerCode*)Params["$InnerCode"].Seq)->m_InnerCode.second.size() == 0)
			{
				CDB_BuildWarning("Warning called by utils module");
			}
			else
			{
				CDB_BuildWarning(((GIL::InnerCode*)Params["$InnerCode"].Seq)->m_InnerCode.second);
			}
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