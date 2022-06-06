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
					if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected target organism name after #Target");
						break;
					}
					Target->TargetOrganism = Tokens[i]->Value;
					break;
				}
				case LexerToken::SETATTR:
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected attribute name after #SetAttr");
						break;
					}
					std::string AttrName = Tokens[i]->Value;
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected value after attribute name \"{0}\"", AttrName);
						break;
					}
					Target->Attributes[AttrName] = Tokens[i]->Value;
					break;
				}
				case LexerToken::CREATEVAR:
					if (i < Tokens.size() - 2)
					{
						//Get the type and variable name
						if (Tokens[i + 1]->TokenType != LexerToken::IDENT && Tokens[i + 1]->TokenType != LexerToken::STRING)
						{
							CDB_BuildError("Could not find type name for #Var");
							break;
						}
						if (Tokens[i + 2]->TokenType != LexerToken::IDENT && Tokens[i + 2]->TokenType != LexerToken::STRING)
						{
							CDB_BuildError("Could not find variable name for #Var");
							break;
						}
						//Just because you can create a numerical variable with the type "tree" doesn't mean you should
						bool IsStr = Tokens[i + 1]->Value == "str" || Tokens[i + 1]->Value == "Str";
						std::string& VarName = Tokens[i + 2]->Value;
						i += 2;
						
						//If there's a value specified, set the variable to the value
						if (i < Tokens.size() - 1)
						{
							i += 1;
							if (IsStr)
							{
								Target->StrVars[VarName] = Tokens[i]->Value;
							}
							else
							{
								Target->NumVars[VarName] = std::stod(Tokens[i]->Value);
							}
						}
						else    //Otherwise, set it to a default value
						{
							if (IsStr)
							{
								Target->StrVars[VarName] = "";
							}
							else
							{
								Target->NumVars[VarName] = 0.0;
							}
						}
						break;
					}
					CDB_BuildError("#Var requires a type name and variable name, file ends before these are found");
					break;
				case LexerToken::OPTIMIZE:    //Optimization settings
				{
					++i;
					if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::STRING)
					{
						CDB_BuildError("Expected optimization name after #Optimize");
						break;
					}
					//TODO: If the number of optimizations get big enough, I should rewrite this to use a switch statement
					if (Tokens[i]->Value == "AVOID_RS")
					{
						++i;
						if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::STRING)
						{
							CDB_BuildError("Expected restriction site name after #Optimize AVOID_RS");
							break;
						}
						Target->AvoidRSites.push_back(Tokens[i]->Value);
						break;
					}
					else
					{
						CDB_BuildError("Unknown optimization name \"{0}\"", Tokens[i]->Value);
						break;
					}
				}
				case LexerToken::TYPEDEF:
					if (i >= Tokens.size() - 1 || Tokens[i + 1]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("Typedef was called without a type to define");
						break;
					}
					++i;
					Target->AllocType(Tokens[i]->Value);
					break;
				case LexerToken::INHERITS:
					if (Tokens[i - 1]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("\"inherits\" keyword requires name of child type");
						break;
					}
					if (i >= Tokens.size() - 1 || Tokens[i + 1]->TokenType != LexerToken::IDENT)
					{
						CDB_BuildError("\"inherits\" keyword requires name of parent type (child type was \"{0}\")", Tokens[i - 1]->Value);
						break;
					}
					Target->AddInheritance(Target->GetTypeByName(Tokens[i - 1]->Value), Target->GetTypeByName(Tokens[i + 1]->Value));

					//If there's an extra IDENT, remove it
					if (Target->Main[Target->Main.size() - 1] == Tokens[i - 1])
						Target->Main.erase(Target->Main.end() - 1);
					++i;
					break;
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
				GetSequence(it->first, it->second, CreatedSequences, CreatedOps, Sequences, Operations, Target);
			}

			for (int i = 0; i < Target->Main.size(); ++i)    //Do forwards
			{
				switch (Target->Main[i]->TokenType)
				{
				case LexerToken::FORWARD:
					if (!(i > 0 && Target->Main[i - 1]->TokenType == LexerToken::IDENT))
					{
						CDB_BuildError("Missing IDENT token before or after forward (=>)");
						break;
					}

					if (Target->Main[i + 1]->TokenType == LexerToken::IDENT)
					{
						if (Target->Sequences.contains(Target->Main[i + 1]->Value))
						{
							//If the forward's pointing to a valid sequence set the first part to the second part's pointer
							Sequence* Seq = new SequenceForward(Target->Sequences[Target->Main[i + 1]->Value], Target->Main[i + 1]->Value);
							Seq->ParamIdx2Name = Target->Sequences[Target->Main[i + 1]->Value]->ParamIdx2Name;
							Target->Sequences[Target->Main[i - 1]->Value] = Seq;

							//Make sure the forward doesn't get added
							Target->Main.erase(Target->Main.begin() + i - 1, Target->Main.begin() + i + 2);
							i -= 2;
							break;
						}

						CDB_BuildError("Could not find sequence or operation {0}", Target->Main[i + 1]->Value);
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
				case LexerToken::LPAREN:
				case LexerToken::BEGIN:
					++Depth;
					break;
				case LexerToken::RPAREN:
					--Depth;
					continue;
				case LexerToken::END:
					--Depth;
					break;
				case LexerToken::ASSIGNTYPE:
					if (Tokens[i + 1]->TokenType == LexerToken::IDENT)
					{
						++NumTokens;
						++i;
					}
					continue;
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

		void GetTypeInfo(Project* Proj, Sequence* Seq, int& i, std::vector<Token*>& Tokens);

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
			for (int i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::IDENT:
					if (Tokens[i - 1]->TokenType == LexerToken::ASSIGNTYPE)
						break;
					//GetSequence(Tokens[i]->Value, Sequences[Tokens[i]->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;
				case LexerToken::CALLOP:
					//GetOperation(Tokens[i]->Value, Operations[Tokens[i]->Value], CompletedSequences, CompletedOps, Sequences, Operations, Target);
					break;

				//Skip parentheses
				case LexerToken::LPAREN:
					Compiler::GetTokensInBetween(Tokens, i, LexerToken::LPAREN, LexerToken::RPAREN);
					break;
				default:
					break;
				}
			}

			//Get the parameters
			int i = 0;
			StaticSequence* Seq = new StaticSequence();
			if (Tokens[0]->TokenType == LexerToken::LPAREN)
			{
				for (i = 1; i < Tokens.size(); ++i)
				{
					if (Tokens[i]->TokenType == LexerToken::PARAM && Tokens[i]->Value != "InnerCode")
					{
						Seq->ParamIdx2Name.push_back(Tokens[i]->Value);
					}
					else if (Tokens[i]->TokenType == LexerToken::RPAREN)
					{
						++i;
						break;
					}
				}
			}

			//Now get types if they were provided (syntax "sequence s($Type1 $Type2) : (any, cds) : cds")
			if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE)
			{
				GetTypeInfo(Target, Seq, i, Tokens);
			}

			Seq->SetTokens(std::vector<Token*>(Tokens.begin() + i, Tokens.end()));

			Target->Sequences[Name] = Seq;
		}

		void GetTypeInfo(Project* Proj, Sequence* Seq, int& i, std::vector<Token*>& Tokens)
		{
			if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE && Tokens[i + 1]->TokenType == LexerToken::LPAREN)
				++i;
			//Get the parameter types
			if (Tokens[i]->TokenType == LexerToken::LPAREN)
			{
				++i;
				int ParamIdx = 0;
				for (i; i < Tokens.size(); ++i)
				{
					if (Tokens[i]->TokenType == LexerToken::IDENT)
					{
						Seq->ParameterTypes[Seq->ParamIdx2Name[ParamIdx]] = Proj->GetTypeByName(Tokens[i]->Value);
						++ParamIdx;
					}
					else if (Tokens[i]->TokenType == LexerToken::PARAM)
					{
						if (i >= Tokens.size() - 2)
						{
							CDB_BuildError("Parameter \"{0}\" was typed by name, but no type was given", Tokens[i]->Value);
							return;
						}

						//Parameters can be assigned types by name using the syntax "sequence seq($Param) : ($Param : cds)"
						if (Tokens[i + 1]->TokenType != LexerToken::ASSIGNTYPE)
						{
							CDB_BuildError("Parameter \"{0}\" was typed by name, but the type was not assigned (types are assigned using a colon)", Tokens[i]->Value);
							return;
						}

						if (Tokens[i + 2]->TokenType != LexerToken::IDENT)
						{
							CDB_BuildError("Parameter \"{0}\" was typed by name, but no type was given", Tokens[i]->TokenType);
							return;
						}

						//Set the type
						Seq->ParameterTypes[Tokens[i]->Value] = Proj->GetTypeByName(Tokens[i + 2]->Value);
						i += 2;
					}
					else if (Tokens[i]->TokenType == LexerToken::RPAREN)
					{
						++i;
						break;
					}
				}
			}

			//Now get the sequence's type
			if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE)
			{
				if (i >= Tokens.size() - 1 || Tokens[i + 1]->TokenType != LexerToken::IDENT)
				{
					CDB_BuildError("Sequence was assigned a type, but no type was given");
					return;
				}

				Seq->SeqType = Proj->GetTypeByName(Tokens[i + 1]->Value);
				i += 2;
			}
		}

		
		//This function is deprecated and was used back when the compiler differentiated between sequences and operations. It will be removed in the future.
		void GetOperation(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target)
		{
			//If this operation has already been compiled, return
			if (std::find(CompletedOps.begin(), CompletedOps.end(), Name) != CompletedOps.end())
			{
				return;
			}

			CompletedOps.push_back(Name);

			for (int i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::LPAREN:
					Compiler::GetTokensInBetween(Tokens, i, LexerToken::LPAREN, LexerToken::RPAREN);
					break;
				default:
					break;
				}
			}

			//Get the parameters
			if (Tokens[0]->TokenType == LexerToken::LPAREN)
			{
				StaticSequence* NewOp = new StaticSequence();
				int i = 1;
				for (i = 1; i < Tokens.size(); ++i)
				{
					if (Tokens[i]->TokenType == LexerToken::PARAM && Tokens[i]->Value != "InnerCode")
					{
						NewOp->ParamIdx2Name.push_back(Tokens[i]->Value);
					}
					else if (Tokens[i]->TokenType == LexerToken::RPAREN)
					{
						++i;
						break;
					}
				}

				//Now get types if they were provided (syntax "sequence s($Type1 $Type2) : (any, cds) : cds")
				if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE)
				{
					++i;
					GetTypeInfo(Target, NewOp, i, Tokens);
				}

				NewOp->SetTokens(std::vector<Token*>(Tokens.begin() + i, Tokens.end()));
				Target->Sequences[Name] = NewOp;
			}
			else
			{
				Target->Sequences[Name] = new StaticSequence(Tokens);
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

			//Write the variables to the file
			Len = this->StrVars.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto& var : this->StrVars)
			{
				SaveString(var.first, OutputFile);
				SaveString(var.second, OutputFile);
			}

			Len = this->NumVars.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (auto& var : this->NumVars)
			{
				SaveString(var.first, OutputFile);
				OutputFile.write((char*)&var.second, sizeof(double));
			}

			//Write the types to the file
			Len = this->Types.size();
			OutputFile.write((char*)&Len, sizeof(int));
			for (Type* type : this->Types)
			{
				if (type->IsUserType())
				{
					type->Save(OutputFile);
				}
			}
		}

		//CGIL function definitions:
		Project* LoadCGIL4(std::ifstream& InputFile);

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
			case 1:
			case 2:
			case 3:
				CDB_BuildFatal("CGIL version {0} is not supported by this version of GIL", Version);
				return nullptr;    //Just in case
			case 4:
				return LoadCGIL4(InputFile);
			default:
				CDB_BuildFatal("Error reading CGIL version from path {0}", Path);
				return nullptr;
			}
		}

		//Some more func defs
		void GetNamespace(std::vector<std::string*>& Namespaces, std::vector<Lexer::Token*>* Tokens, int i);

		std::pair<Sequence*, Project*> Project::GetSeq(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules)
		{
			std::string& SeqName = (*Tokens)[i]->Value;    //Save the name of the sequence
			if ((*Tokens)[i - 1]->TokenType == LexerToken::NAMESPACE)
			{
				std::vector<std::string*> Namespaces;
				GetNamespace(Namespaces, Tokens, i - 1);    //Get the namespaces. For example, N1::N2::N3 becomes [N1, N2, N3]
				if (Modules->contains(*Namespaces[0]))
				{
					return { (*Modules)[*Namespaces[0]]->GetSequence(SeqName), nullptr };
				}
				return { this->Namespaces[*Namespaces[0]]->GetSeqFromNamespace(SeqName, Namespaces, 1, Modules), this->Namespaces[*Namespaces[0]] };
			}
			return { Sequences[SeqName], this };
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



		uint16_t Project::AllocType(std::string TypeName)
		{
			//If the type already exists, return a reference to it
			if (TypeName2Idx.contains(TypeName))
				return TypeName2Idx[TypeName];
			
			//Throw an error if the max number of types have been created
			if (Types.size() == UINT16_MAX)
			{
				CDB_BuildError("Failed to allocate new type with name \"{0}\", too many types have been created (max {1})", TypeName, UINT16_MAX - 1);
				return UINT16_MAX;
			}

			uint16_t ID = (uint16_t)Types.size();
			TypeName2Idx[TypeName] = ID;
			Types.push_back(new Type(TypeName, ID));
			return ID;
		}

		Type* Project::GetTypeByID(uint16_t ID)
		{
			return Types[ID];
		}

		Type* Project::GetTypeByName(std::string& Name)
		{
			return Types[TypeName2Idx[Name]];
		}

		void Project::AddInheritance(Type* child, Type* parent)
		{
			child->AddInheritance(parent);
		}

		void Project::AddInheritance(Type* child, uint16_t parent)
		{
			AddInheritance(child, Types[parent]);
		}

		void Project::AddInheritance(uint16_t child, uint16_t parent)
		{
			AddInheritance(Types[child], Types[parent]);
		}



		void Project::RemoveInheritance(Type* child, Type* parent)
		{
			child->RemoveInheritance(parent);
		}

		void Project::RemoveInheritance(Type* child, uint16_t parent)
		{
			RemoveInheritance(child, Types[parent]);
		}

		void Project::RemoveInheritance(uint16_t child, uint16_t parent)
		{
			RemoveInheritance(Types[child], Types[parent]);
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

		Project* LoadCGIL4(std::ifstream& InputFile)
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
				Proj->Operations[Name] = Operation::LoadOperation(InputFile);
			}

			Len = -1;
			InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
			for (int i = 0; i < Len; ++i)
			{
				std::string Name;
				LoadStringFromFile(Name, InputFile);
				Proj->Sequences[Name] = Sequence::LoadSequence(InputFile, Proj);
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

			//Load the variables
			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));
			for (int i = 0; i < Len; ++i)
			{
				std::string key;
				std::string val;
				LoadStringFromFile(key, InputFile);
				LoadStringFromFile(val, InputFile);
				Proj->StrVars[key] = val;
			}

			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));
			for (int i = 0; i < Len; ++i)
			{
				std::string key;
				double val;
				LoadStringFromFile(key, InputFile);
				InputFile.read((char*)&val, sizeof(double));
				Proj->StrVars[key] = val;
			}
			return Proj;

			//Load the types
			Len = 0;
			InputFile.read((char*)&Len, sizeof(int));
			for (int i = 0; i < Len; ++i)
			{
				Type().Load(InputFile, Proj);
			}
		}
	}
}