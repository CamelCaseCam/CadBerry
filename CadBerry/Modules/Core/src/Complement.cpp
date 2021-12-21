#include "Complement.h"

namespace Core
{
	std::unordered_map<char, char> Complements = std::unordered_map<char, char>({
		{'a', 't'},
		{'A', 'T'},
		{'t', 'a'},
		{'T', 'A'},
		{'c', 'g'},
		{'C', 'G'},
		{'g', 'c'},
		{'G', 'C'},
		{'u', 'a'},
		{'U', 'A'},
		{'m', 'k'},
		{'M', 'K'},
		{'r', 'y'},
		{'R', 'Y'},
		{'w', 'w'},
		{'W', 'W'},
		{'s', 's'},
		{'S', 'S'},
		{'y', 'r'},
		{'Y', 'R'},
		{'k', 'm'},
		{'K', 'M'},
		{'v', 'b'},
		{'V', 'B'},
		{'h', 'd'},
		{'H', 'D'},
		{'d', 'h'},
		{'D', 'H'},
		{'b', 'v'},
		{'B', 'V'},
		{'n', 'n'},
		{'N', 'N'},
	});

	void GetReverseComplement(std::string& Input, std::string& Output)
	{
		GetComplement(Input, Output);
		int n = Output.length() - 1;
		for (int i = 0; i < Output.length() / 2; ++i)
		{
			std::swap(Output[i], Output[n]);
			--n;
		}
	}

	void GetComplement(std::string& Input, std::string& Output)
	{
		Output = "";
		Output.reserve(Input.length());
		for (char c : Input)
		{
			Output += Complements[c];
		}
	}
	
	void Reverse(std::string& Input, std::string& Output)
	{
		Output = "";
		Output.reserve(Input.length());
		for (int i = Input.length() - 1; i >= 0; --i)
		{
			Output += Input[i];
		}
	}
}