#include "gilpch.h"
#include "Project.h"
#include "GIL/Compiler/Compiler.h"
#include "Operation.h"
#include "Sequence.h"
#include "GIL/SaveFunctions.h"

#include "GIL/Bools/BoolContext.h"
#include "GIL/Bools/IfStatement.h"

#include "GIL/Errors.h"

#include "GIL/CGIL/CGIL_info.h"
#include "Core.h"

using namespace GIL::Lexer;

namespace GIL
{
	namespace Parser
	{
		//Function definitions 
		void GetReusableElements(std::vector<Token*>& Tokens, Project* Target);    //Does most of the parsing

		//Converts IDENT tokens to DNA where needed
		void FixTokens(std::vector<Token*>& Tokens, std::map<std::string, std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations);


		void GetSequence(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>&Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target);

		void GetOperation(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>&Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target);

		void CreateBool(std::string& BoolName, std::vector<Token*>& Tokens, int& i);
		

		//Checks if IDENT could be DNA
		bool IsDNA(std::string& s);
		//Gets tokens in between { and }
		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, int& i);


		Project* Project::Parse(std::vector<Token*>& Tokens)
		{
			Project* CurrentProject = new Project();

			GetReusableElements(Tokens, CurrentProject);

			return CurrentProject;
		}

		Project::~Project()
		{
			for (auto s : Sequences)
			{
				delete s.second;
			}

			for (auto o : Operations)
			{
				delete o.second;
			}

			for (Token* t : Main)
			{
				Token::SafeDelete(t);
			}

			for (auto p : Namespaces)
			{
				delete p.second;
			}
		}

		void GetReusableElements(std::vector<Token*>& Tokens, Project* Target)    //Does all the actual parsing
		{
			std::map<std::string, std::vector<Token*>> Sequences;
			std::map<std::string, std::vector<Token*>> Operations;
			Target->Main = std::vector<Token*>();    //The file entry point

			for (int i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::DEFINESEQUENCE:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT)    //Get name of sequence
					{
						CDB_BuildError("Expected IDENT token after sequence definition");
						break;
					}
					std::string Name = Tokens[i]->Value;
					++i;
					Sequences[Name] = GetInsideTokens(Tokens, i);    //Adds sequence to be compiled after
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
				case LexerToken::IMPORT:    //Adds file name to imports, but the compiler imports the files
					++i;
					if (Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected file name after import token, found token of type {0}", LexerToken2Str[Tokens[i]->TokenType]);
						break;
					}
					Target->Imports.push_back(Tokens[i]->Value);
					break;
				case LexerToken::USING:
					++i;
					if (Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected file name after using token, found token of type {0}", LexerToken2Str[Tokens[i]->TokenType]);
						break;
					}
					Target->Usings.push_back(Tokens[i]->Value);
					break;
				case LexerToken::CREATENAMESPACE:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Expected namespace name");
						break;
					}
					std::string Name = Tokens[i]->Value;
					++i;
					//TODO: allow the user to define things inside namespaces that use stuff outside the namespaces
					std::vector<Token*> InsideTokens = GetInsideTokens(Tokens, i);
					Target->Namespaces[Name] = Project::Parse(InsideTokens);    //Parse the tokens inside the namespace into a seperate Project
					break;
				}
				default:
					Target->Main.push_back(Tokens[i]);
					break;
				}
			}

			FixTokens(Tokens, Sequences, Operations);    //Converts IDENT tokens to DNA where needed

			std::map<std::string, std::vector<Token*>>::iterator it;

			//Vectors so that we don't compile stuff multiple times or recursively
			std::vector<std::string> CreatedSequences;
			std::vector<std::string> CreatedOps;
			CreatedSequences.reserve(Sequences.size());
			CreatedOps.reserve(Operations.size());

			for (it = Sequences.begin(); it != Sequences.end(); ++it)    //Loop through sequences and recursively compile their dependencies
			{
				GetSequence(it->first, it->second, CreatedSequences, CreatedOps, Sequences, Operations, Target);
			}

			for (it = Operations.begin(); it != Operations.end(); ++it)    //Loop through operations and recursively compile their dependencies
			{
				GetOperation(it->first, it->second, CreatedSequences, CreatedOps, Sequences, Operations, Target);
			}

			for (int i = 0; i < Tokens.size(); ++i)    //Do forwards
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
					}
					break;
				case LexerToken::BOOL:
				{
					if (!(i + 3 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::IDENT))
					{
						CDB_BuildError("Missing IDENT token after bool declaration");
						break;
					}

					++i;
					std::string& BoolName = Tokens[i]->Value;
					++i;
					if (Tokens[i]->TokenType != LexerToken::EQUALS)
					{
						FatalError("Unbound boolean error: bool \"{0}\" has no value", BoolName);
						return;
					}
					++i;

					CreateBool(BoolName, Tokens, i);
					break;
				}
				case LexerToken::IF:
				{
					IfStatement* CurrentStatement = new IfStatement();
					switch (Tokens[i + 1]->TokenType)
					{
					case LexerToken::IDENT:    // if BoolName
					{
						CurrentStatement->BoolName = Tokens[i + 1]->Value;
						BoolContext::IfStatements.push_back(CurrentStatement);
						break;
					}
					case LexerToken::LPAREN:    // if (Some expression)
					{
						BoolContext::Value* IntBool = BoolContext::AllocIntermediateBool(nullptr);
						i += 2;
						CreateBool(IntBool->BoolName, Tokens, i);

						if (BoolContext::Bools[IntBool->BoolName] == nullptr)
						{
							CheckFatal;
							CDB_BuildError("Failed to parse bool from parentheses");
							delete CurrentStatement;
							continue;
						}
						BoolContext::IfStatements.push_back(CurrentStatement);
						break;
					}
					case LexerToken::BOOL_TRUE:    // if true
					{
						CurrentStatement->BoolName = "$True";
						BoolContext::IfStatements.push_back(CurrentStatement);
						break;
					}
					case LexerToken::BOOL_FALSE:    //if false
					{
						CurrentStatement->BoolName = "$False";
						BoolContext::IfStatements.push_back(CurrentStatement);
						break;
					}
					default:
						FatalError("Invalid token {0} after IF token", *Tokens[i + 1]);
						continue;
					}
					break;

					/*
					At this point, we've got the condition that the if statment is asking about (CurrentStatement->BoolName). Now, we need
					to collect all the genes in the if statement's body. If the if statement only contains sequences, each sequence will be 
					considered a seperate gene. However, if any operations, DNA, or amino sequences are found in the body, the entire body 
					will be treated as a single gene. 
					*/

					//Get body
					std::vector<Token*> Tokens = GetInsideTokens(Tokens, i);
				}
				default:
					break;
				}
			}
		}


		/*
		Converts "bool b = b1 & b2 & !b3 | b4"
		To:
		$Auto0 = b1 & b2
		$Auto1 = !b3
		$Auto2 = $Auto0 & $Auto1
		b = $Auto2 | b4
		*/
		void CreateBool(std::string& BoolName, std::vector<Token*>& Tokens, int& i)
		{
			BoolContext::Value* LValue;
			BinOpType OpType;
			BoolContext::Value* RValue;

			if (Tokens[i]->TokenType == LexerToken::BOOL_TRUE)
			{
				LValue = new BoolContext::Value("$True");
			}
			else if (Tokens[i]->TokenType == LexerToken::BOOL_FALSE)
			{
				LValue = new BoolContext::Value("$False");
			}
			else if (Tokens[i]->TokenType == LexerToken::IDENT)
			{
				LValue = new BoolContext::Value(Tokens[i]->Value);
			}
			else if (Tokens[i]->TokenType == LexerToken::NOT && Tokens[i + 1]->TokenType == LexerToken::IDENT)
			{
				LValue = new BoolContext::Value(Tokens[i + 1]->Value);
				BoolContext::BinOp* NotOp = new BoolContext::BinOp(LValue, BinOpType::NOT, nullptr);
				LValue = BoolContext::AllocIntermediateBool(NotOp);
			}
			else if (Tokens[i]->TokenType == LexerToken::LPAREN)
			{
				LValue = BoolContext::AllocIntermediateBool(nullptr);
				++i;
				CreateBool(LValue->BoolName, Tokens, i);

				if (BoolContext::Bools[LValue->BoolName] == nullptr)
				{
					CheckFatal;
					CDB_BuildError("Failed to parse bool expression in parentheses");
					return;
				}
			}
			else
			{
				CDB_BuildError("Expected IDENT after bool \"{0}\", found {1}", BoolName, LexerToken2Str[Tokens[i]->TokenType]);
				return;
			}
			++i;

			for (i; i + 2 < Tokens.size() && Tokens[i]->TokenType != LexerToken::NEWLINE && Tokens[i]->TokenType != LexerToken::RPAREN; i += 2)
			{
				switch (Tokens[i]->TokenType)    //Get BinOpType
				{
				case LexerToken::AND:
					OpType = BinOpType::AND;
					break;
				case LexerToken::OR:
					OpType = BinOpType::OR;
					break;
				case LexerToken::NAND:
					OpType = BinOpType::NAND;
					break;
				case LexerToken::NOR:
					OpType = BinOpType::NOR;
					break;
				default:
					CDB_BuildError("Unexpected token type {0} in boolean \"{1}\" declaration", LexerToken2Str[Tokens[i]->TokenType], BoolName);
					return;
				}

				switch (Tokens[i + 1]->TokenType)
				{
				case LexerToken::NOT:
				{
					if (Tokens[i + 2]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Expected IDENT token after NOT operator");
						return;
					}
					BoolContext::BinOp* Op = new BoolContext::BinOp(new BoolContext::Value(Tokens[i + 2]->Value), BinOpType::NOT, nullptr);
					RValue = BoolContext::AllocIntermediateBool(Op);
					++i;
					break;
				}
				case LexerToken::LPAREN:
				{
					RValue = BoolContext::AllocIntermediateBool(nullptr);
					i += 2;

					CreateBool(RValue->BoolName, Tokens, i);
					if (BoolContext::Bools[RValue->BoolName] == nullptr)
					{
						CheckFatal;
						CDB_BuildError("Failed to parse bool expression in parentheses");
						return;
					}
					break;
				}
				case LexerToken::IDENT:
					RValue = new BoolContext::Value(Tokens[i + 1]->Value);
					break;
				case LexerToken::BOOL_TRUE:
					RValue = new BoolContext::Value("$True");
					break;
				case LexerToken::BOOL_FALSE:
					RValue = new BoolContext::Value("$False");
					break;
				default:
					CDB_BuildError("Expected NOT or IDENT token after binary operation in declaration of bool \"{0}\" (found {1})", BoolName, LexerToken2Str[Tokens[i]->TokenType]);
					return;
				}

				if (i + 2 < Tokens.size())
				{
					switch (Tokens[i + 2]->TokenType)
					{
					case LexerToken::AND:
					case LexerToken::OR:
					case LexerToken::NAND:
					case LexerToken::NOR:
						//We need to allocate an intermediate bool
						LValue = BoolContext::AllocIntermediateBool(new BoolContext::BinOp(LValue, OpType, RValue));
						break;
					default:
						break;
					}
				}
			}

			BoolContext::Bools[BoolName] = new BoolContext::BinOp(LValue, OpType, RValue);
		}

		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, int& i)    //Gets all tokens enclosed by a pair of curly brackets
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
				case LexerToken::NEWLINE:
				case LexerToken::COMMENT:
					continue;
				default:
					break;
				}
				if (Depth == 0)
					break;
			}

			std::vector<Token*> OutputTokens;
			OutputTokens.reserve(NumTokens);    //We find out how many tokens there are so we only have to resize the vector once

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
			//If this sequence has already been compiled, return
			if (std::find(CompletedSequences.begin(), CompletedSequences.end(), Name) != CompletedSequences.end())
			{
				return;
			}

			CompletedSequences.push_back(Name);    //Add this sequence to the compiled sequences list
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
			//If this operation has already been compiled, return
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
			Save(OutputFile);
		}

		void Project::Save(std::ofstream& OutputFile)
		{
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

			//Write operations to the file
			int Len = this->Operations.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto op : this->Operations)
			{
				std::string Name = op.first;
				SaveString(Name, OutputFile);
				op.second->Save(OutputFile);
			}

			//Write sequences to the file
			Len = this->Sequences.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto sequence : this->Sequences)
			{
				std::string Name = sequence.first;
				SaveString(Name, OutputFile);
				sequence.second->Save(OutputFile);
			}

			//Write entry point to file
			Len = this->Main.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (GIL::Lexer::Token* t : this->Main)
			{
				t->Save(OutputFile);
			}

			//Write imports to the file for dynamic linking (WIP)
			Len = this->Imports.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (std::string& s : this->Imports)
			{
				SaveString(s, OutputFile);
			}

			//Write namespaces to the file (As of right now, this forces CGIL files to be statically linked)
			Len = this->Namespaces.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto Proj : this->Namespaces)
			{
				std::string Name = Proj.first;
				SaveString(Name, OutputFile);
				Proj.second->Save(OutputFile);
			}

			//Write the dll imports to the file
			Len = this->Usings.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (std::string& s : this->Usings)
			{
				SaveString(s, OutputFile);
			}
		}

		//CGIL function definitions:
		Project* LoadCGIL0(std::ifstream& InputFile);
		Project* LoadCGIL1(std::ifstream& InputFile);
		Project* LoadCGIL2(std::ifstream& InputFile);
		Project* LoadCGIL3(std::ifstream& InputFile);

		Project* Project::Load(std::string Path)
		{
			std::ifstream InputFile(Path, std::ios::binary|std::ios::in);
			return Project::Load(InputFile, Path);
		}
		Project* Project::Load(std::ifstream& InputFile, std::string& Path)
		{
			int Version = -1;
			InputFile.read((char*)&Version, sizeof(int));
			
			switch (Version)
			{
			case 0:
				return LoadCGIL0(InputFile);
			case 1:
				return LoadCGIL1(InputFile);
			case 2:
				return LoadCGIL2(InputFile);
			case 3:
				return LoadCGIL3(InputFile);
			default:
				CDB_BuildFatal("Error reading CGIL version from path {0}", Path);
			}
		}

		//Some more func defs
		void GetNamespace(std::vector<std::string*>& Namespaces, std::vector<Lexer::Token*>* Tokens, int i);

		Sequence* Project::GetSeq(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules)
		{
			std::string& SeqName = (*Tokens)[i]->Value;    //Save the name of the sequence
			if ((*Tokens)[i - 1]->TokenType == LexerToken::NAMESPACE)
			{
				std::vector<std::string*> Namespaces;
				GetNamespace(Namespaces, Tokens, i - 1);    //Get the namespaces. For example, N1::N2::N3 becomes [N1, N2, N3]
				if (Modules->contains(*Namespaces[0]))
				{
					return (*Modules)[*Namespaces[0]]->GetSequence(SeqName);
				}
				return this->Namespaces[*Namespaces[0]]->GetSeqFromNamespace(SeqName, Namespaces, 1, Modules);
			}
			return Sequences[SeqName];
		}

		//Recursively traverse namespaces until you get to the one with the sequence
		Sequence* Project::GetSeqFromNamespace(std::string& SeqName, std::vector<std::string*>& Namespaces, int i, 
			std::map<std::string, GILModule*>* Modules)
		{
			if (i < Namespaces.size())
				return this->Namespaces[*Namespaces[i]]->GetSeqFromNamespace(SeqName, Namespaces, i + 1, Modules);
			return Sequences[SeqName];
		}

		Operation* Project::GetOp(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules)
		{
			std::string& OpName = (*Tokens)[i]->Value;
			if ((*Tokens)[i - 1]->TokenType == LexerToken::NAMESPACE)
			{
				std::vector<std::string*> Namespaces;
				GetNamespace(Namespaces, Tokens, i - 1);
				if (Modules->contains(*Namespaces[0]))
				{
					return (*Modules)[*Namespaces[0]]->GetOperation(OpName);
				}
				return this->Namespaces[*Namespaces[0]]->GetOpFromNamespace(OpName, Namespaces, 1, Modules);
			}
			return Operations[OpName];
		}

		Operation* Project::GetOpFromNamespace(std::string& OpName, std::vector<std::string*>& Namespaces, int i,
			std::map<std::string, GILModule*>* Modules)
		{
			if (i < Namespaces.size())
				return this->Namespaces[*Namespaces[i]]->GetOpFromNamespace(OpName, Namespaces, i + 1, Modules);
			return Operations[OpName];
		}

		void GetNamespace(std::vector<std::string*>& Namespaces, std::vector<Lexer::Token*>* Tokens, int i)
		{
			int idx = i;
			for (idx; idx > -1; --idx)    //Find the first namespace token
			{
				if ((*Tokens)[idx]->TokenType != LexerToken::NAMESPACE)
				{
					++idx;
					break;
				}
			}
			for (idx; idx < i + 1; ++idx)
			{
				Namespaces.push_back(&(*Tokens)[idx]->Value);
			}
		}

		Project* LoadCGIL0(std::ifstream& InputFile)
		{
			int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

			int Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the operations
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Operations[Name] = new DynamicOperation();
				Proj->Operations[Name]->Load(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = new StaticSequence();
				Proj->Sequences[Name]->Load(InputFile);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
			Proj->Main.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				Proj->Main.push_back(Token::Load(InputFile));
			}
			return Proj;
		}

		Project* LoadCGIL1(std::ifstream& InputFile)
		{
			int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

			int Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the operations
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Operations[Name] = new DynamicOperation();
				Proj->Operations[Name]->Load(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = new StaticSequence();
				Proj->Sequences[Name]->Load(InputFile);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
			Proj->Main.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				Proj->Main.push_back(Token::Load(InputFile));
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the imports
			Proj->Imports.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				std::string buff;
				LoadStringFromFile(buff, InputFile);
				Proj->Imports.push_back(std::move(buff));
			}
			return Proj;
		}

		Project* LoadCGIL2(std::ifstream& InputFile)
		{
			int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

			int Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the operations
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Operations[Name] = new DynamicOperation();
				Proj->Operations[Name]->Load(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = new StaticSequence();
				Proj->Sequences[Name]->Load(InputFile);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
			Proj->Main.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				Proj->Main.push_back(Token::Load(InputFile));
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the imports
			Proj->Imports.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				std::string buff;
				LoadStringFromFile(buff, InputFile);
				Proj->Imports.push_back(std::move(buff));
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the namespaces
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Namespaces[Name] = Project::Load(InputFile, Name);
			}
			return Proj;
		}

		Project* LoadCGIL3(std::ifstream& InputFile)
		{
			int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

			int Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the operations
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Operations[Name] = new DynamicOperation();
				Proj->Operations[Name]->Load(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = new StaticSequence();
				Proj->Sequences[Name]->Load(InputFile);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
			Proj->Main.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				Proj->Main.push_back(Token::Load(InputFile));
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the imports
			Proj->Imports.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				std::string buff;
				LoadStringFromFile(buff, InputFile);
				Proj->Imports.push_back(std::move(buff));
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));    //Load the namespaces
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Namespaces[Name] = Project::Load(InputFile, Name);
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));
			Proj->Usings.reserve(Len);
			for (int i = 0; i < Len; ++i)
			{
				std::string buff;
				LoadStringFromFile(buff, InputFile);
				Proj->Usings.push_back(std::move(buff));
			}
			return Proj;
		}
	}
}