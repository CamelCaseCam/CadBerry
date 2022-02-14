#pragma once
#include <string>
#include <future>

#ifdef IRES_DLL
	#define IRESAPI __declspec(dllexport)
#else
	#define IRESAPI __declspec(dllimport)
#endif // IRES_DLL


namespace IRESDesign
{
	struct IRESAPI IRESParams
	{
		int GenerationSize;
		int NumGenerations;
		int NewPerGeneration;
		int PreservePerGeneration;
		int MinAttenuatorLength;
		int MaxAttenuatorLength;
		int MaxBoundOverlap;
		int* UpdateGenerationNumber;
		std::string Target;

		IRESParams(int generationSize, int numGenerations, int newPerGeneration, int preservePerGeneration, int minAttenuatorLength, int maxAttenuatorLength, int maxBoundOverlap, int* updateGenerations, std::string&& target) : 
			GenerationSize(generationSize), MinAttenuatorLength(minAttenuatorLength), NewPerGeneration(newPerGeneration), PreservePerGeneration(preservePerGeneration), 
			MaxAttenuatorLength(maxAttenuatorLength), MaxBoundOverlap(maxBoundOverlap), UpdateGenerationNumber(updateGenerations), Target(target), 
			NumGenerations(numGenerations) {}
	};
	IRESAPI void* CreateIRESThread(void* Params);
	IRESAPI std::string CreateIRES(int GenerationSize, int NumGenerations, int NewPerGeneration, int PreservePerGeneration, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, int* UpdateGenerationNumber, std::string& Target);
}