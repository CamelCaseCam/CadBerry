#pragma once
#include <string>


/*
IF I write my own parser for GIL, it needs to support the following types of rules:
 * Token-based rules. Returns true if the tokens at the index match
 * Wildcard rules. Always skips this rule
 * Repeats. Repeats the rule until you reach the end of the pattern

The program should be structured as a list of nodes. Token-based rules will take in a node and return true if the node types match

*/

namespace GIL
{
	namespace Parser
	{
		template<typename NodeValueType>
		struct Node
		{
			Node(std::string id) : m_ID(id) {}

			std::string m_ID;
			NodeValueType m_Value;
		};

		enum class RuleTermType
		{
			NodeMatch,
			WildCard,
			Repeat,
		};

		enum class RepeatType
		{
			Lazy,
			Greedy,
		};


		constexpr RuleTerm NodeMatch();

		template<typename T>
		struct RuleTerm
		{
			
		};
	}
}