#include <gilpch.h>
#include "Compiler.h"
#include "CodonEncoding.h"
#include "GIL/Lexer/Lexer.h"
#include "GIL/Lexer/LexerMacros.h"
#include "GIL/Modules/GILModule.h"
#include "CompilerMacros.h"

#include "GIL/Utils/Utils.h"
#include "GIL/RestrictionSites.h"

#include "CadBerry.h"
#include "CadBerry/SharedLib.h"

namespace GIL
{
	namespace Compiler
	{
		using namespace GIL::Lexer;
		using namespace GIL::Parser;

		std::map<std::string, GILModule*> Modules;

		//Function definitions
		void AminosToDNA(std::string& DNA, std::string& aminos, Project* Proj, CodonEncoding& CurrentEncoding);
		void AminosIDXToDNA(std::string& DNA, std::string& aminos, std::vector<int>& Idxs, Project* Proj, CodonEncoding& CurrentEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj, CodonEncoding& OriginEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj);
		void ImportFile(std::string& Path, Project* Proj);
		void LinkDLL(std::string& Path, Project* Proj);

		std::pair<std::vector<Region>, std::string> Compile(Project* Proj, std::vector<Token*>* Tokens)
		{
			if (Tokens == nullptr)
			{
				Tokens = &Proj->Main;
			}
			std::vector<Region> Output;
			CodonEncoding CurrentEncoding(Proj->TargetOrganism);

			for (std::string& s : Proj->Imports)
			{
				if (!Proj->Namespaces.contains(s))
				{
					ImportFile(s, Proj);
				}
			}

			for (std::string& s : Proj->Usings)
			{
				LinkDLL(s, Proj);
			}

			Region CurrentRegion;
			std::string Code;
			CurrentRegion.Name = "Main";
			CurrentRegion.Start = 1;
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
						if (CurrentRegion.Start != Code.length() || CurrentRegion.Name != "Main")
						{
							CurrentRegion.End = Code.length();
							Output.push_back(CurrentRegion);
						}
						CurrentRegion = Region();
						CurrentRegion.Name = (*Tokens)[i]->Value;
						CurrentRegion.Start = Code.length();
						break;
					}
					CDB_BuildError("#BeginRegion not followed by ident");
					break;
				case LexerToken::ENDREGION:
					CurrentRegion.End = Code.length();
					Output.push_back(CurrentRegion);
					CurrentRegion = Region();
					CurrentRegion.Name = "Main";
					CurrentRegion.Start = Code.length();
					break;
				case LexerToken::IDENT:
				{
					//Check if main region has anything in it
					if ((CurrentRegion.Start != Code.length() && Code.length() != 0) || CurrentRegion.Name != "Main")
					{
						CurrentRegion.End = Code.length();
						Output.push_back(CurrentRegion);
						CurrentRegion = Region();
						CurrentRegion.Name = "Main";
					}

					GIL::Sequence* Seq = Proj->GetSeq(Tokens, i, &Modules);
					if (Seq == nullptr)    //Make sure sequence exists
					{
						CDB_BuildError("Sequence \"{0}\" does not exist", t->Value);
						break;
					}

					//Sequence exists
					std::vector<Region>* SeqRegions = Seq->GetRegions(Proj);
					int Start = Code.length();
					Code += *Proj->GetSeq(Tokens, i, &Modules)->GetCode(Proj);
					for (Region r : *SeqRegions)
					{
						if (r.Start == 0)
						{
							r.Start = 1;
						}
						r.Add(Start);
						Output.push_back(std::move(r));
					}
					CurrentRegion.Start = Code.length();
					break;
				}
				case LexerToken::CALLOP:
				{
					//Check if main region has anything in it
					if ((CurrentRegion.Start != Code.length() && Code.length() != 0) || CurrentRegion.Name != "Main")
					{
						CurrentRegion.End = Code.length();
						Output.push_back(CurrentRegion);
						CurrentRegion = Region();
						CurrentRegion.Name = "Main";
					}
					int OldIdx = i;
					++i;
					std::vector<Token*> InsideTokens = GetInsideTokens(*Tokens, i);

					auto Ptr = Proj->GetOp(Tokens, OldIdx, &Modules);
					if (Ptr == nullptr)    //Make sure the operation exists
					{
						CDB_BuildError("Operation \"{0}\" does not exist", t->Value);
						break;
					}

					//Operation exists
					auto output = Ptr->Get(InsideTokens, Proj);
					int Start = Code.length();
					if (Start == 0)    //Make sure start isn't 0
						Start = 1;
					Code += output.second;

					Output.push_back(Region(t->Value, Start, Code.length()));    //Create a region for the operation
					for (Region r : output.first)
					{
						if (r.Name == "Main")    //If it's a main region, we don't want to add it
						{
							continue;
						}
						r.Add(Start);
						Output.push_back(r);
					}
					CurrentRegion.Start = Code.length();
					break;
				}
				case LexerToken::FROM:
				{
					++i;
					if ((*Tokens)[i]->TokenType != LexerToken::IDENT)    //Check if the next token is the organism's name
					{
						CDB_BuildError("Expected organism name after FROM token");
						break;
					}
					std::string& OrganismName = (*Tokens)[i]->Value;
					std::pair<std::vector<Region>, std::string> output;

					/* 
					"from Unoptomized" exists so that you can take a sequence from an organism without a gilEncoding and have it generate a 
					sequence that *should* work in your target. If you set the origin as Unoptimized, it will pick the best codons for your 
					target. This could cause problems with protein folding, but it'll probably be fine
					*/
					if (OrganismName == "Unoptimized")
					{
						++i;

						std::vector<Token*> InBlockTokens = GetInsideTokens(*Tokens, i);    //Get the DNA to be translated
						output = Compile(Proj, &InBlockTokens);

						std::string Aminos;
						std::vector<int> Idxs;
						DNA2Aminos(Aminos, Idxs, output.second, Proj);    //Convert DNA to amino acids
						AminosToDNA(output.second, Aminos, Proj, CurrentEncoding);    //Convert amino acids to DNA optimized for target
					}
					else
					{
						CodonEncoding OriginEncoding(OrganismName);    //Get the organism's codonencoding
						++i;

						std::vector<Token*> InBlockTokens = GetInsideTokens(*Tokens, i);    //Get the DNA to be translated
						output = Compile(Proj, &InBlockTokens);

						std::string Aminos;
						std::vector<int> Idxs;
						DNA2Aminos(Aminos, Idxs, output.second, Proj, OriginEncoding);    //Convert DNA to amino acids
						AminosIDXToDNA(output.second, Aminos, Idxs, Proj, CurrentEncoding);    //Convert amino acids to DNA optimized for target
					}

					//Check if main region has anything in it
					if (CurrentRegion.Start != Code.length() || CurrentRegion.Name != "Main")
					{
						CurrentRegion.End = Code.length();
						Output.push_back(CurrentRegion);
						CurrentRegion = Region();
						CurrentRegion.Name = "Main";
					}
					//Add the regions to the output
					Output.reserve(Output.size() + output.first.size() + 1);    //Reserve enough space

					//Region saying that the area was translated
					Region TranslatedRegion("Translated protein from organism " + OrganismName, Code.length(), 
						Code.length() + output.second.length());
					Output.push_back(TranslatedRegion);

					for (Region& r : output.first)
					{
						r.Add(Code.length());
						Output.push_back(r);
					}
					Code += output.second;
					CurrentRegion.Start = Code.length();
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

		bool HasRestrictionSites(std::string& DNA, std::string& Codon, Project* Proj);

		void AminosToDNA(std::string& DNA, std::string& aminos, Project* Proj, CodonEncoding& CurrentEncoding)
		{
			DNA.reserve(DNA.length() + (aminos.length() * 3));

			for (char a : aminos)
			{
				if (IsWhiteSpace(a))
					continue;
				auto Codons = CurrentEncoding.GetCodons(std::tolower(a));
				for (std::string* codon : *Codons)
				{
					if (!HasRestrictionSites(DNA, *codon, Proj))
					{
						DNA += *codon;
						goto continueLoop;
					}
				}
				CDB_BuildWarning("Added avoided restriction site");
				DNA += *(*Codons)[0];

			continueLoop:
				continue;
			}
		}

		//Converts amino acids with idx to the codon that matches those indeces
		void AminosIDXToDNA(std::string& DNA, std::string& aminos, std::vector<int>& Idxs, Project* Proj, CodonEncoding& CurrentEncoding)
		{
			DNA.reserve(DNA.length() + (aminos.length() * 3));

			for (int a = 0; a < aminos.length(); ++a)
			{
				DNA += *CurrentEncoding.GetFromLetter(aminos[a], Idxs[a]);
			}
		}

		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj, CodonEncoding& OriginEncoding)
		{
			Aminos.reserve(DNA.size() / 3);
			AminoIdxs.reserve(DNA.size() / 3);

			for (int i = 0; i < DNA.length() - 2; i += 3)    // AttGccGt
			{
				auto LetterAndIDX = OriginEncoding.CodonToLetter(std::move(DNA.substr(i, 3)));
				Aminos += LetterAndIDX.first;
				AminoIdxs.push_back(LetterAndIDX.second);
			}
		}

		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj)
		{
			Aminos.reserve(DNA.size() / 3);
			AminoIdxs.reserve(DNA.size() / 3);

			for (int i = 0; i < DNA.length() - 2; i += 3)    // AttGccGt
			{
				std::string Codon = DNA.substr(i, 3);
				for (char& c : Codon)
				{
					c = std::tolower(c);
				}
				Aminos += CodonEncoding::CodonToLetterOnly(std::move(Codon));
			}
		}

		void ImportFile(std::string& Path, Project* Proj)
		{
			std::filesystem::path path = std::filesystem::path(Path);
			bool imported = false;

			if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
				+ path.stem().string() + ".cgil"))    //if cached/precompiled file exists
			{
				Proj->Namespaces[path.stem().string()] = Project::Load(CDB::Application::Get().PreBuildDir + "\\" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				imported = true;
			}
			else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\" 
				+ path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
			{
				CDB_BuildInfo("Worked");
				Proj->Namespaces[path.stem().string()] = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				imported = true;
			}
			else if (std::filesystem::exists(CDB::Application::Get().OpenProject->Path + "\\" + path.parent_path().string() + "\\" + path.stem().string() + ".gil"))
			{
				std::ifstream t(CDB::Application::Get().OpenProject->Path + "\\" + path.parent_path().string() + "\\" 
					+ path.stem().string() + ".gil");
				std::stringstream ss;
				ss << t.rdbuf();

				std::string GILsrc = ss.str();

				auto Tokens = *GIL::Lexer::Tokenize(GILsrc);
				Project* p = GIL::Parser::Project::Parse(Tokens);
				p->Save(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
					+ path.stem().string() + ".cgil");

				Proj->Namespaces[path.stem().string()] = p;
				imported = true;
			}

			if (Proj->TargetOrganism != "")
			{
				if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //if cached/precompiled file exists
				{
					Proj->Namespaces[path.stem().string()] = Project::Load(CDB::Application::Get().PreBuildDir + "\\"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					imported = true;
				}
				else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
				{
					CDB_BuildInfo("Worked");
					Proj->Namespaces[path.stem().string()] = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					imported = true;
				}
			}

			if (!imported)
			{
				CDB_BuildError("Could not find file \"{0}\"", path.string());
			}
		}

		#ifdef CDB_PLATFORM_WINDOWS
		typedef GILModule* (__stdcall* f_GetModule)();
		#else
		typedef GILModule* (*f_GetModule)();
		#endif
		void LinkDLL(std::string& Path, Project* Proj)
		{
			std::filesystem::path LibPath = CDB::Application::Get().PathToEXE + "\\GILModules\\" + Path + ".dll";
			if (Modules.contains(LibPath.stem().string()))
			{
				return;
			}
			if (std::filesystem::exists(LibPath))
			{
				//Load the DLL
				auto func = CDB::GetSharedLibFunc<f_GetModule>(LibPath.string().c_str(), "GetModule");
				Modules[LibPath.stem().string()] = func();
			}
		}

		bool HasRestrictionSites(std::string& DNA, std::string& Codon, Project* Proj)
		{
			for (std::string& rs : Proj->AvoidRSites)
			{
				std::string Pattern = utils::GetRestrictionSite(rs);

				if (utils::FindNearby(DNA, Codon, Pattern))
					return true;
			}
			return false;
		}
	}
}