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
	int CurrentSequenceCallDepth = 0;

	namespace Compiler
	{
		using namespace GIL::Lexer;
		using namespace GIL::Parser;

		std::map<std::string, GILModule*> Modules;

		bool Prepro_IsEq(Project* Proj, std::vector<Token*>* Tokens);
		std::map<std::string, std::function<bool(Project*, std::vector<Token*>*)>> PreproConditions = {
			{"IsEq", Prepro_IsEq},
		};

		//Function definitions
		void AminosToDNA(std::string& DNA, std::string& aminos, Project* Proj, CodonEncoding& CurrentEncoding);
		void AminosIDXToDNA(std::string& DNA, std::string& aminos, std::vector<int>& Idxs, Project* Proj, CodonEncoding& CurrentEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj, CodonEncoding& OriginEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Project* Proj);
		void ImportFile(std::string& Path, Project* Proj);
		void LinkDLL(std::string& Path, Project* Proj);

		inline void AddRegionToVector(const Region& reg, std::vector<Region>& vec, int& last)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
				--last;
			}
		}

		inline void AddRegionToVector(const Region&& reg, std::vector<Region>& vec, int& last)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
				--last;
			}
		}

		inline void AddRegionToVector(const Region& reg, std::vector<Region>& vec)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
			}
		}

		inline void AddRegionToVector(const Region&& reg, std::vector<Region>& vec)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
			}
		}

		//Imports recursively
		inline void ImportAllProjectImports(Project* Proj)
		{
			for (std::string& s : Proj->Imports)
			{
				if (!Proj->Namespaces.contains(s))
				{
					ImportFile(s, Proj);
				}
			}
		}

		//Links recursively
		inline void LinkAllProjectDLLs(Project* Proj)
		{
			for (std::string& s : Proj->Usings)
			{
				LinkDLL(s, Proj);
			}
		}

		std::pair<std::vector<Region>, std::string> Compile(Project* Proj, std::vector<Token*>* Tokens)
		{
			if (Tokens == nullptr)
			{
				Tokens = &Proj->Main;
			}
			std::vector<Region> Output;
			CodonEncoding CurrentEncoding(Proj->TargetOrganism);

			//Import any imports
			ImportAllProjectImports(Proj);

			LinkAllProjectDLLs(Proj);

			std::vector<Region> OpenRegions = { Region(), };
			int LastRegion = 0;

			std::string Code;
			OpenRegions[LastRegion].Name = "Main";
			OpenRegions[LastRegion].Start = 1;
			bool PreproIfValue = false;

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
					if ((*Tokens)[i]->TokenType == LexerToken::IDENT || (*Tokens)[i]->TokenType == LexerToken::STRING)
					{
						//If the open region is the "Main" region
						if (OpenRegions[LastRegion].Name == "Main")
						{
							OpenRegions[LastRegion].End = Code.length();
							AddRegionToVector(OpenRegions[LastRegion], Output);
							OpenRegions[LastRegion] = Region();
						}
						else
						{
							OpenRegions.push_back(Region());
							++LastRegion;
						}

						OpenRegions[LastRegion].Name = (*Tokens)[i]->Value;
						OpenRegions[LastRegion].Start = Code.length();
						break;
					}
					CDB_BuildError("#BeginRegion not followed by ident or string");
					break;
				case LexerToken::ENDREGION:

					if (i + 1 < (*Tokens).size() && (*Tokens)[i + 1]->TokenType == LexerToken::IDENT ||
						(*Tokens)[i + 1]->TokenType == LexerToken::STRING)
					{
						//Check backwards until you reach a region with that name
						for (int r = LastRegion; r > -1; --r)
						{
							if (OpenRegions[r].Name == (*Tokens)[i + 1]->Value)
							{
								//We want to for sure add a region defined by the user, even if it's empty
								OpenRegions[r].End = Code.length();
								Output.push_back(std::move(OpenRegions[r]));
								OpenRegions.erase(OpenRegions.begin() + r);
								--LastRegion;
								goto exit;
							}
						}

						CDB_BuildError("Region {0} is not open", (*Tokens)[i + 1]->Value);

					exit:
						++i;
						break;
					}
					OpenRegions[LastRegion].End = Code.length();
					AddRegionToVector(OpenRegions[LastRegion], Output);
					OpenRegions[LastRegion] = Region();
					OpenRegions[LastRegion].Name = "Main";
					OpenRegions[LastRegion].Start = Code.length();
					break;
				case LexerToken::INC:
					if (i >= Tokens->size() - 1 || ((*Tokens)[i + 1]->TokenType != LexerToken::IDENT && 
						(*Tokens)[i + 1]->TokenType != LexerToken::STRING))
					{
						CDB_BuildError("#Inc expeceted name of variable to increment");
						break;
					}
					++i;

					//Check that the variable exists
					if (!Proj->NumVars.contains((*Tokens)[i]->Value))
					{
						CDB_BuildError("Variable \"{0}\" referenced by #Inc does not exist or is not numerical", (*Tokens)[i]->Value);
						break;
					}

					//Increment the variable
					++Proj->NumVars[(*Tokens)[i]->Value];
					break;
				case LexerToken::DEC:
					if (i >= Tokens->size() - 1 || ((*Tokens)[i + 1]->TokenType != LexerToken::IDENT &&
						(*Tokens)[i + 1]->TokenType != LexerToken::STRING))
					{
						CDB_BuildError("#Dec expeceted name of variable to increment");
						break;
					}
					++i;

					//Check that the variable exists
					if (!Proj->NumVars.contains((*Tokens)[i]->Value))
					{
						CDB_BuildError("Variable \"{0}\" referenced by #Dec does not exist or is not numerical", (*Tokens)[i]->Value);
						break;
					}

					//Increment the variable
					--Proj->NumVars[(*Tokens)[i]->Value];
					break;
				case LexerToken::PREPRO_IF:
				{
					if (i >= Tokens->size() - 1)
					{
						CDB_BuildError("#If expected a condition, encountered end of file");
						break;
					}
					++i;
					if ((*Tokens)[i]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("#If expected a name of condition to invoke");
						break;
					}
					if (!PreproConditions.contains((*Tokens)[i]->Value))
					{
						CDB_BuildError("The preprocessor condition \"{0}\" does not exist", (*Tokens)[i]->Value);
					}

					std::string& ConditionName = (*Tokens)[i]->Value;
					++i;
					std::vector<Token*> Params = GetTokensInBetween(*Tokens, i, LexerToken::LPAREN, LexerToken::RPAREN);
					PreproIfValue = PreproConditions[ConditionName](Proj, &Params);

					//If the condition was true
					if (PreproIfValue)
					{
						int NumInBetween = 0;
						for (i; i < (*Tokens).size(); ++i)
						{
							if ((*Tokens)[i]->TokenType == LexerToken::PREPRO_ELSE || (*Tokens)[i]->TokenType == LexerToken::PREPRO_ENDIF)
								break;
							++NumInBetween;
						}

						std::vector<Token*> ToBeCompiled;
						ToBeCompiled.reserve(NumInBetween);
						for (NumInBetween; NumInBetween != -1; --NumInBetween)
							ToBeCompiled.push_back((*Tokens)[i + 1 - NumInBetween]);

						Compile(Proj, &ToBeCompiled);

						//Skip the else statement if there was one
						GetTokensInBetween(*Tokens, i, (*Tokens)[i]->TokenType, LexerToken::PREPRO_ENDIF);
					}
					else    //Handle the else statement if there was one
					{
						//Skip to the else token
						for (i; i < (*Tokens).size(); ++i)
						{
							//Skip evaluating this if you reach endif
							if ((*Tokens)[i]->TokenType == LexerToken::PREPRO_ENDIF)
								break;

							if ((*Tokens)[i]->TokenType == LexerToken::PREPRO_ELSE)
							{
								int NumInBetween = 0;
								for (i; i < (*Tokens).size(); ++i)
								{
									if ((*Tokens)[i]->TokenType == LexerToken::PREPRO_ENDIF)
										break;
									++NumInBetween;
								}

								std::vector<Token*> ToBeCompiled;
								ToBeCompiled.reserve(NumInBetween);
								for (NumInBetween; NumInBetween != -1; --NumInBetween)
									ToBeCompiled.push_back((*Tokens)[i - NumInBetween]);

								Compile(Proj, &ToBeCompiled);
							}
						}
					}
					break;
				}
				case LexerToken::IDENT:
				{
					auto Seq = Proj->GetSeq(Tokens, i, &Modules);
					if (Seq.first == nullptr)    //Make sure sequence exists
					{
						CDB_BuildError("Sequence \"{0}\" does not exist", t->Value);
						break;
					}

					//If Seq's project is nullptr, it came from a module
					if (Seq.second == nullptr)
						Seq.second = Proj;
					Seq.second->TargetOrganism = Proj->TargetOrganism;

					//Make sure the other project has the same compilation target as the current project
					Seq.second->TargetOrganism = Proj->TargetOrganism;

					//Sequence exists - get parameters
					++i;
					auto params = GetParams(Proj, *Tokens, i, Seq.first->ParamIdx2Name);
					int Start = Code.length();

					//Call sequence
					int CurrentSequenceCallDepth = 0;
					auto SequenceOutput = Seq.first->Get(Seq.second, params);

					//Add output to compiler's output
					Code += SequenceOutput.second;
					int UselessInt;
					for (Region& r : SequenceOutput.first)
					{
						if (r.Start == 0)
						{
							r.Start = 1;
						}
						r.Add(Start);
						AddRegionToVector(std::move(r), Output, UselessInt);
					}
					break;
				}
				case LexerToken::CALLOP:
				{
					auto Op = Proj->GetSeq(Tokens, i, &Modules);
					if (Op.first == nullptr)    //Make sure sequence exists
					{
						CDB_BuildError("Operation \"{0}\" does not exist", t->Value);
						break;
					}
					
					//If Op's project is nullptr, it came from a module
					if (Op.second == nullptr)
						Op.second = Proj;
					Op.second->TargetOrganism = Proj->TargetOrganism;

					//Sequence exists - get parameters
					++i;
					auto params = GetParams(Proj, *Tokens, i, Op.first->ParamIdx2Name);
					int Start = Code.length();

					if ((*Tokens)[i]->TokenType == LexerToken::RPAREN)
						++i;
					std::vector<Token*> InsideTokens = GetInsideTokens(*Tokens, i);
					InnerCode Inner = InnerCode(Compile(Proj, &InsideTokens));
					params["InnerCode"] = &Inner;

					//Operation exists
					auto OperationOutput = Op.first->Get(Op.second, params);
					
					//Add output to compiler's output
					Code += OperationOutput.second;
					int UselessInt;
					for (Region& r : OperationOutput.first)
					{
						if (r.Start == 0)
						{
							r.Start = 1;
						}
						r.Add(Start);
						AddRegionToVector(std::move(r), Output, UselessInt);
					}
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

					//Add the regions to the output
					Output.reserve(Output.size() + output.first.size() + 1);    //Reserve enough space

					//Region saying that the area was translated
					Region TranslatedRegion("Translated protein from organism " + OrganismName, Code.length(), 
						Code.length() + output.second.length());
					AddRegionToVector(TranslatedRegion, Output);

					for (Region& r : output.first)
					{
						if (r.Name == "")
							continue;
						r.Add(Code.length());
						AddRegionToVector(r, Output);
					}
					Code += output.second;
					break;
				}
				case LexerToken::FOR:
				{
					++i;
					if ((*Tokens)[i]->TokenType != LexerToken::IDENT && (*Tokens)[i]->TokenType != LexerToken::STRING)    //Check if the next token is the organism's name
					{
						CDB_BuildError("Expected organism name after FOR token");
						break;
					}
					std::string& OrganismName = (*Tokens)[i]->Value;
					std::pair<std::vector<Region>, std::string> output;

					
					std::string OldTargetOrganism = Proj->TargetOrganism;
					Proj->TargetOrganism = OrganismName;
					++i;

					std::vector<Token*> InBlockTokens = GetInsideTokens(*Tokens, i);    //Get the DNA to be translated
					output = Compile(Proj, &InBlockTokens);
					
					Proj->TargetOrganism = OldTargetOrganism;

					//Add the regions to the output
					Output.reserve(Output.size() + output.first.size() + 1);    //Reserve enough space

					//Region saying that the area was translated
					Region TranslatedRegion("DNA optimized for organism " + OrganismName, Code.length(),
						Code.length() + output.second.length());
					AddRegionToVector(TranslatedRegion, Output);

					for (Region& r : output.first)
					{
						if (r.Name == "")
							continue;
						r.Add(Code.length());
						AddRegionToVector(r, Output);
					}
					Code += output.second;
					break;
				}
				default:
					break;
				}
			}

			//Add any unclosed regions
			for (int r = LastRegion; r > -1; --r)
			{
				OpenRegions[r].End = Code.length();
				AddRegionToVector(std::move(OpenRegions[r]), Output);
			}

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
				Project* p = Project::Load(CDB::Application::Get().PreBuildDir + "\\" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				Proj->Namespaces[path.stem().string()] = p;
				ImportAllProjectImports(p);
				LinkAllProjectDLLs(p);
				imported = true;
			}
			else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\" 
				+ path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
			{
				Project* p = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				Proj->Namespaces[path.stem().string()] = p;
				ImportAllProjectImports(p);
				LinkAllProjectDLLs(p);
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
				ImportAllProjectImports(p);
				LinkAllProjectDLLs(p);
				imported = true;
			}

			if (Proj->TargetOrganism != "")
			{
				if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //if cached/precompiled file exists
				{
					Project* p = Project::Load(CDB::Application::Get().PreBuildDir + "\\"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					Proj->Namespaces[path.stem().string()] = p;
					ImportAllProjectImports(p);
					LinkAllProjectDLLs(p);
					imported = true;
				}
				else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
				{
					Project* p = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					Proj->Namespaces[path.stem().string()] = p;
					ImportAllProjectImports(p);
					LinkAllProjectDLLs(p);
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
				auto func = CDB::GetSharedLibFunc<f_GetModule>(LibPath.c_str(), "GetModule");
				Modules[LibPath.stem().string()] = func();
			}
		}

		std::vector<Token*> GetTokensInBetween(std::vector<Token*>& Tokens, int& i, LexerToken Begin, LexerToken End)
		{
			for (i; i < Tokens.size(); ++i)
			{
				if (Tokens[i]->TokenType == Begin)
					break;
				if (Tokens[i]->TokenType == LexerToken::COMMENT || Tokens[i]->TokenType == LexerToken::NEWLINE)
					continue;
				--i;
				return std::vector<Token*>();
			}
			++i;
			int NumInBetween = 0;
			for (i; i < Tokens.size(); ++i)
			{
				if (Tokens[i]->TokenType == End)
					break;
				++NumInBetween;
			}

			//Now put it into the output vector
			std::vector<Token*> Output;
			Output.reserve(NumInBetween);
			for (NumInBetween; NumInBetween > 0; --NumInBetween)
				Output.push_back(Tokens[i - NumInBetween]);

			return Output;
		}

		std::map<std::string, Param> GetParams(Parser::Project* Proj, std::vector<Token*>& Tokens, int& i, 
			std::vector<std::string>& ParamIdx2Name, std::map<std::string, Param>* Params)
		{
			std::vector<Token*> InsideTokens = GetTokensInBetween(Tokens, i, LexerToken::LPAREN, LexerToken::RPAREN);
			std::map<std::string, Param> Output;

			int ParamIdx = 0;
			for (int n = 0; n < InsideTokens.size(); ++n)
			{
				if (InsideTokens[n]->TokenType == LexerToken::IDENT)
				{
					//Check to make sure sequence exists
					if (!Proj->Sequences.contains(InsideTokens[n]->Value))
					{
						CDB_BuildError("Sequence \"{0}\" passed as parameter does not exist", InsideTokens[n]->Value);
						continue;
					}
					//Put the sequence into the output
					Output[ParamIdx2Name[ParamIdx]] = Param(Proj->Sequences[InsideTokens[n]->Value], Proj->Sequences[InsideTokens[n]->Value]->SeqType);
					++ParamIdx;
				}
				else if (InsideTokens[n]->TokenType == LexerToken::PARAM)
				{
					//Check to make sure there are enough tokens
					if (n >= InsideTokens.size() - 2)
					{
						//See if this was one of the calling sequence's parameters
						if (Params != nullptr && Params->contains(InsideTokens[n]->Value))
						{
							Output[ParamIdx2Name[ParamIdx]] = (*Params)[InsideTokens[n]->Value];
							++ParamIdx;
							continue;
						}

						CDB_BuildError("Parameter \"{0}\" was assigned by name, no value given (reached end of parameters)", InsideTokens[n]->Value);
						return Output;
					}

					if (InsideTokens[n + 1]->TokenType != LexerToken::EQUALS)
					{
						CDB_BuildError("Parameter\"{0}\" was referenced by name but not assigned (assignment requires an equals sign)",
							InsideTokens[n]->Value);
						return Output;
					}

					if (InsideTokens[n + 2]->TokenType != LexerToken::IDENT)
					{
						//If it's a parameter, pass the parameter
						if (Params != nullptr && InsideTokens[n + 2]->TokenType == LexerToken::PARAM
							&& Params->contains(InsideTokens[n + 2]->Value))
						{
							Output[InsideTokens[n]->Value] = (*Params)[InsideTokens[n + 2]->Value];
							n += 2;
							continue;
						}

						CDB_BuildError("Parameter\"{0}\" was assigned to a token not of type IDENT", InsideTokens[n]->Value);
						return Output;
					}

					//Make sure the sequence exists
					if (!Proj->Sequences.contains(InsideTokens[n + 2]->Value))
					{
						CDB_BuildError("Sequence \"{0}\" passed as parameter does not exist", InsideTokens[n + 2]->Value);
						continue;
					}

					Output[InsideTokens[n]->Value] = Param(Proj->Sequences[InsideTokens[n + 2]->Value], Proj->Sequences[InsideTokens[n + 2]->Value]->SeqType);
					n += 2;
				}
			}
			return Output;
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




		//Prepro conditions
		bool Prepro_IsEq(Project* Proj, std::vector<Token*>* Tokens)
		{
			if (Tokens->size() != 2)
			{
				CDB_BuildError("#If IsEq expected two operands");
				for (Token* t : *Tokens)
				{
					CDB_BuildInfo(*t);
				}
				return false;
			}

			if (Proj->StrVars.contains((*Tokens)[0]->Value) && Proj->StrVars.contains((*Tokens)[1]->Value))
			{
				return Proj->StrVars[(*Tokens)[0]->Value] == Proj->StrVars[(*Tokens)[1]->Value];
			}
			else if (Proj->NumVars.contains((*Tokens)[0]->Value) && Proj->NumVars.contains((*Tokens)[1]->Value))
			{
				return Proj->NumVars[(*Tokens)[0]->Value] == Proj->NumVars[(*Tokens)[1]->Value];
			}
			else
			{
				CDB_BuildError("Variables \"{0}\" and \"{1}\" either are not the same type or do not exist", (*Tokens)[0]->Value, 
					(*Tokens)[1]->Value);
				return false;
			}
		}
	}
}