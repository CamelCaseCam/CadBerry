#pragma once
#include <cdbpch.h>

namespace CDB
{
	class CDBAPI BuildEngine
	{
	public:
		virtual ~BuildEngine() {}
		inline std::vector<std::string>* GetBuildableTypes() { return &BuildableTypes; }
		inline std::vector<std::string>* GetOutputTypes() { return &OutputTypes; }

		virtual void PreBuild(std::string& path, std::string& PreBuildDir) = 0;
		virtual void Build(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputDir, 
			std::string& OutputType) = 0;

	protected:
		BuildEngine(std::vector<std::string> buildableTypes, std::vector<std::string> outputTypes) : BuildableTypes(buildableTypes), 
			OutputTypes(outputTypes) {}
		std::vector<std::string> BuildableTypes;
		std::vector<std::string> OutputTypes;
	};
}