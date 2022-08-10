#include "GIL.h"


#define IsStartCodon(Text, i) (Text[i] == 'a' || Text[i] == 'A') && (Text[i + 1] == 't' || Text[i + 1] == 'T') && (Text[i + 2] == 'g' || Text[i + 2] == 'G')

bool HasStartCodon(std::string& Text, int i, int length)
{
	for (int idx = 0; idx < length - 3; ++idx)
	{
		if (IsStartCodon(Text, i))
			return true;
	}
	return false;
}


#define shRNASegmentLength 19
#define HairpinLoopSize 9

class shRNA_Sequence : public GIL::Sequence
{
	virtual void Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& Context)
	{
		//shRNA consists of a sequence, its complement, and a hairpin region

		//Get the sequence (from $InnerCode)
		GIL::InnerCode* Code = (GIL::InnerCode*)Params["InnerCode"].Seq;

		//Advance until we reach a window without a stop codon
		int i = 0;
		for (i; i < Code->m_InnerCode.second.length() && HasStartCodon(Code->m_InnerCode.second, i, shRNASegmentLength); ++i)
			continue;

		std::string Subsegment1 = Code->m_InnerCode.second.substr(i, shRNASegmentLength);
		//Create bulges
		switch (Subsegment1[10])
		{
		case 'a':
		case 'A':
			Subsegment1[i] = 't';
			break;
		case 't':
		case 'T':
			Subsegment1[i] = 'a';
			break;
		case 'c':
		case 'C':
			Subsegment1[i] = 'g';
			break;
		case 'g':
		case 'G':
			Subsegment1[i] = 'c';
			break;
		}

		std::string Complement = GIL::utils::GetReverseComplement(Subsegment1);
		std::string loop = GIL::utils::GenRandomBases(HairpinLoopSize);

		Context.OutputString->append(Subsegment1 + loop + Complement + "uu");
	}

	virtual void Save(std::ofstream& OutputFile) { }
	virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) {}
};


class RegulationModule : public GIL::GILModule
{
public:
	virtual GIL::Sequence* GetSequence(std::string name)
	{
		if (name == "Gen_shRNASequence")
			return new shRNA_Sequence();
		else
		{
			//Terminate compilation
			CDB_BuildError("Sequence \"{0}\" does not exist in regulation module", name);
			throw GIL::GILException();
		}
	}

	virtual GIL::Sequence* GetOperation(std::string name) override { return nullptr; }
};

ExposeModuleToGIL(RegulationModule)