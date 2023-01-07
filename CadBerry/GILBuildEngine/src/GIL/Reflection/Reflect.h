#pragma once
#include <unordered_map>
#include <string>
#include "Core.h"

//#define ReflectableClass(Parent) \
//inline std::unordered_map<std::string, Parent* (*)()> _Reflectable_ ## Parent ## _Map; \
//struct _Reflectable_MapAdder_ ## Parent\
//{\
//	_Reflectable_MapAdder_ ## Parent (std::string Name, Parent* (*ChildCreator)()) { _Reflectable_ ## Parent ## _Map[Name] = ChildCreator;}\
//};\
//class GILAPI Parent

struct GILAPI FieldInfo
{
	FieldInfo(void* parent, void* field) : offset((unsigned int)((char*)field - (char*)parent)) {}
	const unsigned int offset;
};

template<typename T, T* ExampleT>
struct ClassInfoAdder
{
	ClassInfoAdder()
	{
		T::ClassSize = sizeof(T);
		T::ExampleT = ExampleT;
	}
};

template<typename T>
class ReflectableClass
{
public:
	static std::unordered_map<std::string, FieldInfo*> Fields;
	static unsigned int ClassSize;
	static T* ExampleT;
};

#define Reflectable(name) CDBAPI name : public ReflectableClass<name>
#define Reflect(name) namespace ReflectImpl { inline name name ## _Example; inline ClassInfoAdder<name, &name ## _Example> name ## _InfoAdder; }

#define field(name) name; static inline FieldInfo name ## _info = FieldInfo((void*)ExampleT, (void*)&ExampleT->name); \
struct { struct fna_ ## name { fna_ ## name() { Fields[# name] = &name ## _info; } }; static inline fna_ ## name __fna_ ## name = fna_ ## name(); };


class Reflectable(x)
{
public:
	int field(TestData);
	int name; static inline FieldInfo name_info = FieldInfo((void*)ExampleT, (void*)&ExampleT->name); 
	struct { struct fna_name{ fna_name() { Fields["name"] = &name_info; } }; fna_name __fna_name; };
	int field(MoreData);
}; Reflect(x)

class z
{
	int a;
	void* b = &a;
	struct
	{
		int c;
	};
	struct
	{
		int d;
	};
};