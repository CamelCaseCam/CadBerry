#pragma once

#ifdef CDB_DEBUG    //This will only be compiled in debug CadBerry releases
#include <gilpch.h>

namespace GIL
{
	namespace Examples
	{
		std::string SequenceForwardExample = 
R"(#Target S.Cerevisiae
operation TestOp
{
	AAAAATTTTTCCCCGGGGG
	$InnerCode
	@MMCTQQQP
	LX@
}

sequence TestSeq
{
	TTTTTTTTTTTTTTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCC
}

TestFSeq => TestSeq

.Begin
{
	.TestOp
	{
		TestFSeq
	}
}
)";

		std::string SequenceExample =
R"(#Target S.Cerevisiae
sequence TestNestedSequence
{
	@AAAALLLLLQQQQQQX@
}

sequence TestSeq
{
	ATTTTTTC
	TestNestedSequence
	ATTTTTTC
}

TestSeq
)";

		std::string PreprocessorErrorExample = 
R"(#Traget Something
#GGSAGsnewmdmsmaa
#SJKANANDndkaaaa
#amsmsmsmsfanksndjnssdsns
#ssfn$nskdkfn
)";

		std::string GotoExample = 
R"(#Target Yeast
@aaaaaaaaaaa@
)";

		std::string RegionAminosExample = 
R"(#Target S.Cerevisiae

#Region TestRegion
	ATTTTTTTCC
	@QQQQAAAALLLLLX@
	ATTTTTTTCC
#EndRegion
)";

		std::string OpExample = 
R"(#Target S.Cerevisiae
operation TestOp
{
	AAAAAAAAAAAAAAAAAAAAAAAAAAAA
	$InnerCode
	AAAAAAAAAAAAAAAAAAAAAAAAAAAA
}

.TestOp
{
	TTT
}
)";

		std::string BoolExample = 
R"(#Target S.Cerevisiae
bool b1 = true | false
bool b2 = b1 | false & b1 | !b1)";

		std::string BoolExample2 = 
R"(#Target S.Cerevisiae
bool b1 = true & (false | true))";

		std::string BoolExample3 =
R"(#Target S.Cerevisiae
bool b1 => SomePromoter
bool B = true | b1

sequence s
{
	ATTTTTTT
	@AQX@
}

operation o
{
	if B
	{
		s
		$InnerCode
	}
}

.o
{
	CCCCC
}
)";
	}
}
#endif