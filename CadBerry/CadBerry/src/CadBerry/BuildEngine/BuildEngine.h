#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

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
			std::string& OutputType, std::string& Distribution) = 0;
		virtual void* BuildConsole(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputType) { return nullptr; }
		virtual void* BuildConsole(std::string src, std::string OutputType) { return nullptr; }

		/*For compatability, these are all optional but HIGHLY encouraged in order to support full CadBerry projects.
		* Also, this is probably going to be an expensive operation. This will be called before project compilation and only then (since CGIL 
		* intermediates can be generated without linking)
		* 
		* This function should return the code of an entry point file that imports/links the source files with the entry sequences and calls the entry 
		* sequences
		*/
		virtual std::string CreateEntryPoint(std::string& TargetOrganism, std::vector<std::string>& EntrySequences);

	protected:
		BuildEngine(std::vector<std::string> buildableTypes, std::vector<std::string> outputTypes) : BuildableTypes(buildableTypes), 
			OutputTypes(outputTypes) {}
		std::vector<std::string> BuildableTypes;
		std::vector<std::string> OutputTypes;
	};
}