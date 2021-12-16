#include <gilpch.h>
#include "Utils.h"

namespace GIL
{
	namespace utils
	{
		bool Matches(char c, char p);

		/*
		Function to find sequence matching an IUPAC nucleotide sequence. 

		IUPAC nucleotides:
		 - A
		 - T
		 - C
		 - G
		 - U: gets converted to T
		 - R: A or G
		 - Y: C or T
		 - S: G or C
		 - W: A or T
		 - K: G or T
		 - M: A or C
		 - B: C, G, or T
		 - D: A, G, or T
		 - H: A, C, or T
		 - V: A, C, or G
		 - N: A, C, G, or T
		*/
		std::string Find(std::string& Code, std::string pattern)
		{
			//Convert u to t
			for (char& c : Code)
			{
				if (c == 'u' || c == 'U')
				{
					c = 't';
				}
			}

			for (char& c : pattern)
			{
				if (c == 'u' || c == 'U')
				{
					c = 't';
				}
			}

			//Loop through code
			for (int i = 0; i < Code.length(); ++i)
			{
				for (int p = 0; p < pattern.length(); ++p)    //Loop through pattern
				{
					if (!Matches(Code[i + p], pattern[p]))
						goto ContinueLoop;
				}
				return Code.substr(i, pattern.length());

			ContinueLoop:
				continue;
			}
			return std::string();
		}

		bool Matches(char c, char p)
		{
			c = std::tolower(c);
			switch (std::tolower(p))
			{
				//Normal nucleotide codes
			case 'a':
				return c == 'a';
			case 't':
				return c == 't';
			case 'c':
				return c == 'c';
			case 'g':
				return c == 'g';

				//Ambiguous nucleotide codes
			case 'r':
				return c == 'a' || c == 'g';
			case 'y':
				return c == 'c' || c == 't';
			case 's':
				return c == 'g' || c == 'c';
			case 'w':
				return c == 'a' || c == 't';
			case 'k':
				return c == 'g' || c == 't';
			case 'm':
				return c == 'a' || c == 'c';

				//Three-posibility nucleotide codes
			case 'b':
				return c != 'a';
			case 'd':
				return c != 'c';
			case 'h':
				return c != 'g';
			case 'v':
				return c != 't';

			case 'n':
				return true;
			default:
				CDB_BuildError("Unsupported character '{0}' in pattern", p);
				return false;
			}
		}
	}
}