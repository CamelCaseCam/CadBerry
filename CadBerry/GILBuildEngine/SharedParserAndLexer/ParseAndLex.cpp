#include <gilpch.h>
#include "ParseAndLex.h"
#include "AST_Node.h"

#include "lug/lug.hpp"

#include "GIL/Lexer/Token.h"
#include "GIL/Parser/Project.h"
#include "GIL/GILException.h"
#include <iostream>

namespace GIL
{
	using namespace lug::language;
	using namespace Lexer;
	using namespace Parser;
	namespace Parser
	{
		extern rule STATEMENT;
	}
	
	//The lexer

#define Return(Val) [&]{return Val;}
#define KeywordMatch(Keyword, token) rule token = lexeme[#Keyword] < [&]{return Token::##token;}

//Access lug variable of pointer
#define APV(Pointer, Value) (*Pointer)->Value
#define MPV(Pointer, Value) std::move((*Pointer)->Value)

#define UpperLowerPreproToken(Name, Upper, Lower) rule Name = lexeme[#Upper##_sx | #Lower]

#define BindNodes(rule) *(_node % rule < [&] { _nodes->push_back(*_node); })

	lug::grammar grammar_;
	thread_local lug::environment environment_;
	thread_local lug::variable<std::string> _name{ environment_ };
	thread_local lug::variable<Token*> _tok1{ environment_ }, _tok2{ environment_ }, _tok3{ environment_ };
	thread_local lug::variable<std::vector<Token*>> _tokens{ environment_ };
	thread_local lug::variable<std::vector<ParamNode*>> _params{ environment_ };

	thread_local lug::variable<AST_Node*> _node{ environment_ };
	thread_local lug::variable<Call_Params*> _params_node{ environment_ };
	thread_local lug::variable<AccessNamespace*> _namespace_node{ environment_ };
	thread_local lug::variable<std::vector<AST_Node*>> _nodes{ environment_ };
	thread_local lug::variable<Params> _sequence_params{ environment_ };
	thread_local lug::parser* CurrentParser = nullptr;

	
	namespace Lexer
	{
		rule BEGIN = lexeme["}"] < Return(Token::Begin);
		rule END = lexeme["{"] < Return(Token::End);
		rule COMMA = lexeme[","] < Return(Token::Comma);
		rule LPAREN = lexeme["("] < Return(Token::LParen);
		rule RPAREN = lexeme[")"] < Return(Token::RParen);
		
		rule IDENT = lexeme[capture(_name)[*"[a-zA-Z0-9!@#%^&*_+=;|<>?~-]"_rx]] < [&] { return new Token(LexerToken::IDENT, *_name); };
		
		KeywordMatch(sequence, DefineSequence);
		KeywordMatch(operation, DefOp);
		KeywordMatch(import, Import);
		KeywordMatch(if, IF);
		KeywordMatch(using, Using);
		KeywordMatch(from, From);
		KeywordMatch(for, For);
		KeywordMatch(namespace, CreateNamespace);
		KeywordMatch(true, True);
		KeywordMatch(false, False);
		KeywordMatch(typedef, Typedef);
		KeywordMatch(inherits, Inherits);
		KeywordMatch(operator, Operator);
		
		//TODO: create tokens for this
		UpperLowerPreproToken(SETTARGET, SetTarget, settarget)		< [&] {return new Token(LexerToken::SETTARGET, ""); };
		UpperLowerPreproToken(SETATTR, setAttr, SetAttr)			< [&] {return new Token(LexerToken::SETATTR, ""); };
		UpperLowerPreproToken(SETVAR, var, Var)						< [&] {return new Token(LexerToken::CREATEVAR, ""); };
		UpperLowerPreproToken(OPTIMIZE, optimize, Optimize)			< [&] {return new Token(LexerToken::OPTIMIZE, ""); };
		UpperLowerPreproToken(BEGINREGION, region, Region)			< [&] {return new Token(LexerToken::BEGINREGION, ""); };
		UpperLowerPreproToken(ENDREGION, endRegion, EndRegion)		< [&] {return new Token(LexerToken::ENDREGION, ""); };
		UpperLowerPreproToken(CREATEVAR, var, Var)					< [&] {return new Token(LexerToken::CREATEVAR, ""); };
		UpperLowerPreproToken(INC, inc, Inc)						< [&] {return new Token(LexerToken::INC, ""); };
		UpperLowerPreproToken(DEC, dec, Dec)						< [&] {return new Token(LexerToken::DEC, ""); };
		UpperLowerPreproToken(PREPRO_IF, if, If)					< [&] {return new Token(LexerToken::PREPRO_IF, ""); };
		UpperLowerPreproToken(PREPRO_ELSE, else, Else)				< [&] {return new Token(LexerToken::PREPRO_ELSE, ""); };
		UpperLowerPreproToken(PREPRO_ENDIF, endif, EndIf)			< [&] {return new Token(LexerToken::PREPRO_ENDIF, ""); };
		
		rule PARAM = lexeme["$"_sx > capture(_name)["[a-zA-Z0-9!@#%^&*_+=;|<>?~-]*"_rx]] < [&] { return new Token(LexerToken::PARAM, *_name); };
		rule AMINO_LITERAL = lexeme["@"_sx > capture(_name)["[\\s\\S]*"_rx] > "@"] < [&] { return new Token(LexerToken::AMINOS, *_name); };
		rule COMMENT = lexeme[("//"_sx > capture(_name)["[^\\n\\r]*"_rx]) | ("/*"_sx > capture(_name)["[^\\*/]*"_rx] > "*/")] < [&] { return new Token(LexerToken::COMMENT, *_name); };
		rule CALLOP = lexeme["."_sx > capture(_name)["[a-zA-Z0-9!@#%^&*_+=;|<>?~-]*"_rx]] < [&] { return new Token(LexerToken::CALLOP, *_name); };
		rule FORWARD = lexeme["=>"] > [&] { return new Token(LexerToken::FORWARD, ""); };
		rule STRING_LITERAL = lexeme["\""_sx > capture(_name)["[\\s\\S]*_rx"] > "\""] < [&] { return new Token(LexerToken::STRING, *_name); };
		rule DNA_LITERAL = lexeme["'"_sx > capture(_name)["[\\S]*"_rx] > "'"] < [&] { return new Token(LexerToken::STRING, *_name); };
		
		rule ACCESS_NAMESPACE = lexeme["::"] < [&] { return new Token(LexerToken::NAMESPACE, ""); };
		rule ASSIGN_TYPE = lexeme[":"] < [&] { return new Token(LexerToken::ASSIGNTYPE, ""); };
	}
	
	
	//The parser
	namespace Parser
	{
		ParserPosition::ParserPosition(lug::syntax_position pos)
		{
			this->Line = pos.line;
			this->Column = pos.column;
		}
		
		
		//Parser parameter type that incorperates optional type information
		rule P_PARAM = _tok1 % PARAM > ASSIGN_TYPE > _tok2 % IDENT > ~COMMA < [&] { return new ParamNode(std::move(APV(_tok1, Value)), std::move(APV(_tok2, Value))); }
			| _tok1 % PARAM > ~COMMA < [&] { return new ParamNode(std::move(APV(_tok1, Value))); };
		// (Param 1, param 2, param 3)
		rule PARAM_DEFINITION_GROUP = LPAREN > _params % +P_PARAM > RPAREN < [&] { return new Params(std::move(*_params)); };

		//Import file
		rule IMPORT = Lexer::Import > _tok1 % IDENT < [&] { return new GIL::Parser::Import(std::move(APV(_tok1, Value))); }
			| Lexer::Import > _tok1 % STRING_LITERAL < [&] { return new GIL::Parser::Import(std::move(APV(_tok1, Value))); };

		//Link to dynamic library using the using keyword
		rule Using_Link = Lexer::Using > _tok1 % IDENT < [&] { return new GIL::Parser::Using(std::move(APV(_tok1, Value))); }
			| Lexer::Using > _tok1 % STRING_LITERAL < [&] { return new GIL::Parser::Using(std::move(APV(_tok1, Value))); };


		//From and For
		rule FROM = Lexer::From > _tok1 % IDENT > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::From(std::move(APV(_tok1, Value)), std::move(*_nodes)); }
			| Lexer::From > _tok1 % STRING_LITERAL > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::From(std::move(APV(_tok1, Value)), std::move(*_nodes)); };

		rule FOR = Lexer::From > _tok1 % IDENT > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::For(std::move(APV(_tok1, Value)), std::move(*_nodes)); }
			| Lexer::From > _tok1 % STRING_LITERAL > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::For(std::move(APV(_tok1, Value)), std::move(*_nodes)); };
		


		//Defining sequences and operations


		//Format sequence name(params) : type {...}
		rule DEFINE_SEQUENCE = Lexer::DefineSequence > _tok1 % IDENT > _sequence_params % PARAM_DEFINITION_GROUP > ASSIGN_TYPE 
			> _tok2 % IDENT > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::DefineSequence(std::move(APV(_tok1, Value)),
				std::move(*_sequence_params), std::move(APV(_tok2, Value)), std::move(*_nodes)); }
			//sequence name(params) {...}
			| Lexer::DefineSequence > _tok1 % IDENT > _sequence_params % PARAM_DEFINITION_GROUP > BEGIN > BindNodes(STATEMENT) > END
				<[&] { return new GIL::Parser::DefineSequence(std::move(APV(_tok1, Value)),	std::move(*_sequence_params), "", std::move(*_nodes)); }
			//sequence name : type {...}
			| Lexer::DefineSequence > _tok1 % IDENT > ASSIGN_TYPE > _tok2 % IDENT > BEGIN > BindNodes(STATEMENT) > END
				< [&] { return new GIL::Parser::DefineSequence(std::move(APV(_tok1, Value)), {}, std::move(APV(_tok2, Value)), std::move(*_nodes)); }
			//sequence name {...}
			| Lexer::DefineSequence > _tok1 % IDENT > BEGIN > BindNodes(STATEMENT) > END
			< [&] { return new GIL::Parser::DefineSequence(std::move(APV(_tok1, Value)), {}, "", std::move(*_nodes)); };

		//Format operation name(params) : type {...}
		rule DEFINE_OPERATION = Lexer::DefOp > _tok1 % IDENT > _sequence_params % PARAM_DEFINITION_GROUP > ASSIGN_TYPE
			> _tok2 % IDENT > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::DefineOperation(std::move(APV(_tok1, Value)),
				std::move(*_sequence_params), std::move(APV(_tok2, Value)), std::move(*_nodes)); }
			//operation name(params) {...}
			| Lexer::DefOp > _tok1 % IDENT > _sequence_params % PARAM_DEFINITION_GROUP > BEGIN > BindNodes(STATEMENT) > END
			< [&] { return new GIL::Parser::DefineOperation(std::move(APV(_tok1, Value)), std::move(*_sequence_params), "", std::move(*_nodes)); }
			//operation name : type {...}
			| Lexer::DefOp > _tok1 % IDENT > ASSIGN_TYPE > _tok2 % IDENT > BEGIN > BindNodes(STATEMENT) > END
			< [&] { return new GIL::Parser::DefineOperation(std::move(APV(_tok1, Value)), {}, std::move(APV(_tok1, Value)), std::move(*_nodes)); }
			//operation name {...}
			| Lexer::DefOp > _tok1 % IDENT > BEGIN > BindNodes(STATEMENT) > END
			< [&] { return new GIL::Parser::DefineOperation(std::move(APV(_tok1, Value)), {}, "", std::move(*_nodes)); };
		

		//Forwards
		rule P_Forward = Lexer::Operator > _tok1 % IDENT > FORWARD > _tok2 % IDENT < [&] { return new GIL::Parser::DefineOperator(MPV(_tok1, Value), MPV(_tok1, Value)); }
			| _tok1 % IDENT > FORWARD > _tok2 % IDENT < [&] { return new GIL::Parser::Forward(MPV(_tok1, Value), MPV(_tok2, Value)); };


		//Calling sequences
		rule CALL_PARAM = _tok1 % IDENT > ~COMMA < Return(*_tok1);
		rule CALL_PARAMS = LPAREN > _tokens % *CALL_PARAM > RPAREN < [&] { return new GIL::Parser::Call_Params(std::move(*_tokens)); };
		
		rule NAMESPACE_ACCESSION = _tok1 % IDENT > ACCESS_NAMESPACE < Return(*_tok1);
		
		rule P_ACCESS_NAMESPACE = _tokens % *NAMESPACE_ACCESSION < [&] { return new GIL::Parser::AccessNamespace(std::move(*_tokens)); };
		
		rule P_IDENT = _namespace_node % P_ACCESS_NAMESPACE > _tok1 % IDENT > _params_node % CALL_PARAMS 
				< [&] { return new GIL::Parser::CallSequence(MPV(_tok1, Value), std::move(**_params_node), std::move(**_namespace_node)); }
			| _namespace_node % P_ACCESS_NAMESPACE > _tok1 % IDENT < [&] { return new GIL::Parser::CallSequence(MPV(_tok1, Value), {}, std::move(**_namespace_node)); }
			
			| _tok1 % IDENT > _params_node % CALL_PARAMS < [&] { return new GIL::Parser::CallSequence(MPV(_tok1, Value), std::move(**_params_node), {}); }
			| _tok1 % IDENT < [&] { return new GIL::Parser::CallSequence(MPV(_tok1, Value), {}, {}); };

		//Calling operations
		rule P_CALLOP = _namespace_node % P_ACCESS_NAMESPACE > _tok1 % CALLOP > _params_node % CALL_PARAMS > BEGIN > BindNodes(STATEMENT) > END
				< [&] { return new GIL::Parser::CallOperation(MPV(_tok1, Value), std::move(**_params_node), std::move(*_nodes), 
					std::move(**_namespace_node)); }
			| _namespace_node % P_ACCESS_NAMESPACE > _tok1 % CALLOP > BEGIN > BindNodes(STATEMENT) > END
				< [&] { return new GIL::Parser::CallOperation(MPV(_tok1, Value), {}, std::move(*_nodes), std::move(**_namespace_node)); }
			
			| _tok1 % CALLOP > _params_node % CALL_PARAMS > BEGIN > BindNodes(STATEMENT) > END
				< [&] { return new GIL::Parser::CallOperation(MPV(_tok1, Value), std::move(**_params_node), std::move(*_nodes), {}); }
			| _tok1 % CALLOP > BEGIN > BindNodes(STATEMENT) > END < [&] { return new GIL::Parser::CallOperation(MPV(_tok1, Value), {}, std::move(*_nodes), {}); };


		//Preprocessor directives
		rule P_SetTarget = SETTARGET > _tok1 % IDENT < [&] { return new GIL::Parser::SetTarget(MPV(_tok1, Value)); }
			| SETTARGET > _tok1 % STRING_LITERAL < [&] { return new GIL::Parser::SetTarget(MPV(_tok1, Value)); };
		
		//Format #Region RegionName
		rule P_BeginRegion = BEGINREGION > _tok1 % IDENT < [&] { return new GIL::Parser::BeginRegion(MPV(_tok1, Value)); }
			| BEGINREGION > _tok1 % STRING_LITERAL < [&] { return new GIL::Parser::BeginRegion(MPV(_tok1, Value)); };
		
		//Format #EndRegion [RegionName]
		rule P_EndRegion = ENDREGION > _tok1 % IDENT < [&] { return new GIL::Parser::EndRegion(MPV(_tok1, Value)); }
			| ENDREGION > _tok1 % STRING_LITERAL < [&] { return new GIL::Parser::EndRegion(MPV(_tok1, Value)); }
			| ENDREGION < [&] { return new GIL::Parser::EndRegion(); };
		
		//Format #SetAttr Key Value
		rule P_SetAttr = SETATTR > _tok1 % STRING_LITERAL > _tok2 % STRING_LITERAL < [&] { return new GIL::Parser::SetAttr(MPV(_tok1, Value), MPV(_tok2, Value)); };
		
		//Format #Var type name [value]
		rule P_SetVar = SETVAR > _tok1 % IDENT > _tok2 % IDENT > _tok3 % IDENT < [&] { return new GIL::Parser::SetVar(APV(_tok1, Value), MPV(_tok2, Value), MPV(_tok3, Value)); }
			| SETVAR > _tok1 % IDENT > _tok2 % IDENT > _tok3 % STRING_LITERAL < [&] { return new GIL::Parser::SetVar(APV(_tok1, Value), MPV(_tok2, Value), ""); };

		rule P_IncVar = INC > _tok1 % IDENT < [&] { return new GIL::Parser::IncVar(MPV(_tok1, Value)); };
		rule P_DecVar = INC > _tok1 % IDENT < [&] { return new GIL::Parser::DecVar(MPV(_tok1, Value)); };
		
		/*#if - format is #if function(...)
		...
		#Else or #EndIf
		*/
		rule P_Prepro_If = PREPRO_IF > _tok1 % IDENT > LPAREN > _tokens % *IDENT > RPAREN
			> _nodes % *STATEMENT > &(PREPRO_ELSE | PREPRO_ENDIF) < [&] { return new GIL::Parser::Prepro_If(MPV(_tok1, Value), std::move(*_tokens), std::move(*_nodes)); };

		rule P_Prepro_Else = PREPRO_ELSE > BindNodes(STATEMENT) > PREPRO_ENDIF < [&] { return new GIL::Parser::Prepro_Else(std::move(*_nodes)); };
		

		//DNA and amino acid literals
		rule P_DNALiteral = DNA_LITERAL < [&] { return new GIL::Parser::DNALiteral(MPV(_tok1, Value)); };
		rule P_AminoAcidLiteral = AMINO_LITERAL < [&] { return new GIL::Parser::AminoAcidLiteral(MPV(_tok1, Value)); };
		
		
		//Typedefs
		//Format typedef Name [inherits parent]
		rule P_Typedef = Typedef > _tok1 % IDENT > Inherits > _tok2 % IDENT < [&] { return new GIL::Parser::TypedefNode(MPV(_tok1, Value), MPV(_tok2, Value)); }
			| Typedef > _tok1 % IDENT < [&] { return new GIL::Parser::TypedefNode(MPV(_tok1, Value), ""); };

		//Format Name inherits parent
		rule Inherits = _tok1 % IDENT > Inherits > _tok2 % IDENT < [&] { return new GIL::Parser::InheritTypedef(MPV(_tok1, Value), MPV(_tok2, Value)); };
		
		rule P_Comment = COMMENT < [&] { return new GIL::Parser::Comment(); };



		//For calling params in sequence
		rule Use_Param = _tok1 % PARAM > _params_node % CALL_PARAMS < [&] { return new GIL::Parser::UseParam(MPV(_tok1, Value), std::move(**_params_node)); }
			| _tok1 % PARAM < [&] { return new GIL::Parser::UseParam(MPV(_tok1, Value), {}); };
		
		//Statement has to be defined in chunks, otherwise the compiler runs out of heap space
		auto Chunk1 = _node % IMPORT | _node % Using_Link /*| _node % FROM | _node % FOR*/ 
			/* | _node % DEFINE_SEQUENCE | _node % DEFINE_OPERATION*/ | _node % P_Forward;
		
		auto Chunk2 = _node % P_IDENT | /*_node % P_CALLOP |*/ _node % P_SetTarget | _node % P_BeginRegion | _node % P_EndRegion | _node % P_SetAttr
			| _node % P_SetVar | _node % P_IncVar | _node % P_DecVar;
		
		auto Chunk3 = _node % P_DNALiteral | _node % P_AminoAcidLiteral | _node % P_Typedef | _node % Inherits | _node % P_Comment
			| _node % Use_Param /*_node % P_Prepro_If | _node % P_Prepro_Else*/;

		rule STATEMENT = Chunk1 | Chunk2 | Chunk3 < [&] {(*_node)->SetPos(CurrentParser->subject_position()); return *_node; };
		

		thread_local std::vector<AST_Node*>* OutputResult = nullptr;
		rule STATEMENTS = BindNodes(STATEMENT) < [&]
		{  
			if (OutputResult == nullptr)
			{
				CDB_BuildError("No parser output set. Aborting compilation");
				throw GILException();
			}
			
			*OutputResult = std::move(*_nodes);
		};
		
		bool GrammarInitialized = false;
		void LoadGrammar()
		{
			if (!GrammarInitialized)
			{
				grammar_ = start(Parser::STATEMENTS);
				GrammarInitialized = true;
			}	
		}

		Project* Project::TokenizeAndParse(std::string Code)
		{
			LoadGrammar();
			Project* CurrentProject = new Project();
			
			lug::parser parser{ grammar_, environment_ };
			CurrentParser = &parser;
			OutputResult = &CurrentProject->Main;
			
			bool result = parser.parse();
			
			//Add nodes to project
			for (AST_Node* node : CurrentProject->Main)
			{
				//The node has an AddToProject function
				if (node->CanAddToProject())
				{
					ProjectNode* pnode = (ProjectNode*)node;
					pnode->AddToProject(CurrentProject);
				}
			}
			
			return CurrentProject;
		}
	}
}