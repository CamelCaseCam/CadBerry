#include <gilpch.h>
#include "Compiler.h"
#include "CodonEncoding.h"

namespace GIL
{
	namespace Compiler
	{
		using namespace GIL::Lexer;
		using namespace GIL::Parser;

		//Function definitions
		void AminosToDNA(std::string& DNA, std::string aminos, Project* Proj, CodonEncoding& CurrentEncoding);

		std::pair<std::vector<Region>, std::string> Compile(Project* Proj, std::vector<Token*>* Tokens)
		{
			if (Tokens == nullptr)
			{
				Tokens = &Proj->Main;
			}
			std::vector<Region> Output;
			CodonEncoding CurrentEncoding(Proj->TargetOrganism);
			
			Region CurrentRegion;
			std::string Code;
			CurrentRegion.Name = "Main";
			for (int i = 0; i < (*Tokens).size(); ++i)
			{
				Token* t = (*Tokens)[i];
				switch (t->TokenType)
				{
				case LexerToken::AMINOS:
					AminosToDNA(Code, t->Value, Proj, CurrentEncoding);
					break;
				case LexerToken::DNA:
					Code += t->Value;
					break;
				case LexerToken::BEGINREGION:
					++i;
					if ((*Tokens)[i]->TokenType == LexerToken::IDENT)
					{
						if (Code.length() != 0 || CurrentRegion.Name != "Main")
						{
							CurrentRegion.End = Code.length();
							Output.push_back(CurrentRegion);
						}
						CurrentRegion = Region();
						CurrentRegion.Name = (*Tokens)[i]->Value;
						CurrentRegion.Add(Code.length());
						break;
					}
					CDB_BuildError("#BeginRegion not followed by ident");
					break;
				case LexerToken::ENDREGION:
					CurrentRegion.End = Code.length();
					Output.push_back(CurrentRegion);
					CurrentRegion = Region();
					CurrentRegion.Name = "Main";
					break;
				case LexerToken::IDENT:
				{
					CurrentRegion.End = Code.length();
					Output.push_back(CurrentRegion);

					std::vector<Region>* SeqRegions = Proj->Sequences[t->Value]->GetRegions(Proj);
					int Start = Code.length();
					Code += *Proj->Sequences[t->Value]->GetCode(Proj);
					for (Region r : *SeqRegions)
					{
						r.Add(Start);
						Output.push_back(r);
					}
					break;
				}
				case LexerToken::CALLOP:
				{
					CurrentRegion.End = Code.length();
					Output.push_back(CurrentRegion);
					++i;
					std::vector<Token*> InsideTokens = GetInsideTokens(*Tokens, i);

					//TODO: Test if this is safe, I think the parser checks if the ops exist, but it might not
					auto Ptr = Proj->Operations[t->Value];
					auto output = Ptr->Get(InsideTokens, Proj);
					int Start = Code.length();
					Code += output.second;
					for (Region r : output.first)
					{
						if (r.Name == "Main")
						{
							r.Name = t->Value;
						}
						r.Add(Start);
						Output.push_back(r);
					}
					break;
				}
				default:
					break;
				}
			}
			CurrentRegion.End = Code.length();
			Output.push_back(CurrentRegion);

			return std::pair<std::vector<Region>, std::string>(Output, Code);
		}

		void AminosToDNA(std::string& DNA, std::string aminos, Project* Proj, CodonEncoding& CurrentEncoding)
		{
			DNA.reserve(DNA.length() + (aminos.length() * 3));

			for (char a : aminos)
			{
				DNA += *CurrentEncoding.GetFromLetter(a);
			}
		}
	}
}