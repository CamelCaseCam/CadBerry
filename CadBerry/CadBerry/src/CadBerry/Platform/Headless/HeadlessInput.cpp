#include <cdbpch.h>
#include "HeadlessInput.h"
#include "CadBerry/Log.h"

namespace CDB
{
	size_t CurrentInputInVector = 0;
	std::vector<std::string> Inputs;

	//This function lets us run tests when in headless mode with simulated input
	std::string GetInput()
	{
		std::string Output;
		if (Inputs.size() > 0)
		{
			if (CurrentInputInVector < Inputs.size())
			{
				Output = Inputs[CurrentInputInVector];
				std::cout << Inputs[CurrentInputInVector] << std::endl;
				++CurrentInputInVector;
				return Output;
			}
			return "";
		}
		std::getline(std::cin, Output);
		return Output;
	}
}