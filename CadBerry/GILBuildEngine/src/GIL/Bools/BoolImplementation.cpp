#include <gilpch.h>
#include "BoolImplementation.h"

#include "GIL/Parser/Project.h"
#include "GIL/SaveFunctions.h"

namespace GIL
{
	namespace Parser
	{
		void BoolImplementation::Save(std::ofstream& OutputFile, Parser::Project* Proj)
		{
			Alloc->Save(OutputFile, Proj);
			Dealloc->Save(OutputFile, Proj);
			
			Set->Save(OutputFile, Proj);
			Use->Save(OutputFile, Proj);
			
			NumAvailable->Save(OutputFile, Proj);

			And->Save(OutputFile, Proj);
			Or->Save(OutputFile, Proj);
			Not->Save(OutputFile, Proj);

			OutputFile.write((char*)&this->XorAvailable, sizeof(bool));
			if (this->XorAvailable)
			{
				Xor->Save(OutputFile, Proj);
			}
			
			OutputFile.write((char*)&this->NandAvailable, sizeof(bool));
			if (this->NandAvailable)
			{
				Nand->Save(OutputFile, Proj);
			}

			OutputFile.write((char*)&this->NorAvailable, sizeof(bool));
			if (this->NorAvailable)
			{
				Nor->Save(OutputFile, Proj);
			}

			OutputFile.write((char*)&this->XnorAvailable, sizeof(bool));
			if (this->XnorAvailable)
			{
				Xnor->Save(OutputFile, Proj);
			}

			OutputFile.write((char*)&this->ChainedAndAvailable, sizeof(bool));
			if (this->ChainedAndAvailable)
			{
				ChainedAnd->Save(OutputFile, Proj);
			}

			OutputFile.write((char*)&this->ChainedOrAvailable, sizeof(bool));
			if (this->ChainedOrAvailable)
			{
				ChainedOr->Save(OutputFile, Proj);
			}
			
			SaveSize(this->AdditionalSequences.size(), OutputFile);
			for (auto& seq : this->AdditionalSequences)
			{
				SaveString(seq.first, OutputFile);
				seq.second->Save(OutputFile, Proj);
			}

			SaveString(this->name, OutputFile);
		}
		
		BoolImplementation* BoolImplementation::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			BoolImplementation* impl = new BoolImplementation();
			impl->Alloc = Sequence::LoadSequence(InputFile, Proj);
			impl->Dealloc = Sequence::LoadSequence(InputFile, Proj);
			
			impl->Set = Sequence::LoadSequence(InputFile, Proj);
			impl->Use = Sequence::LoadSequence(InputFile, Proj);
			
			impl->NumAvailable = Sequence::LoadSequence(InputFile, Proj);
			
			impl->And = Sequence::LoadSequence(InputFile, Proj);
			impl->Or = Sequence::LoadSequence(InputFile, Proj);
			impl->Not = Sequence::LoadSequence(InputFile, Proj);
			
			InputFile.read((char*)&impl->XorAvailable, sizeof(bool));
			if (impl->XorAvailable)
			{
				impl->Xor = Sequence::LoadSequence(InputFile, Proj);
			}
			
			InputFile.read((char*)&impl->NandAvailable, sizeof(bool));
			if (impl->NandAvailable)
			{
				impl->Nand = Sequence::LoadSequence(InputFile, Proj);
			}
			
			InputFile.read((char*)&impl->NorAvailable, sizeof(bool));
			if (impl->NorAvailable)
			{
				impl->Nor = Sequence::LoadSequence(InputFile, Proj);
			}

			InputFile.read((char*)&impl->XnorAvailable, sizeof(bool));
			if (impl->XnorAvailable)
			{
				impl->Xnor = Sequence::LoadSequence(InputFile, Proj);
			}
			
			InputFile.read((char*)&impl->ChainedAndAvailable, sizeof(bool));
			if (impl->ChainedAndAvailable)
			{
				impl->ChainedAnd = Sequence::LoadSequence(InputFile, Proj);
			}
			
			InputFile.read((char*)&impl->ChainedOrAvailable, sizeof(bool));
			if (impl->ChainedOrAvailable)
			{
				impl->ChainedOr = Sequence::LoadSequence(InputFile, Proj);
			}
			
			size_t NumAdditional = LoadSizeFromFile(InputFile);
			for (size_t i = 0; i < NumAdditional; i++)
			{
				std::string name;
				LoadStringFromFile(name, InputFile);
				Sequence* seq = Sequence::LoadSequence(InputFile, Proj);
				impl->AdditionalSequences[name] = seq;
			}

			LoadStringFromFile(impl->name, InputFile);
			return impl;
		}
	}
}