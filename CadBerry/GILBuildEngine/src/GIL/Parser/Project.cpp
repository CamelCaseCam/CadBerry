#include "gilpch.h"
#include "Project.h"
#include "GIL/Compiler/Compiler.h"
#include "Sequence.h"
#include "GIL/SaveFunctions.h"

#include "GIL/Bools/BoolNode.h"
#include "GIL/Bools/BoolImplementation.h"

#include "GIL/Errors.h"

#include "GIL/CGIL/CGIL_info.h"
#include "Core.h"
#include "GIL/GILException.h"

#include "GIL/Parser/AST_Node.h"

#include "GIL/Compiler/Compiler.h"


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

		void CreateBool(std::string& BoolName, std::vector<Token*>& Tokens, int& i, bool& SyntaxError);

		ParamNode* GetParam(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError);

		AccessNamespace CheckNamespace(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError);

		void GetDistributions(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError, std::vector<std::string>& Distributions);

		BoolNode* ParseBoolExpression(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError, BoolNode* LVALUE = nullptr);


		std::vector<Token*> GetTokensUntil(std::vector<Token*> Tokens, size_t& i, LexerToken End)
		{
			int StartIdx;
			for (i; i < Tokens.size(); ++i)
				if (Tokens[i]->TokenType == End)
					goto FoundEnd;
			return {};
		FoundEnd:
			//Now return the sub-vector
			return std::vector<Token*>(Tokens.begin() + StartIdx, Tokens.begin() + i);
		}
		

		//Checks if IDENT could be DNA
		bool IsDNA(std::string& s);
		//Gets tokens in between { and }
		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, size_t& i);


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

			for (AST_Node* node : this->Main)
			{
				delete node;
			}

			for (auto p : Namespaces)
			{
				delete p.second;
			}
		}


#define Require(cond, token, msg) if (!(cond)) { SyntaxError = true; Error(msg, token); break; }
#define mv(x) std::move(x)

//Assumes that you've already checked for length
#define GetIdent(Name, FirstToken, ErrorCode) Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ErrorCode);\
std::string& Name = Tokens[i + 1]->Value; i += 2

		std::vector<AST_Node*> ParseNodes(std::vector<Token*>& Tokens, Project* Target)
		{
			bool SyntaxError = false;
			std::vector<AST_Node*> OutputNodes;
			std::vector<std::string> Distributions;
			AccessNamespace Namespaces;
			for (size_t i = 0; i < Tokens.size(); ++i)
			{
				switch (Tokens[i]->TokenType)
				{
				case LexerToken::IMPORT:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_001);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_001);
					auto Node = new Import(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
				case LexerToken::USING:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_002);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_002);
					auto Node = new Using(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
#pragma region SimplePrepros
				case LexerToken::SETTARGET:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_003);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_003);
					auto Node = new SetTarget(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
				case LexerToken::SETATTR:
				{
					Require(i + 2 < Tokens.size(), Tokens[i], ERROR_004);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_004);
					Require(Tokens[i + 2]->TokenType == LexerToken::IDENT || Tokens[i + 2]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_004);
					auto Node = new SetAttr(mv(Tokens[i + 1]->Value), mv(Tokens[i + 2]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					i += 2;
					break;
				}
				//#var name type value
				case LexerToken::CREATEVAR:
				{
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_005);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING, Tokens[i], ERROR_005);
					Require(Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_005);
					Require(Tokens[i + 3]->TokenType == LexerToken::IDENT || Tokens[i + 3]->TokenType == LexerToken::STRING, Tokens[i], ERROR_005);
					auto Node = new SetVar(Tokens[i + 2]->Value, mv(Tokens[i + 1]->Value), mv(Tokens[i + 3]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					i += 3;
					break;
				}
				case LexerToken::BEGINREGION:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_006);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING,
						Tokens[i], ERROR_006);
					auto Node = new BeginRegion(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
				case LexerToken::ENDREGION:
				{
					//If the next token is a string or IDENT
					if (i + 1 < Tokens.size() && (Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING))
					{
						auto Node = new EndRegion(mv(Tokens[i + 1]->Value));
						Node->pos.Line = Tokens[i]->line;
						OutputNodes.push_back(Node);
						++i;
						break;
					}
					auto Node = new EndRegion();
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::INC:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_007);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING, Tokens[i], ERROR_007);
					auto Node = new IncVar(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
				case LexerToken::DEC:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_008);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING, Tokens[i], ERROR_008);
					auto Node = new DecVar(mv(Tokens[i + 1]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					++i;
					break;
				}
#pragma endregion
#pragma region PreproConditions
				case LexerToken::PREPRO_IF:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_009);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_009);
					std::string& Condition = Tokens[i + 1]->Value;
					Token* FirstToken = Tokens[i];
					i += 2;

					//Get the parameters for the condition
					auto ParamTokens = Compiler::GetTokensInBetween(Tokens, i, LexerToken::LPAREN, LexerToken::RPAREN);

					//Get the body (can end with either a #EndIf or a #Else
					++i;
					size_t Start = i;
					std::vector<Token*> InsideTokens;
					for (i; i < Tokens.size(); ++i)
					{
						if (Tokens[i]->TokenType == LexerToken::PREPRO_ENDIF)
						{
							InsideTokens = std::vector<Token*>(Tokens.begin() + Start, Tokens.begin() + i);

							goto PREPRO_IF_FoundEnd;
						}
						if (Tokens[i]->TokenType == LexerToken::PREPRO_ELSE)
						{
							InsideTokens = std::vector<Token*>(Tokens.begin() + Start, Tokens.begin() + i);
							--i;

							goto PREPRO_IF_FoundEnd;
						}
					}
					Error(ERROR_010, FirstToken);
					break;
				PREPRO_IF_FoundEnd:
					//Now parse the inner tokens
					auto InsideNodes = ParseNodes(InsideTokens, Target);
					auto Node = new Prepro_If(mv(Condition), mv(ParamTokens), mv(InsideNodes));
					Node->pos.Line = FirstToken->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::PREPRO_ELSE:
				{
					Token* FirstToken = Tokens[i];
					
					++i;
					size_t Start = i;
					//Get tokens
					for (i; i < Tokens.size(); ++i)
					{
						if (Tokens[i]->TokenType == LexerToken::PREPRO_ENDIF)
						{
							auto InsideTokens = std::vector<Token*>(Tokens.begin() + Start, Tokens.begin() + i);
							auto InsideNodes = ParseNodes(InsideTokens, Target);
							auto Node = new Prepro_Else(mv(InsideNodes));
							Node->pos.Line = FirstToken->line;							
							OutputNodes.push_back(Node);
							goto EndSwitch;
						}
					}
					Error(ERROR_012, FirstToken);
					
					EndSwitch:
					break;
				}
#pragma endregion
#pragma region BoolParsing
				case LexerToken::IBOOL:
				{
					Require(i + 2 < Tokens.size(), Tokens[i], ERROR_036);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_037);
					Require(Tokens[i + 2]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_038);
					std::string ImplName = Tokens[i + 1]->Value;

					//Now parse the nodes inside the bool definition
					i += 2;
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto InsideNodes = ParseNodes(InsideTokens, Target);
					
					//Make the implementation
					BoolImplementation* Impl = new BoolImplementation(ImplName);
					for (AST_Node* node : InsideNodes)
					{
						if (node->GetName() == "Forward")
						{
							Forward* fwd = (Forward*)node;
							SequenceForward* seq_forward = new SequenceForward();
							seq_forward->DestinationName = fwd->Destination;
							seq_forward->Location = std::move(fwd->Location);

#define name(upper, lower) if (fwd->Origin == #upper || fwd->Origin == #lower) { Impl-> ##upper = seq_forward; }
#define oname(upper, lower) if (fwd->Origin == #upper || fwd->Origin == #lower) { Impl-> ##upper = seq_forward; Impl-> ## upper ##Available = true; }
							
							name(Alloc, alloc)
							else name(Dealloc, dealloc)
							else name(Set, set)
							else name(Use, use)
							else name(NumAvailable, numAvailable)
							else name(And, and)
							else name(Or, or)
							else name(Not, not)
							else oname(Xor, xor)
							else oname(Nand, nand)
							else oname(Nor, nor)
							else oname(Xnor, xnor)
							else oname(ChainedAnd, chainedAnd)
							else oname(ChainedOr, chainedOr)
							else
							{
								//If there's an unrecognized forward, add it to the additional sequences map
								Impl->AdditionalSequences[fwd->Origin] = seq_forward;
							}
						}
						delete node;
#undef name
					}
					
					//Make sure the implementations are valid
#define Check(Name) if (Impl-> ##Name == nullptr) { CDB_BuildError(ERROR_039, ImplName, #Name); throw GILException(); }
					Check(Alloc)
					Check(Dealloc)
					Check(Set)
					Check(Use)
					Check(NumAvailable)
					Check(And)
					Check(Or)
					Check(Not)
#undef Check
					Target->BoolImplementations.push_back(Impl);
					
					break;
				}
				case LexerToken::BOOL:
				{
					//Defining bools
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_040);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_040);
					
					//Get the name of the bool
					std::string Name = Tokens[i + 1]->Value;
					
					//Check if there's a bool with the same name. Bools are added as sequences with the bool type (to prevent shadowing)
					if (Target->Sequences.contains(Name))
					{
						Error(ERROR_041, Tokens[i]);
						throw GILException();
					}
					GILBool* Bool = new GILBool(Name);
					Target->LocalBools[Name] = (Bool);
					Target->Sequences[Name] = new BoolSequence(Bool);
					
					//Check if the bool is being defined
					++i;
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::EQUALS)
						--i;	//Go back to the bool name so it can be parsed as an identifier
					break;
				}
				
#pragma endregion
				
				case LexerToken::LBRACKET:
					GetDistributions(Tokens, i, SyntaxError, Distributions);
					continue;
				case LexerToken::FOR:
				{
					Token* FirstToken = Tokens[i];
					//Get the target organism
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_015);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING, Tokens[i],
						ERROR_015);
					std::string& NewTarget = Tokens[i + 1]->Value;
					i += 2;
					Require(Tokens[i]->TokenType == LexerToken::BEGIN, FirstToken, ERROR_016);
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto InsideNodes = ParseNodes(InsideTokens, Target);
					auto Node = new For(mv(NewTarget), mv(InsideNodes));
					Node->pos.Line = FirstToken->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::FROM:
				{
					Token* FirstToken = Tokens[i];
					//Get the target organism
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_017);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT || Tokens[i + 1]->TokenType == LexerToken::STRING, Tokens[i],
						ERROR_017);
					std::string& NewTarget = Tokens[i + 1]->Value;
					i += 2;
					Require(Tokens[i]->TokenType == LexerToken::BEGIN, FirstToken, ERROR_018);
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto InsideNodes = ParseNodes(InsideTokens, Target);
					auto Node = new For(mv(NewTarget), mv(InsideNodes));
					Node->pos.Line = FirstToken->line;					
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::DEFINESEQUENCE:
				{
					Token* FirstToken = Tokens[i];
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_019);
					GetIdent(SeqName, FirstToken, ERROR_019);

					//Defines for later
					Params SeqParams;
					std::string* Type = nullptr;
					if (Tokens[i]->TokenType == LexerToken::LPAREN)
					{
						//This sequence takes parameters
						++i;
						for (i; i < Tokens.size(); ++i)
						{
							if (Tokens[i]->TokenType == LexerToken::RPAREN)
								break;
							if (Tokens[i]->TokenType == LexerToken::PARAM)
								SeqParams.m_Params.push_back(mv(GetParam(Tokens, i, SyntaxError)));
							else
							{
								Error(ERROR_013, Tokens[i]);
							}
						}
						++i;
					}
					Require(i + 2 < Tokens.size(), FirstToken, ERROR_020);
					if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE)
					{
						//This sequence has a type
						Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i + 1], ERROR_021);
						Type = &Tokens[i + 1]->Value;
						i += 2;
					}
					//Get the body of the sequence
					Require(Tokens[i]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_020);
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto Nodes = ParseNodes(InsideTokens, Target);

					if (Type == nullptr)
					{
						auto Node = new DefineSequence(mv(SeqName), mv(SeqParams), "any", mv(Nodes));
						Node->pos.Line = FirstToken->line;
						Node->ActiveDistributions = Distributions;						
						OutputNodes.push_back(Node);
						break;
					}
					auto Node = new DefineSequence(mv(SeqName), mv(SeqParams), mv(*Type), mv(Nodes));
					Node->pos.Line = FirstToken->line;
					Node->ActiveDistributions = Distributions;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::DEFOP:
				{
					Token* FirstToken = Tokens[i];
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_019);
					GetIdent(SeqName, FirstToken, ERROR_019);

					//Defines for later
					Params SeqParams;
					std::string* Type = nullptr;
					if (Tokens[i]->TokenType == LexerToken::LPAREN)
					{
						//This sequence takes parameters
						++i;
						for (i; i < Tokens.size(); ++i)
						{
							if (Tokens[i]->TokenType == LexerToken::RPAREN)
								break;
							if (Tokens[i]->TokenType == LexerToken::PARAM)
								SeqParams.m_Params.push_back(mv(GetParam(Tokens, i, SyntaxError)));
							else
							{
								Error(ERROR_013, Tokens[i]);
							}
						}
						++i;
					}
					Require(i + 2 < Tokens.size(), FirstToken, ERROR_020);
					if (Tokens[i]->TokenType == LexerToken::ASSIGNTYPE)
					{
						//This sequence has a type
						Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i + 1], ERROR_021);
						Type = &Tokens[i + 1]->Value;
						i += 2;
					}
					//Get the body of the sequence
					Require(Tokens[i]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_020);
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto Nodes = ParseNodes(InsideTokens, Target);

					if (Type == nullptr)
					{
						auto Node = new DefineOperation(mv(SeqName), mv(SeqParams), "any", mv(Nodes));
						Node->pos.Line = FirstToken->line;
						Node->ActiveDistributions = Distributions;
						OutputNodes.push_back(Node);
						break;
					}
					auto Node = new DefineOperation(mv(SeqName), mv(SeqParams), mv(*Type), mv(Nodes));
					Node->pos.Line = FirstToken->line;
					Node->ActiveDistributions = Distributions;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::OPERATOR:
				{
					Require(i + 2 < Tokens.size(), Tokens[i], ERROR_022);
					Require(Tokens[i + 1]->TokenType == LexerToken::FORWARD && Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_024);
					auto Node = new DefineOperator(mv(Tokens[i]->Value), mv(Tokens[i + 2]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					i += 2;
					break;
				}

				case LexerToken::IF:
				{
					//An IF node
					++i;
					BoolNode* Condition = ParseBoolExpression(Tokens, i, SyntaxError);
					Require(i < Tokens.size() && Tokens[i]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_020);
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto Nodes = ParseNodes(InsideTokens, Target);
					auto statements = MakeIfStatements(Condition, Nodes, Target);
					for (N_UseBool* statement : statements)
					{
						OutputNodes.push_back(statement);
					}
					break;
				}

					//This is going to be a big one
				case LexerToken::IDENT:
				{
					//Is this a bool assignment?
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::EQUALS)
					{
						//There are two options. Either the bool is being assigned to true, or it's being assigned to a sequence
						Require(i + 2 < Tokens.size(), Tokens[i], ERROR_042);
						
						std::string& Name = Tokens[i]->Value;
						
						i += 2;
						if (Tokens[i]->TokenType == LexerToken::BOOL_TRUE)
						{
							auto Node = new N_SetBoolTrue(mv(Namespaces), mv(Name));
							Node->pos.Line = Tokens[i]->line;
							OutputNodes.push_back(Node);
						}
						else
						{
							//This could either be setting the bool to a sequence or another bool
							auto Node = ParseBoolExpression(Tokens, i, SyntaxError, nullptr);
							Target->GraphHeads.push_back(Node);
							//Node is a placeholder or boolean value - turn it into a setbool node
							auto outnode = new N_SetBool(mv(Namespaces), Name, Node);
							OutputNodes.push_back(outnode);
							--i;
						}
						break;
					}
					
					//Is this a forward?
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::FORWARD)
					{
						//This is a forward
						Require(i + 2 < Tokens.size(), Tokens[i], ERROR_025);
						Require(Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_025);
						//Check if the destination has a namespace
						auto Origin = Tokens[i];
						i += 2;
						AccessNamespace destNamespace = CheckNamespace(Tokens, i, SyntaxError);
												
						auto Node = new Forward(mv(Origin->Value), mv(Tokens[i]->Value), mv(destNamespace));
						Node->pos.Line = Origin->line;
						OutputNodes.push_back(Node);
						break;
					}
					
					//Is this a type inheritance?
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::INHERITS)
					{
						Require(i + 2 < Tokens.size(), Tokens[i], ERROR_028);
						Require(Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_028);
						auto Node = new InheritTypedef(mv(Tokens[i]->Value), mv(Tokens[i + 2]->Value));
						Node->pos.Line = Tokens[i]->line;
						OutputNodes.push_back(Node);
						i += 2;
						break;
					}
					
					
					//This is a sequence/operation being "called"
					//Check if it has namespaces
					AccessNamespace newns = CheckNamespace(Tokens, i, SyntaxError);
					if (newns.Namespaces.size() != 0)
					{
						Namespaces = std::move(newns);
						--i;
						continue;
					}

					
					//Check if it has params
					Token* FirstToken = Tokens[i];
					std::string& SeqName = Tokens[i]->Value;
					std::vector<Token*> params;
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::LPAREN)
					{
						i += 2;
						for (i; i < Tokens.size(); ++i)
						{
							if (Tokens[i]->TokenType == LexerToken::RPAREN)
								break;
							if (Tokens[i]->TokenType == LexerToken::COMMA)
								continue;
							params.push_back(Tokens[i]);
						}
					}
					Call_Params SeqParams(mv(params));


					auto Node = new CallSequence(mv(SeqName), mv(SeqParams), mv(Namespaces));
					Node->pos.Line = FirstToken->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::CALLOP:
				{
					//Check if it has params
					std::string& OpName = Tokens[i]->Value;
					std::vector<Token*> params;
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::LPAREN)
					{
						i += 2;
						for (i; i < Tokens.size(); ++i)
						{
							if (Tokens[i]->TokenType == LexerToken::RPAREN)
								break;
							if (Tokens[i]->TokenType == LexerToken::COMMA)
								continue;
							params.push_back(Tokens[i]);
						}
					}
					Call_Params OpParams(mv(params));
					//Now make sure it has a body
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_029);
					Require(Tokens[i + 1]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_029);
					i += 1;
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto Nodes = ParseNodes(InsideTokens, Target);


					auto Node = new CallOperation(mv(OpName), mv(OpParams), mv(Nodes), mv(Namespaces));
					Node->pos.Line = Tokens[i]->line;					
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::DNA:
				{
					auto Node = new DNALiteral(mv(Tokens[i]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::AMINOS:
				{
					auto Node = new AminoAcidLiteral(mv(Tokens[i]->Value));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::TYPEDEF:
				{
					Require(i + 1 < Tokens.size(), Tokens[i], ERROR_027);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_027);
					
					std::string& Name = Tokens[i + 1]->Value;
					std::string Inherits;
					++i;
					//If it includes an inheritance
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::INHERITS)
					{
						//Make sure it has a type to inherit from
						Require(i + 2 < Tokens.size(), Tokens[i], ERROR_028);
						Require(Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_028);
						Inherits = mv(Tokens[i + 2]->Value);
						i += 2;
					}

					auto Node = new TypedefNode(mv(Name), mv(Inherits));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::PARAM:
				{
					//This is a sequence passed as a parameter being "called"


					//Check if it has params
					std::string& SeqName = Tokens[i]->Value;
					std::vector<Token*> params;
					if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::LPAREN)
					{
						i += 2;
						for (i; i < Tokens.size(); ++i)
						{
							if (Tokens[i]->TokenType == LexerToken::RPAREN)
								break;
							if (Tokens[i]->TokenType == LexerToken::COMMA)
								continue;
							params.push_back(Tokens[i]);
						}
					}
					Call_Params SeqParams(mv(params));


					auto Node = new UseParam(mv(SeqName), mv(SeqParams));
					Node->pos.Line = Tokens[i]->line;
					OutputNodes.push_back(Node);
					break;
				}
				case LexerToken::CREATENAMESPACE:
				{
					Require(i + 3 < Tokens.size(), Tokens[i], ERROR_030);
					Require(Tokens[i + 1]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_031);
					Require(Tokens[i + 2]->TokenType == LexerToken::BEGIN, Tokens[i], ERROR_032);
					std::string& Name = Tokens[i + 1]->Value;
					i += 2;
					auto InsideTokens = GetInsideTokens(Tokens, i);
					auto Nodes = ParseNodes(InsideTokens, Target);
					auto Node = new Namespace(mv(Name), mv(Nodes));
					Node->pos.Line = Tokens[i]->line;					
					OutputNodes.push_back(Node);
					break;
				}
				}
				
				//If nothing used the distributions, then we can remove them
				Distributions = {};
				Namespaces = {};
			}

			//If there was/were syntax error(s), throw an exception
			if (SyntaxError)
			{
				CDB_BuildError("Syntax errors detected during parsing, terminating compilation");
				throw GILException();
			}
			return OutputNodes;
		}

#undef Require
#define Require(cond, token, msg) if (!(cond)) { SyntaxError = true; Error(msg, token); return nullptr; }
		ParamNode* GetParam(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError)
		{
			Require(Tokens[i]->TokenType == LexerToken::PARAM, Tokens[i], ERROR_013);
			if (i + 2 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::ASSIGNTYPE)
			{
				//Make sure the type is present
				Require(Tokens[i + 2]->TokenType == LexerToken::IDENT, Tokens[i], ERROR_014);


				//Now construc the node
				i += 2;
				ParamNode* node = new ParamNode(mv(Tokens[i - 2]->Value), mv(Tokens[i]->Value));
				if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::COMMA)
					++i;
				return node;
			}
			//Return an untyped parameter
			ParamNode* node = new ParamNode(mv(Tokens[i]->Value));
			if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::COMMA)
				++i;
			return node;
		}


#undef Require
#define Require(cond, token, msg) if (!(cond)) { SyntaxError = true; Error(msg, token); return {}; }
		AccessNamespace CheckNamespace(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError)
		{
			//Check if this is a namespace
			if (i + 1 == Tokens.size() || Tokens[i + 1]->TokenType != LexerToken::NAMESPACE)
			{
				return AccessNamespace(std::vector<Token*>());
			}
			
			//Now move forwards to find the last namespace
			size_t Start = i;
			for (i; i < Tokens.size(); ++i)
			{
				if (Tokens[i]->TokenType != LexerToken::IDENT && Tokens[i]->TokenType != LexerToken::CALLOP)
				{
					Error(ERROR_026, Tokens[i]);
					return {};
				}
				//If NAMESPACE is the first token, that's an error
				if (i + 1 < Tokens.size() && Tokens[i + 1]->TokenType == LexerToken::NAMESPACE)
				{
					++i;
					continue;
				}
				break;
			}
			//Now we have the start of the namespace
			std::vector<Token*> Namespace;
			for (Start; Start < i; Start += 2)
			{
				Namespace.push_back(Tokens[Start]);
			}
			return AccessNamespace(mv(Namespace));
		}

		void GetDistributions(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError, std::vector<std::string>& Distributions)
		{
			if (Tokens[i]->TokenType != LexerToken::LBRACKET)
			{
				Distributions = {};
				return;
			}
			++i;
			//Advance adding string or ident tokens until we hit RBRACKET
			for (i; i < Tokens.size(); ++i)
			{
				if (Tokens[i]->TokenType == LexerToken::RBRACKET)
					break;
				if (Tokens[i]->TokenType == LexerToken::IDENT)
					Distributions.push_back(Tokens[i]->Value);
				else if (Tokens[i]->TokenType == LexerToken::STRING)
					Distributions.push_back(Tokens[i]->Value);
				
				//Tokens like COMMA will be ignored
			}
		}


		inline bool IsUnop(std::vector<Token*>& Tokens, size_t& i)
		{
			if (Tokens[i]->TokenType == LexerToken::NOT)
				return true;
			if (Tokens[i]->TokenType == LexerToken::IDENT)
			{
				//User-defined unary operations may not even be possible with the language design. TODO: implement them
				return false;
			}
			return false;
		}
		
		BoolNode* GetValue(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError)
		{
			if (Tokens[i]->TokenType == LexerToken::NOT)
			{
				//Check the identity of the next node
				++i;
				if (Tokens[i]->TokenType != LexerToken::IDENT)
				{
					//Throw an error
					Error(ERROR_044, Tokens[i]);
					SyntaxError = true;
					return nullptr;
				}
				//Get namespaces
				AccessNamespace Namespace = CheckNamespace(Tokens, i, SyntaxError);
				if (Tokens[i]->TokenType != LexerToken::IDENT)
				{
					//Throw an error
					Error(ERROR_044, Tokens[i]);
					SyntaxError = true;
					return nullptr;
				}
				return new N_Not(new N_Placeholder(Tokens[i]->Value, mv(Namespace)));
			}
			else if (Tokens[i]->TokenType == LexerToken::LPAREN)
			{
				++i;
				return ParseBoolExpression(Tokens, i, SyntaxError, nullptr);
			}
			else if (Tokens[i]->TokenType != LexerToken::IDENT)
			{
				ERROR(ERROR_045, Tokens[i]);
				SyntaxError = true;
				return nullptr;
			}
			else
			{
				//Get namespaces
				AccessNamespace Namespace = CheckNamespace(Tokens, i, SyntaxError);
				if (Tokens[i]->TokenType != LexerToken::IDENT)
				{
					//Throw an error
					Error(ERROR_045, Tokens[i]);
					SyntaxError = true;
					return nullptr;
				}
				return new N_Placeholder(Tokens[i]->Value, mv(Namespace));
			}
		}

		//Handles namespaces, expressions, parentheses, etc
		BoolNode* ParseBoolExpression(std::vector<Token*>& Tokens, size_t& i, bool& SyntaxError, BoolNode* LVALUE)
		{
			//Get RVALUE
			BoolNode* RVALUE = nullptr;

			//First, we need to see if this is a unary or binary operation. If it's a custom operator, things will be a little bit more 
			//difficult. Because of reasons detailed in https://twitter.com/CamelCaseCam1/status/1602469015473078272?s=20&t=ndZqNKAorpZJe4UAJNfcZw,
			//it's impossible to have both custom unary and binary operators. So we only support custom binary operators.

			//There are 6 possibilities
			//1. unop LVALUE
			//2. LVALUE binop RVALUE
			//3. unop LVALUE binop RVALUE
			//4. LVALUE binop unop RVALUE
			//5. unop LVALUE binop unop RVALUE
			//6. LVALUE
			
			if (LVALUE == nullptr)
			{
				LVALUE = GetValue(Tokens, i, SyntaxError);

				//Check if there's more terms in the expression
				++i;
			}

			if (i >= Tokens.size() || Tokens[i]->TokenType == LexerToken::RPAREN)
			{
				return LVALUE;
			}

			switch (Tokens[i]->TokenType)
			{
			case LexerToken::AND:
			{
				//AND stuff
				++i;
				RVALUE = GetValue(Tokens, i, SyntaxError);
				auto newlval = new N_And(LVALUE, RVALUE);
				
				//Recursively call this function to generate the bool graph
				++i;
				return ParseBoolExpression(Tokens, i, SyntaxError, newlval);
			}
			case LexerToken::OR:
			{
				//OR stuff
				++i;
				RVALUE = GetValue(Tokens, i, SyntaxError);
				auto newlval = new N_Or(LVALUE, RVALUE);

				//Recursively call this function to generate the bool graph
				++i;
				return ParseBoolExpression(Tokens, i, SyntaxError, newlval);
			}
			default:
				return LVALUE;
			}
			

			//switch (Tokens[i]->TokenType)
			//{
			//case LexerToken::BOOL_TRUE:
			//{
			//	//If there are any other nodes, we just simplify this node
			//	if (i + 1 < Tokens.size())
			//	{
			//		switch (Tokens[i + 1]->TokenType)
			//		{
			//		//AND and OR both evaluate to whatever the RVALUE is
			//		case LexerToken::AND:
			//		case LexerToken::OR:
			//		case LexerToken::LPAREN:
			//			i += 2;
			//			return ParseBoolExpression(Tokens, i);
			//		}
			//	}
			//	break;
			//}
			//case LexerToken::LPAREN:
			//{
			//	++i;
			//	LVALUE = ParseBoolExpression(Tokens, i);
			//	break;
			//}
			//case LexerToken::RPAREN:
			//{
			//	CDB_BuildError(ERROR_043, Tokens[i]->line);
			//	throw GILException();
			//}
			//case LexerToken::NOT:
			//{
			//	++i;
			//	BoolNode* RVALUE = ParseBoolExpression(Tokens, i);
			//	return new N_Not(RVALUE);
			//	break;
			//}
			//case LexerToken::IDENT:
			//{

			//}
			//}
		}





		
		void GetReusableElements(std::vector<Token*>& Tokens, Project* Target)    //Does all the actual parsing
		{
			auto nodes = ParseNodes(Tokens, Target); 
			Target->Main = mv(nodes);
			for (AST_Node* node : Target->Main)
			{
				//The node has an AddToProject function
				if (node->CanAddToProject())
				{
					ProjectNode* pnode = (ProjectNode*)node;
					pnode->AddToProject(Target);
				}
			}
		}


		

		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, int& i)
		{
			size_t s = i;
			return GetInsideTokens(Tokens, s);
		}

		std::vector<Token*> GetInsideTokens(std::vector<Token*>& Tokens, size_t& i)    //Gets all tokens enclosed by a pair of curly brackets
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
			
		}

		void GetTypeInfo(Project* Proj, Sequence* Seq, int& i, std::vector<Token*>& Tokens)
		{
			
		}

		
		//This function is deprecated and was used back when the compiler differentiated between sequences and operations. It will be removed in the future.
		void GetOperation(std::string Name, std::vector<Token*>& Tokens, std::vector<std::string>& CompletedSequences, std::vector<std::string>& CompletedOps, std::map<std::string,
			std::vector<Token*>>& Sequences, std::map<std::string, std::vector<Token*>>& Operations, Project* Target)
		{
			
		}

		void ConvertTokenToDNA(Token* t)
		{
			//Clean the string by shifting characters backwards
			int NumRem = 0;
			for (int i = 0; i < t->Value.length(); ++i)
			{
				switch (t->Value[i])
				{
				case '\r':
				case ' ':
				case '\n':
					++NumRem;
					break;
				default:
					//Shift the characters backwards
					t->Value[i - NumRem] = t->Value[i];
					break;
				}
			}
			t->Value.resize(t->Value.length() - NumRem);
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
				else if (t->TokenType == LexerToken::DNA)
				{
					ConvertTokenToDNA(t);
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

			//Write sequences to the file
			int Len = this->Sequences.size();
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
			for (AST_Node* node : this->Main)
			{
				AST_Node::SaveNode(node, OutputFile);
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
		Project* LoadCGIL5(std::ifstream& InputFile);

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
			case 4:
				CDB_BuildError("CGIL version {0} is not supported by this version of GIL", Version);
				throw GIL::GILException();
			case 5:
				return LoadCGIL5(InputFile);
			default:
				CDB_BuildError("Error reading CGIL version from path {0}", Path);
				throw GIL::GILException();
			}
		}

		//Some more func defs
		void GetNamespace(std::vector<std::string*>& Namespaces, std::vector<Lexer::Token*>* Tokens, int i);

		std::pair<Sequence*, Project*> Project::GetSeq(CallSequence* Seq, std::map<std::string, GILModule*>* Modules)
		{
			//Check if the sequence is in another namespace
			if (Seq->Location.size() != 0)
			{
				if (Modules->contains(Seq->Location[0]))
				{
					return { (*Modules)[Seq->Location[0]]->GetSequence(Seq->Name), nullptr };
				}
				return this->Namespaces[Seq->Location[0]]->GetSeqFromNamespace(Seq->Name, Seq->Location, 1, Modules);
			}
			return { Sequences[Seq->Name], this };
		}

		std::pair<Sequence*, Project*> Project::GetSeq(CallOperation* Seq, std::map<std::string, GILModule*>* Modules)
		{
			//Check if the sequence is in another namespace
			if (Seq->Location.size() != 0)
			{
				if (Modules->contains(Seq->Location[0]))
				{
					return { (*Modules)[Seq->Location[0]]->GetSequence(Seq->Name), nullptr };
				}
				return this->Namespaces[Seq->Location[0]]->GetSeqFromNamespace(Seq->Name, Seq->Location, 1, Modules);
			}
			return { Sequences[Seq->Name], this };
		}

		std::pair<Sequence*, Parser::Project*> Project::GetOperator(std::string& OperatorName, std::map<std::string, GILModule*>* Modules)
		{
			if (!Operators.contains(OperatorName))
				return { nullptr, this };
			Operator* op = (Operator*)Operators[OperatorName];
			return { op, op->Origin };
		}

		//Recursively traverse namespaces until you get to the one with the sequence
		std::pair<Sequence*, Project*> Project::GetSeqFromNamespace(std::string& SeqName, std::vector<std::string>& Namespaces, int i,
			std::map<std::string, GILModule*>* Modules)
		{
			if (i == 0 && Namespaces.size() == 1 && Modules->contains(Namespaces[0]))
			{
				return { (*Modules)[Namespaces[0]]->GetSequence(SeqName), nullptr };
			}
			if (i < Namespaces.size())
				return this->Namespaces[Namespaces[i]]->GetSeqFromNamespace(SeqName, Namespaces, i + 1, Modules);
			return { Sequences[SeqName], this };
		}

		Sequence* Project::GetOperatorFromNamespace(std::string& SeqName, std::vector<std::string*>& Namespaces, int i, std::map<std::string, GILModule*>* Modules)
		{
			if (i < Namespaces.size())
				return this->Namespaces[*Namespaces[i]]->GetOperatorFromNamespace(SeqName, Namespaces, i + 1, Modules);
			return Operators[SeqName];
		}

		Sequence* Project::GetOp(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules)
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
			return Sequences[OpName];
		}

		Sequence* Project::GetOpFromNamespace(std::string& OpName, std::vector<std::string*>& Namespaces, int i,
			std::map<std::string, GILModule*>* Modules)
		{
			if (i < Namespaces.size())
				return this->Namespaces[*Namespaces[i]]->GetOpFromNamespace(OpName, Namespaces, i + 1, Modules);
			return Sequences[OpName];
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

		Project* LoadCGIL5(std::ifstream& InputFile)
		{
			int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));
			InputFile.read((char*)&IntVal, sizeof(int));

			Project* Proj = new Project();
			LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

			int Len = -1;
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
				AST_Node* Node = AST_Node::LoadNode(InputFile, Proj);
				if (Node != nullptr)
					Proj->Main.push_back(Node);
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