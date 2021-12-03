#pragma once
#include "CadBerry.h"

namespace Core
{
	extern std::unordered_map<char, char> Complements;

	void GetReverseComplement(std::string& Input, std::string& Output);
	void GetComplement(std::string& Input, std::string& Output);
	void Reverse(std::string& Input, std::string& Output);
}