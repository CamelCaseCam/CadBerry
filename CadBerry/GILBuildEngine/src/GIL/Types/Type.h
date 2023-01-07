#pragma once
#include "Core.h"

namespace GIL
{
	namespace Parser
	{
		class GILAPI Project;
	}
	
	class GILAPI Type
	{
	public:
		Type() {}
		Type(std::string typeName, uint16_t typeID) : TypeName(typeName), TypeID(typeID) {}
		Type(std::string typeName, uint16_t typeID, std::vector<Type*>&& inherits) : TypeName(typeName), TypeID(typeID), Inherits(inherits) {}
		virtual ~Type() {}
		
		virtual bool IsOfType(Type* other, bool FromChild);
		virtual void AddInheritance(Type* other) { Inherits.push_back(other); }
		virtual void RemoveInheritance(Type* other);
		virtual void ClearInheritance() { Inherits.clear(); }

		virtual void Save(std::ofstream& OutputFile);
		virtual Type* Load(std::ifstream& InputFile, Parser::Project* Proj);

		inline uint16_t GetID() { return TypeID; }
		
#define FirstUserTypeIdx 7
		inline const bool IsUserType() const { return TypeID >= FirstUserTypeIdx; }

		static Type any;
		static Type cds;
		static Type ncds;
		static Type promoter;
		static Type protein;
		static Type GIL_bool;
		static Type data;

		std::string TypeName;
	private:
		uint16_t TypeID = 0;
		
		//Parent types
		std::vector<Type*> Inherits;
	};

	class Type_any : public Type
	{
	public:
		Type_any(std::string typeName, uint16_t typeID) : Type(typeName, typeID) {}

		virtual bool IsOfType(Type* other, bool FromChild) override { return !FromChild; }
	};
}