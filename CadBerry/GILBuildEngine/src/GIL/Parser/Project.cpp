#include "gilpch.h"
#include "Project.h"
#include "GIL/Compiler/Compiler.h"
#include "Operation.h"
#include "Sequence.h"
#include "GIL/SaveFunctions.h"

#include "GIL/CGIL/CGIL_info.h"
#include "Core.h"

using namespace GIL::Lexer;

namespace GIL
{
	namespace Parser
	{
		//Function definitions 
		void GetReusableElements(std::vector<Token*>& Tokens, Project* Target);

		void FixTokens(std::vector<Token*>& Tokens, std::map<std::string, std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations);


		void GetSequence(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>&Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target);

		void GetOperation(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>&Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target);
		


		bool IsDNA(std::string& s);
		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, int& i);


		Project* Project::Parse(std::vector<Token*>& Tokens)
		{
			Project* CurrentProject = new Project();

			GetReusableElements(Tokens, CurrentProject);

			return CurrentProject;
		}

		void GetReusableElements(std::vector<Token*>& Tokens, Project* Target)
		{
			std::map<std::string, std::vector<Token*>> Sequences;
			std::map<std::string, std::vector<Token*>> Operations;
			Target->Main = std::vector<Token*>();

			for (int i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::DEFINESEQUENCE:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Expected IDENT token after sequence definition");
						break;
					}
					std::string Name = Tokens[i]->Value;
					++i;
					Sequences[Name] = GetInsideTokens(Tokens, i);
					break;
				}
				case LexerToken::DEFOP:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Expected IDENT token after operation definition");
						break;
					}
					std::string Name = Tokens[i]->Value;
					++i;
					Operations[Name] = GetInsideTokens(Tokens, i);
					break;
				}
				case LexerToken::SETTARGET:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Expected target organism name after #Target");
						break;
					}
					Target->TargetOrganism = Tokens[i]->Value;
					break;
				}
				default:
					Target->Main.push_back(Tokens[i]);
					break;
				}
			}

			FixTokens(Tokens, Sequences, Operations);

			std::map<std::string, std::vector<Token*>>::iterator it;
			std::vector<std::string> CreatedSequences;
			std::vector<std::string> CreatedOps;
			CreatedSequences.reserve(Sequences.size());
			CreatedOps.reserve(Operations.size());

			for (it = Sequences.begin(); it != Sequences.end(); ++it)
			{
				GetSequence(it->first, it->second, CreatedSequences, CreatedOps, Sequences, Operations, Target);
			}

			for (it = Operations.begin(); it != Operations.end(); ++it)
			{
				GetOperation(it->first, it->second, CreatedSequences, CreatedOps, Sequences, Operations, Target);
			}

			for (int i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::FORWARD:
					if (!(i > 0 && Tokens[i - 1]->TokenType == LexerToken::IDENT))
					{
						CDB_BuildError("Missing IDENT token before or after forward (=>)");
						break;
					}

					if (Tokens[i + 1]->TokenType == LexerToken::IDENT)
					{
						if (Target->Sequences.find(Tokens[i + 1]->Value) != Target->Sequences.end())
						{
							//If the forward's pointing to a valid sequence set the first part to the second part's pointer
							Target->Sequences[Tokens[i - 1]->Value] = Target->Sequences[Tokens[i + 1]->Value];
							break;
						}
						else if (Target->Operations.find(Tokens[i + 1]->Value) != Target->Operations.end())
						{
							Target->Operations[Tokens[i - 1]->Value] = Target->Operations[Tokens[i + 1]->Value];
							break;
						}

						CDB_BuildError("Could not find sequence or operation {0}", Tokens[i + 1]->Value);
						break;
					}
				default:
					break;
				}
			}
		}

		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, int& i)
		{
			int TokensStart = i;
			int NumTokens = 0;
			int Depth = 0;

			for (i; i < Tokens.size(); ++i)
			{
				++NumTokens;
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::BEGIN:
					++Depth;
					break;
				case LexerToken::END:
					--Depth;
					break;
				default:
					break;
				}
				if (Depth == 0)
					break;
			}

			std::vector<Token*> OutputTokens;
			OutputTokens.reserve(NumTokens);

			int LastToken = TokensStart + NumTokens;
			for (TokensStart; TokensStart < LastToken; ++TokensStart)
			{
				OutputTokens.push_back(Tokens[TokensStart]);
			}
			return OutputTokens;
		}

		void GetSequence(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target)
		{
			if (std::find(CompletedSequences.begin(), CompletedSequences.end(), Name) != CompletedSequences.end())
			{
				return;
			}

			CompletedSequences.push_back(Name);
			Target->Sequences[Name] = nullptr;

			//We want to compile sequence dependencies first
			for (Token* t : Tokens)
			{
				switch (t->TokenType)
				{
				case LexerToken::IDENT:
					GetSequence(t->Value, Sequences[t->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;
				case LexerToken::CALLOP:
					GetOperation(t->Value, Operations[t->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;
				default:
					break;
				}
			}

			Target->Sequences[Name] = new StaticSequence(Tokens);
		}

		void GetOperation(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target)
		{
			if (std::find(CompletedOps.begin(), CompletedOps.end(), Name) != CompletedOps.end())
			{
				return;
			}

			CompletedOps.push_back(Name);
			DynamicOperation* NewOp = new DynamicOperation();
			Target->Operations[Name] = NewOp;
			NewOp->tokens.reserve(Tokens.size());

			for (Token* t : Tokens)
			{
				NewOp->tokens.push_back(t);
				switch (t->TokenType)
				{
				case LexerToken::IDENT:
					GetSequence(t->Value, Sequences[t->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;
				case LexerToken::CALLOP:
					GetOperation(t->Value, Operations[t->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;
				default:
					break;
				}
			}
		}


		//This function changes all the idents that are actually DNA to DNA
		void FixTokens(std::vector<Token*>& Tokens, std::map<std::string, std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations)
		{
			for (Token* t : Tokens)
			{
				if (t->TokenType == LexerToken::IDENT)
				{
					if (IsDNA(t->Value))
					{
						if (Sequences.find(t->Value) == Sequences.end()
							&& Operations.find(t->Value) == Operations.end())
						{
							t->TokenType = LexerToken::DNA;
						}
					}
				}
			}
		}

		bool IsDNA(std::string& s)
		{
			for (char c : s)
			{
				switch (c)
				{
				case 'a':
				case 'c':
				case 't':
				case 'g':
				case 'A':
				case 'C':
				case 'T':
				case 'G':
					break;
				default:
					return false;
				}
			}
			return true;
		}

		void Project::Save(std::string Path)
		{
			std::ofstream OutputFile(Path, std::ios::binary);
			//Write the CGIL file version to the file
			int version = CGILVersion;
			OutputFile.write((char*)&version, sizeof(int));

			//The GIL compiler version that made this file
			version = GIL_MAJOR;
			OutputFile.write((char*)&version, sizeof(int));
			version = GIL_MINOR;
			OutputFile.write((char*)&version, sizeof(int));
			version = GIL_FIX;
			OutputFile.write((char*)&version, sizeof(int));

			//Write the target organism to the file
			SaveString(this->TargetOrganism, OutputFile);

			int Len = this->Operations.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto op : this->Operations)
			{
				std::string Name = op.first;
				SaveString(Name, OutputFile);
				op.second->Save(OutputFile);
			}

			Len = this->Sequences.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto sequence : this->Sequences)
			{
				std::string Name = sequence.first;
				SaveString(Name, OutputFile);
				sequence.second->Save(OutputFile);
			}

			Len = this->Main.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (GIL::Lexer::Token* t : this->Main)
			{
				t->Save(OutputFile);
			}
		}

		//CGIL function definitions:
		Project* LoadCGIL0(std::ifstream& InputFile);

		Project* Project::Load(std::string Path)
		{
			std::ifstream InputFile(Path);
			int Version;
			InputFile.read((char*)&Version, sizeof(int));
			
			switch (Version)
			{
			case 0:
				return LoadCGIL0(InputFile);
			default:
				CDB_BuildFatal("Error reading CGIL version from path {0}", Path);
			}
		}

		Project* LoadCGIL0(std::ifstream& InputFile)
		{
			int IntVal;
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);

			int Len = -1;
			InputFile.read((char*)&Len, sizeof(int));
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Operations[Name] = new DynamicOperation();
				Proj->Operations[Name]->Load(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = new StaticSequence();
				Proj->Sequences[Name]->Load(InputFile);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));
			Proj->Main.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				Proj->Main.push_back(Token::Load(InputFile));
			}
			return Proj;
		}
	}
}