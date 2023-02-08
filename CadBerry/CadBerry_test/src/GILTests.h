#ifndef SaveAndLoad
#define SaveAndLoad(project)

#include "catch_test_macros.hpp"
#include "CadBerry.h"
#include "CadBerry/Platform/Headless/HeadlessInput.h"

#include "GIL.h"
#include "GIL/Lexer/Lexer.h"

#include <cstdio>

#include "GILExamples.h"
#endif

SECTION("Setting target organism")
{
	auto tokens = GIL::Lexer::Tokenize(TargetExample1);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	GIL::Compiler::Compile(project.raw());
	CHECK(project->TargetOrganism == "S.Cerevisiae");

	tokens = GIL::Lexer::Tokenize(TargetExample2);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "E.coli");

	tokens = GIL::Lexer::Tokenize(TargetExample3);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Escherichia coli");

	tokens = GIL::Lexer::Tokenize(TargetExample4);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "human");

	tokens = GIL::Lexer::Tokenize(TargetExample5);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Pichia");

	tokens = GIL::Lexer::Tokenize(TargetExample6);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Pichia Pastoris");

	tokens = GIL::Lexer::Tokenize(TargetExample7);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Saccharomyces cerevisiae");

	tokens = GIL::Lexer::Tokenize(TargetExample8);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Unoptimized");

	tokens = GIL::Lexer::Tokenize(TargetExample9);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->TargetOrganism == "Yeast");
}

SECTION("Preprocessor commands")
{
	//Regions
	auto tokens = GIL::Lexer::Tokenize(RegionExample);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.first.size() == 2);

	CHECK(Output.first[0].Name == "Region2");
	CHECK(Output.first[1].Name == "Region1");

	CHECK(Output.first[1].Start == 0);
	CHECK(Output.first[1].End == 12);
	CHECK(Output.first[0].Start == 6);
	CHECK(Output.first[0].End == 12);



	tokens = GIL::Lexer::Tokenize(OverlappingRegionExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.first.size() == 2);

	CHECK(Output.first[0].Name == "Region1");
	CHECK(Output.first[1].Name == "Region2");

	CHECK(Output.first[0].Start == 0);
	CHECK(Output.first[0].End == 12);
	CHECK(Output.first[1].Start == 6);
	CHECK(Output.first[1].End == 18);


	//SetAttr
	tokens = GIL::Lexer::Tokenize(SetAttrExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());

	REQUIRE(project->Attributes.size() > 0);

	CHECK(project->Attributes["a normal string"] == "another completely normal string");
	CHECK(project->Attributes["A More Complicated STRING #%$##@$%RWETRBFIKUOI::P}{(*"] == "~~~~~~~~~#%$#^$%^&YHGF::");


	//#Var
	tokens = GIL::Lexer::Tokenize(CreateVarExample1);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 1);
	CHECK(project->NumVars.size() == 1);

	CHECK(project->StrVars["Var1"] == "SomeString");
	CHECK(project->NumVars["Var2"] == 10);


	tokens = GIL::Lexer::Tokenize(CreateVarExample2);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 1);
	CHECK(project->NumVars.size() == 1);

	CHECK(project->StrVars["SSDGSFDV  SDFSEFDCSR##@23or3fnreiuh34"] == "TestValue");
	CHECK(project->NumVars["THIS IS A NUMBER23"] == 3892302332.6);


	//#Inc
	tokens = GIL::Lexer::Tokenize(IncExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 0);
	CHECK(project->NumVars.size() == 1);

	CHECK(project->NumVars["TestVar"] == 22234.0);


	//#Dec
	tokens = GIL::Lexer::Tokenize(DecExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 0);
	CHECK(project->NumVars.size() == 1);

	CHECK(project->NumVars["TestVar"] == 22232.0);


	//#If
	tokens = GIL::Lexer::Tokenize(PreproIfExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 0);
	CHECK(project->NumVars.size() == 3);

	CHECK(project->NumVars["Counter"] == 1.0);


	//#Else
	tokens = GIL::Lexer::Tokenize(PreproElseExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(project->StrVars.size() == 0);
	CHECK(project->NumVars.size() == 3);

	CHECK(project->NumVars["Counter"] == 1.0);
}

SECTION("DNA and amino acid literals")
{
	auto tokens = GIL::Lexer::Tokenize(DNAExample1);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "AAATTTCCCGGG");

	tokens = GIL::Lexer::Tokenize(DNAExample2);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "DSSDGFSSEVREFEWADSFVTERSD");

	tokens = GIL::Lexer::Tokenize(DNAExample3);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "@#@#454356w5r23$#$@453425");


	tokens = GIL::Lexer::Tokenize(AminoSequenceExample1);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	for (auto& c : Output.second) c = std::toupper(c);
	REQUIRE(Output.second == "GGTATTCTTTAAGCTGTTATGCCTTTTTGGAGTACTAATCAATATTGTAAACGTCATGATGAA");

	tokens = GIL::Lexer::Tokenize(AminoSequenceExample2);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	for (auto& c : Output.second) c = std::toupper(c);
	REQUIRE(Output.second == "GGTATTTTGTAAGCTGTTATGCCATTTTGGTCTACTAATCAATATTGTAAAAGACATGATGAA");
}

SECTION("Sequences and operations")
{
	auto tokens = GIL::Lexer::Tokenize(SequenceNoParams);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "CCCAAATTTGGG");

	tokens = GIL::Lexer::Tokenize(MultiSequenceNoParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "AAATTTCCC");

	tokens = GIL::Lexer::Tokenize(SequenceWithParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTAAACCC");

	tokens = GIL::Lexer::Tokenize(SequenceWithParamsWithParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTAAAAAACCC");

	//Typed params 
	tokens = GIL::Lexer::Tokenize(SequenceWithTypedParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTAAACCC");

	tokens = GIL::Lexer::Tokenize(SequenceWithTypedParamsWithParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTAAAAAACCC");

	//Multiple params
	tokens = GIL::Lexer::Tokenize(SequenceWithMultipleParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "AAATTT");

	tokens = GIL::Lexer::Tokenize(CustomType);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->Types[project->TypeName2Idx["TYPE"]] != nullptr);

	tokens = GIL::Lexer::Tokenize(CustomTypeInheritance);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	CHECK(project->Types[project->TypeName2Idx["TYPE"]]->IsOfType(project->Types[project->TypeName2Idx["cds"]], false));


	//Operations
	tokens = GIL::Lexer::Tokenize(OpNoParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTCCCTTT");

	tokens = GIL::Lexer::Tokenize(SequenceWithInnerCode);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTCCCTTT");

	tokens = GIL::Lexer::Tokenize(OpSequenceParams);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTCCCGGGTTT");
}

SECTION("Forwards")
{
	auto tokens = GIL::Lexer::Tokenize(SequenceForward);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "AAAAA");

	tokens = GIL::Lexer::Tokenize(SequenceForwardWithParams);
	project = GIL::Parser::Project::Parse(*tokens);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "AAAAATTT");
}

SECTION("For and from")
{
	auto tokens = GIL::Lexer::Tokenize(ForExample);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	for (auto& c : Output.second) c = std::toupper(c);
	REQUIRE(Output.second == "GGTATTTTGTAAGCTGTTATGCCATTTTGGTCTACTAATCAATATTGTAAAAGACATGATGAA");
}

SECTION("Operators")
{
	auto tokens = GIL::Lexer::Tokenize(UnaryOperatorExample);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "TTTAAAAATTTAAAAA");

	tokens = GIL::Lexer::Tokenize(BinaryOperatorExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "TTTAAA");
}

SECTION("Distributions")
{
	auto tokens = GIL::Lexer::Tokenize(DistributionExample);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	
	//Should compile to nothing
	auto Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "");

	//Should compile to TTT
	std::string Distribution = "debug";
	Output = GIL::Compiler::Compile(project.raw(), nullptr, &Distribution);
	REQUIRE(Output.second == "TTT");

	Distribution = "Rel_full";
	Output = GIL::Compiler::Compile(project.raw(), nullptr, &Distribution);
	REQUIRE(Output.second == "TTT");
}

SECTION("Namespaces")
{
	auto tokens = GIL::Lexer::Tokenize(SingleNamespace);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "TTT");

	tokens = GIL::Lexer::Tokenize(MultipleNamespaces);
	project = GIL::Parser::Project::Parse(*tokens);
	Output = GIL::Compiler::Compile(project.raw());
	REQUIRE(Output.second == "TTT");

	tokens = GIL::Lexer::Tokenize(NamespaceSequenceAsParameter);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());
	CHECK(Output.second == "TTTAAACCC");
}

SECTION("Bools")
{
	auto tokens = GIL::Lexer::Tokenize(BoolImplementationExample);
	CDB::scoped_ptr<GIL::Parser::Project> project = GIL::Parser::Project::Parse(*tokens);
	//SaveAndLoad(project);
	REQUIRE(project->BoolImplementations.size() == 1);
	REQUIRE(project->BoolImplementations[0]->name == "TestImpl");

	//Make sure all the sequences are defined
	auto impl = project->BoolImplementations[0];
	CHECK(impl->Alloc != nullptr);
	CHECK(impl->Dealloc != nullptr);
	CHECK(impl->Set != nullptr);
	CHECK(impl->Use != nullptr);
	CHECK(impl->NumAvailable != nullptr);

	CHECK(impl->And != nullptr);
	CHECK(impl->Or != nullptr);
	CHECK(impl->Not != nullptr);

	//Make sure extra sequences aren't defined
	CHECK((!impl->ChainedAndAvailable && impl->ChainedAnd == nullptr));
	CHECK((!impl->ChainedOrAvailable && impl->ChainedOr == nullptr));

	CHECK((!impl->NandAvailable && impl->Nand == nullptr));
	CHECK((!impl->NorAvailable && impl->Nor == nullptr));
	CHECK((!impl->XnorAvailable && impl->Xnor == nullptr));
	CHECK((!impl->XorAvailable && impl->Xor == nullptr));

	CHECK(impl->AdditionalSequences.size() == 0);


	tokens = GIL::Lexer::Tokenize(BoolExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	auto Output = GIL::Compiler::Compile(project.raw());

	REQUIRE(Output.second == "GGGGAAAAGGGGGGGGAAAAGGGGGGGGCCCCGGGGAAAAAAAAAAAATTTTAAAATTTTGGGGAAAAGGGGCCCCAAAACCCCGGGGAAAAGGGG");

	tokens = GIL::Lexer::Tokenize(SequenceBoolExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());

	REQUIRE(Output.second == "GGGGAAAAGGGGTTTTAAATTTTGGGGAAAAGGGGTTTTAAATTTTTTTAAAAGGGAAAA");

	tokens = GIL::Lexer::Tokenize(SequenceParamBoolExample);
	project = GIL::Parser::Project::Parse(*tokens);
	SaveAndLoad(project);
	Output = GIL::Compiler::Compile(project.raw());

	REQUIRE(Output.second == "GGGGAAAAGGGGTTTTAAATTTTGGGGAAAAGGGGTTTTAAATTTTTTTAAAAGGGAAAATTTAAAAGGGAAAA");
}