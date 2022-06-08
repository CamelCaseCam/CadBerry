#include <gilpch.h>
#include "Type.h"

#include "GIL/SaveFunctions.h"
#include "GIL/Parser/Project.h"

namespace GIL
{
	Type Type::any = Type_any("any", 0);
	Type Type::cds = Type("cds", 1, { &Type::any });
	Type Type::protein = Type("protein", 2, { &Type::cds });
	Type Type::ncds = Type("ncds", 3, { &Type::any });
	Type Type::promoter = Type("promoter", 4, { &Type::ncds });
	Type Type::GIL_bool = Type("bool", 5, { &Type::ncds });


	bool Type::IsOfType(Type* other, bool FromChild)
	{
		if (this == other || this->TypeID == other->TypeID)
			return true;
		for (Type* t : this->Inherits)
		{
			if (t->IsOfType(other, true))
				return true;
		}
		return false;
	}
	
	void Type::RemoveInheritance(Type* other)
	{
		for (int i = 0; i < this->Inherits.size(); ++i)
		{
			if (this->Inherits[i] == other)
			{
				this->Inherits.erase(this->Inherits.begin() + i);
			}
		}
	}
	
	void Type::Save(std::ofstream& OutputFile)
	{
		//Save the type name
		SaveString(this->TypeName, OutputFile);

		//Save the type inheritance
		OutputFile.write((char*)this->Inherits.size(), sizeof(size_t));
		
	}
	
	Type* Type::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		//Recreate the type
		std::string NewType;
		LoadStringFromFile(NewType, InputFile);
		auto TypeID = Proj->AllocType(NewType);
		
		//Load the type inheritance
		size_t InheritanceCount;
		InputFile.read((char*)&InheritanceCount, sizeof(size_t));
		for (size_t i = 0; i < InheritanceCount; ++i)
		{
			std::string InheritanceName;
			LoadStringFromFile(InheritanceName, InputFile);
			
			//Make sure the type exists
			auto InheritanceID = Proj->AllocType(InheritanceName);
			Proj->AddInheritance(TypeID, InheritanceID);
		}
	}
}