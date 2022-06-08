#define IRES_DLL
#include "GenerateIRES.h"
#include "GIL.h"
#include "CadBerry/Threading/ThreadPool.h"

#include "RNA.h"
#undef min
#undef max

#include <stdlib.h>
#include <random>
#include <ctime>

namespace IRESDesign
{
	//The base HCV IRES
	const std::string IRES = "GCCAgcccccUGAUGGGGGGGACACUCCACCAUGAAUCACUCCCCUGUGAGGAACUACUGUCUUCACGCAGAAAGCGUCUAGCCAUGGCGUUAGUAUGAGUGUCGUGCAGCCUCCAGGACCCCCCCUCCCGGGAAAGCCAUAGUGGUCUGCGGAACCGGUGAGUACACCGGAAUUGCCAGGACGACCGGGUCCUUUCUUGGAUUAACCCGCUCAAUGCCUGGAGAUUUGGGCGUGCCCCCGCAAGACUGCUAGCCGAGUAGUGUUGGGUCGCGAAAGGCCUUGUGGUACUGCCUGAUAGGGUGCUUGCGAGUGCCCCGGGAGGUCUCGUAGACCGUGCACCAUGAGCACGAAUCCU";

#define MutationRate 0.5f

	using namespace GIL;

	struct Sensor
	{
		int Start;
		int Len;
		float Fitness = 0.0f;
		int SelfBinding = 0;
		int AttenuatorBinding = 0;
		float FreeEnergy = 0.0f;

		Sensor() : Start(0), Len(0) {}
		Sensor(int start, int len) : Start(start), Len(len) {}
	};

	struct GenInfo
	{
		float AvgSelfBinding;
		float AvgAttenuatorBinding;
		float AvgFreeEnergy;
		int MaxBoundOverlap;
	};

	struct AvailableRegion
	{
		AvailableRegion(int start, int count) : Start(start), Count(count) {}

		int Start;
		int Count;
	};

	struct SensorInput
	{
		GenInfo* LastGeneration;
		Sensor* self;
		std::string& Target;
		std::vector<AvailableRegion>& UnboundRegions;

		SensorInput(GenInfo* lastGeneration, Sensor* Self, std::string& target, std::vector<AvailableRegion>& unboundRegions)
			: LastGeneration(lastGeneration), self(Self), Target(target), UnboundRegions(unboundRegions) {}
	};

	//Function defs
	void Mutate(Sensor& Sensor, int TarLen, float mutationRate, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap,
		std::vector<AvailableRegion>& UnboundRegions, std::uniform_real_distribution<float>& Dist, std::mt19937& RGen);
	Sensor NewSensor(std::string& Target, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, std::vector<AvailableRegion>& AvailableRegions);

	//Each IRES in the population is run as a thread pool task. This means that it takes advantage of threads up to n_threads == NumGenerations
	void* Evaluate(void* Params);

	int FindMatches(const std::string& Strand1, const std::string& Strand2, int MinChunkSize = 14);    //13 is our min chunk size, because that is one bigger than the binding region
	bool FindMatchByChunkSize(const std::string& Strand1, const std::string& Strand2, int ChunkSize);
	int DetectBinding(const std::string& NTs, int MinChunkSize = 3);    //Detects binding within the DNA sequence NTs

	void Quicksort(std::vector<SensorInput>& Vector);
	void Quicksort(std::vector<SensorInput>& Vector, int low, int high);

	Sensor CreateChildSensor(std::vector<Sensor>& Sensors, int TarLen, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap,
		std::vector<AvailableRegion>& AvailableRegions, std::uniform_real_distribution<float>& Dist, std::mt19937& RGen);

	void FixTargetFreedom(Sensor* sensor, int MaxBoundOverlap, std::vector<AvailableRegion>& AvailableRegions);


	void* CreateIRESThread(void* Params)
	{
		std::string* Output = new std::string();
		IRESParams* p = (IRESParams*)Params;
		(*Output) = CreateIRES(p->GenerationSize, p->NumGenerations, p->NewPerGeneration, p->PreservePerGeneration, p->MinAttenuatorLength, p->MaxAttenuatorLength, p->MaxBoundOverlap, p->UpdateGenerationNumber, p->Target);
		delete p;
		return Output;
	}

	//This one is run in whichever thread calls it, so you may want to call it in a new thread using CreateIRESThread
	std::string CreateIRES(int GenerationSize, int NumGenerations, int NewPerGeneration, int PreservePerGeneration, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, int* UpdateGenerationNumber, std::string& Target)
	{
		std::vector<AvailableRegion> UnboundRegions;
		//In a scope so TargetRNA gets deleted
		{
			//Before ANYTHING, we need to figure out which parts of Target are available (unbound)
			RNA TargetRNA(Target.c_str());
			TargetRNA.FoldSingleStrand(20.0f, 5, 5, "", 100, true);

			//Now loop through nucleotides and check if they're bound
			int NumUnbound = 0;
			for (int i = 0; i < Target.length(); ++i)
			{
				CDB_BuildInfo(TargetRNA.GetPair(i + 1));
				if (TargetRNA.GetPair(i + 1) == 0)
					++NumUnbound;
				else
				{
					//First check if at least MinAttenuatorLength nucleotides were unbound
					if (NumUnbound >= MinAttenuatorLength)
					{
						UnboundRegions.push_back({ i - NumUnbound, NumUnbound });
					}
					NumUnbound = 0;
				}
			}
			if (NumUnbound >= MinAttenuatorLength)
			{
				UnboundRegions.push_back({ static_cast<int>(Target.length() - 1) - NumUnbound, NumUnbound });
			}
		}

		//Early exit if there aren't any free regions
		if (UnboundRegions.size() == 0)
		{
			CDB_BuildError("Could not find free regions of at least {0} bp in target", MinAttenuatorLength);
			return "";
		}


		GenInfo LastGeneration;
		LastGeneration.MaxBoundOverlap = MaxBoundOverlap;
		std::vector<Sensor> Sensors;
		std::vector<Sensor> Sensors2;
		bool UseSensors1 = true;
		std::vector<SensorInput> Inputs;
		Sensors.reserve(GenerationSize);
		Inputs.reserve(GenerationSize);
		for (int i = 0; i < GenerationSize; ++i)    //Generate the starting sensors
		{
			Sensors.push_back(NewSensor(Target, MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, UnboundRegions));
		}
		for (int i = 0; i < GenerationSize; ++i)
		{
			Inputs.push_back(SensorInput(&LastGeneration, &Sensors[i], Target, UnboundRegions));
		}

		std::vector<std::future<void*>> Outputs;
		Outputs.reserve(GenerationSize);
		for (int i = 0; i < NumGenerations; ++i)
		{
			*UpdateGenerationNumber = i;
			for (SensorInput Input : Inputs)
			{
				auto Added = CDB::ThreadPool::Get()->AddStandardTask(Evaluate, &Input);
				if (!Added.first)
				{
					CDB_EditorError("Could not add genetic algorithm to thread pool, thread pool is closed");
					return "";
				}
				Outputs.push_back(std::move(Added.second));
			}
			for (std::future<void*>& Future : Outputs)
				Future.get();
			//We don't want to keep these futures
			Outputs.clear();

			//Now we have the fitness scores for the attenuators
			//We'll quicksort it so we can select the n best sensors for the next generation
			if (UseSensors1)
			{
				Sensors2.clear();
			}
			else
			{
				Sensors.clear();
			}

			int s = 0;
			if (PreservePerGeneration > 0)
			{
				Quicksort(Inputs);

				for (s; s < PreservePerGeneration && s < GenerationSize; ++s)
				{
					if (UseSensors1)
					{
						Sensors2.push_back(*Inputs[i].self);
					}
					else
					{
						Sensors.push_back(*Inputs[i].self);
					}
				}
			}

			//Now generate a couple of new variants
			if (NewPerGeneration > 0)
			{
				for (int n = 0; n < NewPerGeneration && s + n < GenerationSize; ++n)
				{
					if (UseSensors1)
					{
						Sensors2.push_back(NewSensor(Target, MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, UnboundRegions));
					}
					else
					{
						Sensors.push_back(NewSensor(Target, MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, UnboundRegions));
					}
				}
				s += NewPerGeneration;
			}

			//Get the average values and total fitness
			float FitnessSum = 0.0f;
			LastGeneration.AvgFreeEnergy = 0.0f;
			for (SensorInput& si : Inputs)
			{
				if (si.self->Fitness > 0)
					FitnessSum += si.self->Fitness;

				LastGeneration.AvgFreeEnergy += si.self->FreeEnergy;
			}
			LastGeneration.AvgFreeEnergy = LastGeneration.AvgFreeEnergy / GenerationSize;
			std::mt19937 RandomGenerator(time(0));
			std::uniform_real_distribution<float> Dist(0.0, FitnessSum);

			for (s; s < GenerationSize; ++s)
			{
				if (UseSensors1)
				{
					Sensors2.push_back(CreateChildSensor(Sensors, Target.length(), MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, UnboundRegions, Dist, RandomGenerator));
				}
				else
				{
					Sensors.push_back(CreateChildSensor(Sensors2, Target.length(), MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, UnboundRegions, Dist, RandomGenerator));
				}
			}

			//Set the inputs for the next generation
			for (int s = 0; s < Inputs.size(); ++s)
			{
				if (UseSensors1)
				{
					Inputs[s].self = &Sensors2[s];
				}
				else
				{
					Inputs[s].self = &Sensors[s];
				}
			}

			//Flip UseSensors1
			UseSensors1 = !UseSensors1;
		}
		
		//Now get the best attenuator
		int BestIdx = 0;
		for (int i = 0; i < Inputs.size(); ++i)
		{
			if (Inputs[i].self->Fitness > Inputs[BestIdx].self->Fitness)
			{
				BestIdx = i;
			}
		}
		return Target.substr(Inputs[BestIdx].self->Start, Inputs[BestIdx].self->Len);
	}

	bool CheckAvailibility(int Start, int Len, int MaxBoundOverlap, std::vector<AvailableRegion>& AvailableRegions)
	{
		for (AvailableRegion& AR : AvailableRegions)
		{
			if (Start + MaxBoundOverlap >= AR.Start && Len - MaxBoundOverlap <= AR.Count)
				return true;
		}
		return false;
	}

	Sensor NewSensor(std::string& Target, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, std::vector<AvailableRegion>& AvailableRegions)
	{
		if (AvailableRegions.size() == 0)
			return Sensor(0, 1);

		//Transform to available regions
		int TotalAvailable = 0;
		for (AvailableRegion& AR : AvailableRegions)
			TotalAvailable += AR.Count;

		int Start = rand() % (TotalAvailable - MaxAttenuatorLength);
		int Len = rand() % (MaxAttenuatorLength - MinAttenuatorLength) + MinAttenuatorLength;

		//Now transform back to target
		for (AvailableRegion& AR : AvailableRegions)
		{
			//Does Start fall within the availible region
			if (Start < AR.Count)
			{
				Start = AR.Start + Start;

				//Check for overflow
				if (Start + Len > Target.size())
				{
					Start -= Start + Len - Target.size();
				}
				else if (Start - AR.Start + Len > AR.Count + MaxBoundOverlap)
				{
					Start -= (Start - AR.Start + Len) - (AR.Count + MaxBoundOverlap);
				}
				return Sensor(Start, Len);
			}
			else
				Start -= AR.Count;
		}

		return Sensor(Start, Len);
	}

	Sensor CreateChildSensor(std::vector<Sensor>& Sensors, int TarLen, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, 
		std::vector<AvailableRegion>& AvailableRegions, std::uniform_real_distribution<float>& Dist, std::mt19937& RGen)
	{
		float TargetFitness = Dist(RGen);
		float CurrentFitness = 0.0f;

		for (Sensor& s : Sensors)
		{
			if (s.Fitness < 0)
				continue;
			CurrentFitness += s.Fitness;
			if (CurrentFitness >= TargetFitness)
			{
				Sensor Output = s;
				Mutate(Output, TarLen, MutationRate, MinAttenuatorLength, MaxAttenuatorLength, MaxBoundOverlap, AvailableRegions, Dist, RGen);
				return Output;
			}
		}
		CDB_EditorError("Failed to create child sensor");
		return {};
	}

	void Mutate(Sensor& Sensor, int TarLen, float mutationRate, int MinAttenuatorLength, int MaxAttenuatorLength, int MaxBoundOverlap, 
		std::vector<AvailableRegion>& UnboundRegions, std::uniform_real_distribution<float>& Dist, std::mt19937& RGen)
	{
		float Mutate = Dist(RGen) / Dist.b();
		if (Mutate > mutationRate)
			return;
		//Normalize mutation rate
		Mutate = Mutate / mutationRate;

		/*
		There are four mutation options:
		1. Shift the attenuator down by 1
		2. Shift the attenuator up by 1
		3. Shift the attenuator length down by 1
		4. Shift the attenuator length up by 1
		*/

		if (Mutate < 0.25f)
		{
			Sensor.Start--;
		}
		else if (Mutate < 0.5f)
		{
			Sensor.Start++;
		}
		else if (Mutate < 0.75f)
		{
			Sensor.Len--;
		}
		else
		{
			Sensor.Len++;
		}

		//Now we need to clean the sensor to make sure all the values are in a valid range
		if (Sensor.Start < 0)
		{
			Sensor.Start = 0;
		}
		else if (Sensor.Len > MaxAttenuatorLength)
		{
			Sensor.Len--;
		}
		else if (Sensor.Len < MinAttenuatorLength)
		{
			Sensor.Len++;
		}
		else if (Sensor.Start + Sensor.Len > TarLen)
		{
			if (Mutate < 0.5f)
			{
				Sensor.Start--;
			}
			else
			{
				Sensor.Len--;
			}
		}

		//Finally, make sure the sensor targets an accessible region
		FixTargetFreedom(&Sensor, MaxBoundOverlap, UnboundRegions);
	}




	float GetSelfBinding(SensorInput* Input, const std::string& Attenuator);
	//float GetAttenuatorIRESBinding(SensorInput* Input, const std::string& Attenuator);
	float CheckInterference(const std::string& Attenuator);
	float GetStability(SensorInput* Input, const std::string& Attenuator);
	

	/*
	My thoughts on the genetic algorithm:
	1. By incentivising a big free energy difference between the bound and unbound states, we disincentivize attenuator self-binding and 
	   attenuator-IRES binding. This would increase the unbound free energy and decrease the bound free energy
	2. However, this incentivizes the "agent" to block the interference region from binding with the IRES. Because of this, I'll add a term that 
	   checks for Attenuator+Binding region self-binding
	*/
	void* Evaluate(void* Params)
	{
		SensorInput* Input = (SensorInput*)Params;
		std::string Attenuator = Input->Target.substr(Input->self->Start, Input->self->Len);
		
		float SelfBindingFitness = GetSelfBinding(Input, Attenuator);
		//float AtIRESBindingFitness = GetAttenuatorIRESBinding(Input, Attenuator);
		float FreeEnergyDifference = GetStability(Input, Attenuator);

		//Set fitness
		Input->self->Fitness = SelfBindingFitness + FreeEnergyDifference;

		return nullptr;
	}

	void FixTargetFreedom(Sensor* sensor, int MaxBoundOverlap, std::vector<AvailableRegion>& AvailableRegions)
	{
		if (AvailableRegions.size() == 0)
			return;
		//Find the region that is closest to the sensor's target
		int ClosestIdx = 0;
		int ClosestDistance = 999999999999999;
		for (int i = 0; i < AvailableRegions.size(); ++i)
		{
			//If the sensor falls within the region, return
			if (sensor->Start + MaxBoundOverlap >= AvailableRegions[i].Start && sensor->Len - MaxBoundOverlap <= AvailableRegions[i].Count)
				return;
			int Dist = abs(sensor->Start - AvailableRegions[i].Start);
			
			//Break if the distance is further than the closest distance
			if (Dist > ClosestDistance)
				break;
			ClosestIdx = i;
			ClosestDistance = Dist;
		}

		if (sensor->Start + MaxBoundOverlap < AvailableRegions[ClosestIdx].Start)
			--sensor->Start;
		else
			--sensor->Len;
	}


	float GetSelfBinding(SensorInput* Input, const std::string& Attenuator)
	{
		std::string AttenuatorAndBindingRegion = Attenuator + "GCAAGCACCCTAT";
		Input->self->SelfBinding = DetectBinding(AttenuatorAndBindingRegion);
		return -(4.0f * Input->self->SelfBinding - 4.0f * Input->LastGeneration->AvgSelfBinding);
	}

	float GetAttenuatorIRESBinding(SensorInput* Input, const std::string& Attenuator)
	{
		Input->self->AttenuatorBinding = FindMatches(IRES, Attenuator);
		return -(4.0f * Input->self->AttenuatorBinding - 4.0f * Input->LastGeneration->AvgAttenuatorBinding);
	}

	float CheckInterference(const std::string& Attenuator)
	{
		int Binding = FindMatches(IRES, "GCAAGCACCCTAT");
		return 8.0f * Binding;    //Preventing this from binding would have to give -104 kcals/mol for it to be worth it
	}


	//Evaluates the difference in stability between the bound and unbound states
	float GetStability(SensorInput* Input, const std::string& Attenuator)
	{
		//Assemble the IRES
		std::string CompleteSensor = IRES + utils::GetReverseComplement(Attenuator) + "GCAAGCACCCTAT";
		RNA Sensor(CompleteSensor.c_str());

		//Fold the IRES. We only want the MFE structure
		int Result = Sensor.FoldSingleStrand(20.0f, 5, 5, "", 100, true);

		if (Result != 0)    //If there was an error
		{
			//We can gracefully recover from this by returning an absurdly low fitness value to make sure this strand is eliminated
			CDB_EditorError("Error folding IRES sensor: {0}", RNA::GetErrorMessage(Result));
			return -999999999999.0f;
		}

		float FreeEnergy = Sensor.CalculateFreeEnergy();
		if (Sensor.GetErrorCode() != 0)
		{
			CDB_EditorError("Failed to calculate IRES sensor free energy: {0}", RNA::GetErrorMessage(Sensor.GetErrorCode()));
			return -999999999999.0f;
		}


		HybridRNA Duplex(CompleteSensor.c_str(), utils::GetReverseComplement(Attenuator).c_str());

		Result = Duplex.FoldBimolecular(10.0f, 5, 0, "", 100);

		if (Result != 0)
		{
			CDB_EditorError("Error folding IRES-attenuator duplex: {0}", Duplex.GetErrorMessage(Result));
			return -9999999999999999.0f;
		}

		float DuplexFreeEnergy = Duplex.CalculateFreeEnergy();

		if (Duplex.GetErrorCode() != 0)
		{
			CDB_EditorError("Failed to calculate duplex free energy: {0}", Duplex.GetErrorMessage(Duplex.GetErrorCode()));
			return -999999999999999.0f;
		}

		//Now calculate the difference and return it (we subtract DuplexFreeEnergy because they're both theoretically negative)
		//Having bigger numbers be more fit makes things easier for us
		Input->self->FreeEnergy = FreeEnergy - DuplexFreeEnergy;

		return Input->self->FreeEnergy - Input->LastGeneration->AvgFreeEnergy;
	}




	bool DetectBindingInChunks(const std::string& NTs, size_t ChunkSize);

	//Detects base pairing in sequence NTs in chunks of at least MinChunkSize
	int DetectBinding(const std::string& NTs, int MinChunkSize)
	{
		for (size_t ChunkSize = NTs.size() / 2; ChunkSize >= MinChunkSize; --ChunkSize)
		{
			if (DetectBindingInChunks(NTs, ChunkSize))
			{
				return ChunkSize;
			}
		}
		return 0;
	}

	//Detects if any chunks of size ChunkSize bind to the rest of NTs
	bool DetectBindingInChunks(const std::string& NTs, size_t ChunkSize)
	{
		//Move along the strand and find if any chunks of ChunkSize have an antisense sequence later on
		for (size_t i = 0; i < NTs.size() - ChunkSize; ++i)
		{
			std::string Chunk = NTs.substr(i, ChunkSize);
			Chunk = utils::GetReverseComplement(Chunk);
			if (NTs.find(Chunk, i + ChunkSize) != -1)
			{
				return true;
			}
		}
		return false;
	}


	bool DetectBindingInChunks(const std::string& NTs, size_t ChunkSize);

	int FindMatches(const std::string& Strand1, const std::string& Strand2, int MinChunkSize)
	{
		for (size_t ChunkSize = Strand1.length(); ChunkSize >= MinChunkSize; --ChunkSize)
		{
			if (FindMatchByChunkSize(Strand1, Strand2, ChunkSize))
				return ChunkSize;
		}
		return 0;
	}

	bool FindMatchByChunkSize(const std::string& Strand1, const std::string& Strand2, int ChunkSize)
	{
		for (size_t i = 0; i < Strand1.size() - ChunkSize; ++i)
		{
			std::string tmp = Strand1.substr(i, ChunkSize);
			if (Strand2.find(utils::GetReverseComplement(tmp)) != -1)
				return true;
		}
		return false;
	}



	//_________________________________________________________________________________________________________
	// Quicksort implementation for sorting the attenuators
	//_________________________________________________________________________________________________________

	void swap(SensorInput* a, SensorInput* b)
	{
		Sensor* S1Sensor = a->self;
		a->self = b->self;
		b->self = S1Sensor;
	}

	int partition(std::vector<SensorInput>& Vector, int low, int high)
	{
		int pivot = Vector[high].self->Fitness;
		int i = low - 1;

		for (int j = low; j <= high - 1; ++j)
		{
			if (Vector[j].self->Fitness <= pivot)
			{
				++i;
				swap(&Vector[i], &Vector[j]);
			}
		}
		swap(&Vector[i + 1], &Vector[high]);
		return i + 1;
	}

	void Quicksort(std::vector<SensorInput>& Vector) { Quicksort(Vector, 0, Vector.size() - 1); }

	void Quicksort(std::vector<SensorInput>& Vector, int low, int high)
	{
		if (low < high)
		{
			int pi = partition(Vector, low, high);

			Quicksort(Vector, low, pi - 1);
			Quicksort(Vector, pi + 1, high);
		}
	}
}