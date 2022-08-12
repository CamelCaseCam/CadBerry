#include <gilpch.h>
#include "CompilerFunctions.h"
#include "GIL/Lexer/LexerMacros.h"

#include "CadBerry/Application.h"
#include "CadBerry/SharedLib.h"

#include "GIL/Utils/Utils.h"
#include "Compiler.h"


namespace GIL
{
	
	using namespace Parser;
	using namespace Lexer;
	namespace Compiler
	{
		bool Prepro_IsEq(Project* Proj, std::vector<Token*>* Tokens);

		std::map<std::string, GILModule*> Modules;
		std::map<std::string, std::function<bool(Project*, std::vector<Token*>*)>> PreproConditions = {
			{"IsEq", Prepro_IsEq},
		};

		

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

		//Pointers in returned vector point to the same memory as the original vector and should not be deleted
		std::vector<Token*> GetTokensInBetween(std::vector<Token*>& Tokens, size_t& i, LexerToken Begin, LexerToken End)
		{
			int start = i;
			for (i; i < Tokens.size(); ++i)
			{
				if (Tokens[i]->TokenType == Begin)
					break;
				if (Tokens[i]->TokenType == LexerToken::COMMENT || Tokens[i]->TokenType == LexerToken::NEWLINE)
					continue;
				i = start;
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

		std::vector<Lexer::Token*> GetTokensInBetween(std::vector<Lexer::Token*>& Tokens, int& i, Lexer::LexerToken Begin, Lexer::LexerToken End)
		{
			size_t temp = i;
			return GetTokensInBetween(Tokens, temp, Begin, End);
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
					Output[ParamIdx2Name[ParamIdx]] = Param(Proj->Sequences[InsideTokens[n]->Value], Proj, Proj->Sequences[InsideTokens[n]->Value]->SeqType);
					++ParamIdx;
				}
				else if (InsideTokens[n]->TokenType == LexerToken::PARAM)
				{
					//Check to make sure the parameter isn't $InnerCode
					if (InsideTokens[n]->Value == "InnerCode")
					{
						CDB_BuildError("GIL forbids passing InnerCode as a parameter explicitly");
						continue;
					}

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

					Output[InsideTokens[n]->Value] = Param(Proj->Sequences[InsideTokens[n + 2]->Value], Proj, Proj->Sequences[InsideTokens[n + 2]->Value]->SeqType);
					n += 2;
				}
			}
			return Output;
		}
	}
}