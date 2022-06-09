#include "TRAPzyme.h"

#include <stdlib.h>

namespace sensing
{
	//__________________________________________________________________________________________________________________________
	// Operation SetTRAPzymeTarget
	//__________________________________________________________________________________________________________________________

	std::string TargetText = "";
	std::pair<std::vector<GIL::Parser::Region>, std::string> Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
	{
		TargetText = GetInnerCode(Params).second;    //Sets the Target to the compiles inner tokens
		return std::pair<std::vector<GIL::Parser::Region>, std::string>();
	}

	GIL::Sequence* SetTRAPzymeTarget::self = nullptr;
	GIL::Sequence* SetTRAPzymeTarget::GetPtr()
	{
		if (self == nullptr)
		{
			self = new SetTRAPzymeTarget();
		}
		return self;
	}




	//__________________________________________________________________________________________________________________________
	// Operation GenTZFromAttenuator
	//__________________________________________________________________________________________________________________________
	
	using namespace GIL;

	//Func defs
	std::pair<std::string, std::string> FindValidAttenuator(std::string& Tar, int start = 0, int Binding = 100000, float OtherGC = 0.0f);    //Binding is big so the first call returns something
	std::pair<std::string, std::string> FindValidV2Attenuator(std::string& Tar, int start = 0, int Binding = 100000, float OtherGC = 0.0f);
	int DetectBinding(std::string& NTs, int MinChunkSize = 3);    //Detects binding within the DNA sequence NTs

	std::string FindBestTRAPzymeBody(std::string& AntiAttenuator, int Start = 0, int BestNumMatches = 100000);
	std::string FindBestTRAPzymeV2Body(std::string& AntiAttenuator, int Start = 0, int BestNumMatches = 100000);
	std::string GenTRAPzymeBody(std::string Target, std::string& AntiAttenuator);
	std::string GenTRAPzymeV2Body(std::string Target, std::string& AntiAttenuator);

	int FindMatches(std::string& Strand1, std::string& Strand2, int MinChunkSize = 7);    //7 is our min chunk size, because that is one bigger than the binding region

	std::pair<std::vector<GIL::Parser::Region>, std::string> Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
	{
		//Compile the attenuator
		std::string Attenuator = GetInnerCode(Params).second;
		//Find a valid Attenuator sequence (aa = "angarnnnnnnnnnnnnnnn")
		auto Result = FindValidAttenuator(Attenuator);
		if (Result.first == "")    //No attenuator found
		{
			CDB_BuildError("Cannot find valid attenuator sequence in sequence {0}", Attenuator);
			return std::pair<std::vector<GIL::Parser::Region>, std::string>();
		}

		std::string TZyme = FindBestTRAPzymeBody(Result.second);
		return { { GIL::Parser::Region("TRAPzyme generated by GenTZFromAttenuator", 0, TZyme.size()) }, TZyme + Result.second };
	}



	std::string FindBestTRAPzymeBody(std::string& Attenuator, int Start, int BestNumMatches)
	{
		//Now find a valid target sequence. It's probably fine if the target kinda binds to itself
		auto Target = utils::Find(TargetText, "nnnnthnnnnnnnnnnnnnnnnnnnn", Start);
		if (Target.first == "")
		{
			CDB_BuildError("Could not find TRAPzyme target");
			return "";
		}
		std::string TZymeCandidate = GenTRAPzymeBody(Target.first.substr(0, 12), Attenuator);

		//Find the biggest chunk of the AA sequence binds to the TZymeCandidate 
		int NumMatches = FindMatches(Attenuator, TZymeCandidate);
		if (NumMatches == 0)
			return TZymeCandidate;
		if (NumMatches < BestNumMatches)
		{
			std::string OtherCandidate = FindBestTRAPzymeBody(Attenuator, Target.second + 1, NumMatches);
			if (OtherCandidate != "")
				return OtherCandidate;
			CDB_BuildWarning("Generated TRAPzyme with attenuator-ribozyme binding in chunk(s) of {0}", NumMatches);
			return TZymeCandidate;
		}
		return "";
	}

	std::string GenTRAPzymeBody(std::string Target, std::string& AntiAttenuator)
	{

		/*
		TRAPzyme: Current section 
		______
		     | 
		   o-<>[-o]
		*/
		std::string tmp = Target.substr(6, 4);
		std::string TRAPzyme = utils::GetReverseComplement(tmp) + "CTG";

		/*
		TRAPzyme: Current section
		______
	     	 |
		  o-[<>]-o
		*/
		tmp = AntiAttenuator.substr(AntiAttenuator.size() - 8);
		TRAPzyme += utils::GetReverseComplement(tmp);

		/*
		TRAPzyme: Current section
		______
			 |
		 [o-]<>-o
		*/
		tmp = tmp.substr(0, 4);    //Get arm

		float GC = utils::GetGCRAtio(tmp);
		float AG = utils::GetAGRatio(tmp);

		if (GC > 0.5f)    //We should use AT
		{
			if (AG > 0.5)
			{
				TRAPzyme += "ttta";
			}
			else
			{
				TRAPzyme += "aaat";
			}
		}
		else    //We should use GC
		{
			if (AG > 0.5)
			{
				TRAPzyme += "cccg";
			}
			else
			{
				TRAPzyme += "gggc";
			}
		}

		TRAPzyme += utils::GetReverseComplement(tmp) + "GAAA";

		/*
		TRAPzyme: Current section
		______
			[|]
		   o-<>-o
		*/

		tmp = Target.substr(0, 5);
		TRAPzyme += utils::GetReverseComplement(tmp);

		return TRAPzyme;
	}


	bool FindMatchByChunkSize(std::string& Strand1, std::string& Strand2, int ChunkSize);

	int FindMatches(std::string& Strand1, std::string& Strand2, int MinChunkSize)
	{
		for (size_t ChunkSize = Strand1.length(); ChunkSize >= MinChunkSize; --ChunkSize)
		{
			if (FindMatchByChunkSize(Strand1, Strand2, ChunkSize))
				return ChunkSize;
		}
		return 0;
	}

	std::pair<std::string, std::string> FindValidAttenuator(std::string& Tar, int start, int binding, float OtherGC)
	{
		auto result = utils::Find(Tar, "angarnnnnnnnnnnnnnnn", start);    //Find attenuator sequence
		if (result.second == -1)    //Return nothing if the attenuator wasn't found
		{
			return { "", "" };
		}
		std::string AntiAttenuator = utils::GetReverseComplement(result.first);
		int Binding = DetectBinding(AntiAttenuator);    //Returns the chunk size where binding was first found or 0 if none was found
		if (Binding != 0)
		{
			if (Binding < binding)    //If this has more binding than whatever function called it, see if there's a better one later on
			{
				//Try to find a better candidate
				std::pair<std::string, std::string> OtherOption = FindValidAttenuator(Tar, result.second + 1, Binding);
				if (OtherOption.first != "")
					return OtherOption;
				CDB_BuildWarning("Found Antiattenuator with self-binding in chunk(s) of {0}", Binding);
				return { result.first, AntiAttenuator };
			}
			else if (Binding == binding)    //All else being equal, optimize for a higher or lower GC content
			{
				float SelfGC = utils::GetGCRAtio(AntiAttenuator);
				std::pair<std::string, std::string> OtherOption = FindValidAttenuator(Tar, result.second + 1, Binding, SelfGC);
				if (OtherOption.first != "")
					return OtherOption;

				double Other = pow(OtherGC - 0.5f, 2);
				double Self = pow(SelfGC - 0.5f, 2);

				if (Other > Self)
					return { "", "" };
				return { result.first, AntiAttenuator };
			}
			return FindValidAttenuator(Tar, result.second + 1, binding);    //If there's a better attenuator later on, return it
		}
		//If there's no binding, return this candidate
		return { result.first, AntiAttenuator };
	}

	bool DetectBindingInChunks(std::string& NTs, size_t ChunkSize);

	//Detects base pairing in sequence NTs in chunks of at least MinChunkSize
	int DetectBinding(std::string& NTs, int MinChunkSize)
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

	bool FindMatchByChunkSize(std::string& Strand1, std::string& Strand2, int ChunkSize)
	{
		for (size_t i = 0; i < Strand1.size() - ChunkSize; ++i)
		{
			std::string tmp = Strand1.substr(i, ChunkSize);
			if (Strand2.find(utils::GetReverseComplement(tmp)) != -1)
				return true;
		}
		return false;
	}

	//Detects if any chunks of size ChunkSize bind to the rest of NTs
	bool DetectBindingInChunks(std::string& NTs, size_t ChunkSize)
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



	GIL::Sequence* GenTZFromAttenuator::self = nullptr;
	GIL::Sequence* GenTZFromAttenuator::GetPtr()
	{
		if (self == nullptr)
		{
			self = new GenTZFromAttenuator();
		}
		return self;
	}

	std::pair<std::vector<GIL::Parser::Region>, std::string> Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
	{
		/*
		Pattern is angarthvvvsvvvvvvvvvvvvvvvv

		Breakdown:
		vvvvvvvvvvvvvvvvsvvvhtragna
						^	 ^
						|	 |Only possibe cut site
						|Attenuator must be created to include the cut site, because the only other option is blocked by this s
		*/

		std::string pattern = "angarthvvvsvvvvvvvvvvvvvvvv";
		std::string Candidate;
		Candidate = utils::GenWithPattern(pattern);
		CDB_BuildInfo("Candidate");

		//HACK: Definitely room for optimization
		while (DetectBinding(Candidate) != 0)
			Candidate = utils::GenWithPattern(pattern);


		return { {}, Candidate };
	}



	GIL::Sequence* CreateAttenuator::self = nullptr;
	GIL::Sequence* CreateAttenuator::GetPtr()
	{
		if (self == nullptr)
		{
			self = new CreateAttenuator();
		}
		return self;
	}



	std::pair<std::vector<GIL::Parser::Region>, std::string> Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
	{
		//Compile the attenuator
		std::string Attenuator = GetInnerCode(Params).second;
		//Find a valid Attenuator sequence (aa = "angarnnnnnnnnnnnnnnn")
		auto Result = FindValidV2Attenuator(Attenuator);
		if (Result.first == "")    //No attenuator found
		{
			CDB_BuildError("Cannot find valid attenuator sequence in sequence {0}", Attenuator);
			return std::pair<std::vector<GIL::Parser::Region>, std::string>();
		}

		std::string TZyme = FindBestTRAPzymeV2Body(Result.second);
		return { { GIL::Parser::Region("TRAPzyme generated by GenTZFromAttenuator", 0, TZyme.size()) }, Result.second + TZyme };
	}



	GIL::Sequence* GenTZV2FromAttenuator::self = nullptr;
	GIL::Sequence* GenTZV2FromAttenuator::GetPtr()
	{
		if (self == nullptr)
			self = new GenTZV2FromAttenuator();
		return self;
	}

	std::pair<std::string, std::string> FindValidV2Attenuator(std::string& Tar, int start, int binding, float OtherGC)
	{
		std::pair<std::string, int> Result = utils::Find(Tar, "nnnnnnnnnnnnnnnnnnnn");
		if (Result.second == -1)
		{
			return { "", "" };
		}

		int Binding = DetectBinding(Result.first);
		int NextIdx = Result.second + 1;
		std::pair<std::string, int> NextOption;

		FindCandidate:
		if (Binding > 0)
		{
			NextOption = utils::Find(Tar, "nnnnnnnnnnnnnnnnnnnn", NextIdx);    //Find an alternative AA strand
			if (NextOption.first != "")
			{
				int OtherOptionBinding = DetectBinding(NextOption.first);
				NextIdx = NextOption.second + 1;

				if (OtherOptionBinding == 0)
				{
					Result = NextOption;
				}
				else if (OtherOptionBinding < Binding)
				{
					Result = NextOption;
					goto FindCandidate;
				}
				else
				{
					goto FindCandidate;
				}
			}			
		}

		//Now generate the rest of the attenuator
		std::string CurrentAttenuator = "AAAAA" + Result.first;

		//I'm too lazy to come up with a new variable name
		{
			std::string Pattern = "nnnytcnt";
			std::string Result = utils::GenWithPattern(Pattern);
			std::string NextOption = "";

			Binding = FindMatches(Result, CurrentAttenuator, 3);

			FindNewCandidate:
			if (Binding > 0)
			{
				NextOption = utils::GenWithPattern(Pattern);

				int OtherOptionBinding = FindMatches(NextOption, CurrentAttenuator, 3);
				if (OtherOptionBinding == 0)
				{
					std::string FullAttenuator = NextOption + CurrentAttenuator;
					int b = DetectBinding(FullAttenuator);
					if (b == 0)
						Result = NextOption;
					else
						goto FindNewCandidate;
				}
				else
				{
					goto FindNewCandidate;
				}
			}

			CurrentAttenuator += Result;
		}
		return { Result.first, CurrentAttenuator };
	}

	std::string FindBestTRAPzymeV2Body(std::string& Attenuator, int Start, int BestNumMatches)
	{
		//Now find a valid target sequence. It's probably fine if the target kinda binds to itself
		auto Target = utils::Find(TargetText, "nnnnthnnnnnnnnnnnnnnnnnnnn", Start);
		if (Target.first == "")
		{
			CDB_BuildError("Could not find TRAPzyme target");
			return "";
		}
		std::string TZymeCandidate = GenTRAPzymeV2Body(Target.first.substr(0, 12), Attenuator);

		//Find the biggest chunk of the AA sequence binds to the TZymeCandidate 
		int NumMatches = FindMatches(Attenuator, TZymeCandidate);
		if (NumMatches == 0)
			return TZymeCandidate;
		if (NumMatches < BestNumMatches)
		{
			std::string OtherCandidate = FindBestTRAPzymeBody(Attenuator, Target.second + 1, NumMatches);
			if (OtherCandidate != "")
				return OtherCandidate;
			CDB_BuildWarning("Generated TRAPzyme with attenuator-ribozyme binding in chunk(s) of {0}", NumMatches);
			return TZymeCandidate;
		}
		return "";
	}

	std::string GenTRAPzymeV2Body(std::string Target, std::string& Attenuator)
	{
		/*
		TRAPzyme: Current section
		______
			 |
		   o-<>[-o]
		*/
		std::string tmp = Target.substr(6, 4);
		std::string TRAPzyme = utils::GetReverseComplement(tmp) + "CTG";

		/*
		TRAPzyme: Current section
		______
			 |
		  o-[<>]-o
		*/
		tmp = Attenuator.substr(Attenuator.size() - 8);
		TRAPzyme += utils::GetReverseComplement(tmp);

		/*
		TRAPzyme: Current section
		______
			 |
		 [o-]<>-o
		*/
		tmp = tmp.substr(0, 4);    //Get arm

		float GC = utils::GetGCRAtio(tmp);
		float AG = utils::GetAGRatio(tmp);

		if (GC > 0.5f)    //We should use AT
		{
			if (AG > 0.5)
			{
				TRAPzyme += "ttta";
			}
			else
			{
				TRAPzyme += "aaat";
			}
		}
		else    //We should use GC
		{
			if (AG > 0.5)
			{
				TRAPzyme += "cccg";
			}
			else
			{
				TRAPzyme += "gggc";
			}
		}

		TRAPzyme += utils::GetReverseComplement(tmp) + "GAAA";

		/*
		TRAPzyme: Current section
		______
			[|]
		   o-<>-o
		*/

		tmp = Target.substr(0, 5);
		TRAPzyme += utils::GetReverseComplement(tmp);

		return TRAPzyme;
	}

	std::string GenTarget(GIL::Parser::Project* Proj)
	{
		std::string pattern = "nnnnthnnnnnnnnnnnnnnnnnnnn";
		std::string Target = utils::GenWithPattern(pattern);
		if (Proj->Attributes["TZT_AVOID_ATG"] != "true" || (Target.find("atg") == -1 && Target.find("ATG") == -1))
		{
			return Target;
		}
		return GenTarget(Proj);
	}

	std::pair<std::vector<GIL::Parser::Region>, std::string> Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params)
	{
		return { {}, GenTarget(Proj) };
	}



	GIL::Sequence* GenTZTarget::self = nullptr;
	GIL::Sequence* GenTZTarget::GetPtr()
	{
		if (self == nullptr)
			self = new GenTZTarget();
		return self;
	}
}