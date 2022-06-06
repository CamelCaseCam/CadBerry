#include <gilpch.h>
#include "Utils.h"
#include "CadBerry/Application.h"
#include "GIL/Parser/Project.h"

namespace GIL
{
	namespace utils
	{
		const std::unordered_map<char, char> Complements = std::unordered_map<char, char>({
			{'a', 't'},
			{'A', 'T'},
			{'t', 'a'},
			{'T', 'A'},
			{'c', 'g'},
			{'C', 'G'},
			{'g', 'c'},
			{'G', 'C'},
			{'u', 'a'},
			{'U', 'A'},
			{'m', 'k'},
			{'M', 'K'},
			{'r', 'y'},
			{'R', 'Y'},
			{'w', 'w'},
			{'W', 'W'},
			{'s', 's'},
			{'S', 'S'},
			{'y', 'r'},
			{'Y', 'R'},
			{'k', 'm'},
			{'K', 'M'},
			{'v', 'b'},
			{'V', 'B'},
			{'h', 'd'},
			{'H', 'D'},
			{'d', 'h'},
			{'D', 'H'},
			{'b', 'v'},
			{'B', 'V'},
			{'n', 'n'},
			{'N', 'N'},
		});

		bool Matches(char c, char p);

		/*
		Function to find sequence matching an IUPAC nucleotide sequence, starting at start or 0 if start isn't provided. 

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
		std::pair<std::string, int> Find(std::string& Code, std::string pattern, int start)
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
			for (int i = start; i < Code.length() - pattern.length() + 1; ++i)
			{
				for (int p = 0; p < pattern.length(); ++p)    //Loop through pattern
				{
					if (!Matches(Code[i + p], pattern[p]))
						goto ContinueLoop;
				}
				return { Code.substr(i, pattern.size()), i };

			ContinueLoop:
				continue;
			}
			return { std::string(), -1 };
		}

		std::string GetComplement(const std::string& Code)
		{
			std::string Output = "";
			Output.reserve(Code.size());
			for (char c : Code)
			{
				Output += Complements.at(c);
			}
			return Output;
		}

		std::string GetReverseComplement(const std::string& Code)
		{
			std::string Output = GetComplement(Code);
			int n = Output.length() - 1;
			for (int i = 0; i < Output.length() / 2; ++i)
			{
				std::swap(Output[i], Output[n]);
				--n;
			}
			return Output;
		}

		float GetGCRAtio(const std::string& Code)
		{
			float NumGC = 0;
			for (char c : Code)
			{
				switch (c)
				{
				case 'g':
				case 'c':
				case 'G':
				case 'C':
				case 's':
				case 'S':
					NumGC++;
				}
			}
			return NumGC / Code.size();
		}

		float GetAGRatio(const std::string& Code)
		{
			float NumAG = 0;
			for (char c : Code)
			{
				switch (c)
				{
				case 'a':
				case 'g':
				case 'A':
				case 'G':
				case 'r':
				case 'R':
					NumAG++;
				}
			}
			return NumAG / Code.size();
		}

		/*
		Function to generate a sequence matching an IUPAC nucleotide sequence.

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

		std::string GenWithPattern(std::string& pattern)
		{
			std::string Output;
			Output.reserve(pattern.size());
			for (int i = 0; i < pattern.size(); ++i)
			{
				switch (std::tolower(pattern[i]))
				{
				case 'r':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'a';
					}
					else
					{
						Output += 'g';
					}
					break;
				}
				case 'y':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'c';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 's':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'g';
					}
					else
					{
						Output += 'c';
					}
					break;
				}
				case 'w':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'a';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 'k':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'g';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 'm':
				{
					bool b = rand() & 1;    //Masking
					if (b)
					{
						Output += 'a';
					}
					else
					{
						Output += 'c';
					}
					break;
				}
				case 'b':
				{
					int r = rand() % 3;
					if (r == 0)
					{
						Output += 'c';
					}
					else if (r == 1)
					{
						Output += 'g';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 'd':
				{
					int r = rand() % 3;
					if (r == 0)
					{
						Output += 'a';
					}
					else if (r == 1)
					{
						Output += 'g';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 'h':
				{
					int r = rand() % 3;
					if (r == 0)
					{
						Output += 'a';
					}
					else if (r == 1)
					{
						Output += 'c';
					}
					else
					{
						Output += 't';
					}
					break;
				}
				case 'v':
				{
					int r = rand() % 3;
					if (r == 0)
					{
						Output += 'a';
					}
					else if (r == 1)
					{
						Output += 'c';
					}
					else
					{
						Output += 'g';
					}
					break;
				}
				case 'n':
				{
					int r = rand();
					bool b1 = r & 0b01;    //Masking
					bool b2 = r & 0b10;

					if (b1)
					{
						if (b2)
						{
							Output += 'a';
						}
						else
						{
							Output += 't';
						}
					}
					else
					{
						if (b2)
						{
							Output += 'c';
						}
						else
						{
							Output += 'g';
						}
					}
					break;
				}

				case 'a':
				case 't':
				case 'c':
				case 'g':
				case 'u':
					Output += pattern[i];
					break;
				default:
					break;
				}
			}
			return Output;
		}

		bool FindNearby(std::string& Code1, std::string& Code2, std::string& Pattern)
		{
			/*
			Slides along the strand like so (for pattern nnn and Code1, Code2 of aaaa, tttt
			aaaatttt
			  nnn
			   nnn
			    nnn
				 nnn
			*/
			if (Code1.length() == 0 && Code2.length() < Pattern.length())
				return false;

			for (int i = 0; i < Code2.length(); ++i)    //We will always check Code2.length() bases
			{
				int Offset = -Pattern.length() + 1 + i;
				for (int p = 0; p < Pattern.length(); ++p)
				{
					for (int i = Code1.length() + Offset; i < Code1.length(); ++i)
					{
						if (!Matches(Code1[i], Pattern[p]))
							return false;
						++p;
					}
					for (p; p < Pattern.length(); ++p)
					{
						if (!Matches(Code2[p + Offset], Pattern[p]))
							return false;
					}
				}
			}
			return true;
		}

		std::string GetRestrictionSite(RestrictionSite rs)
		{
			switch (rs)
			{
			case RestrictionSite::EcoRI:
				return "CTTAAG";
			case RestrictionSite::EcoRII:
				return "GGWCC";
			case RestrictionSite::BamHI:
				return "CCTAGG";
			case RestrictionSite::HindIII:
				return "TTCGAA";
			case RestrictionSite::TaqI:
				return "AGCT";
			case RestrictionSite::NotI:
				return "CGCCGGCG";
			case RestrictionSite::HinFI:
				return "CTNAG";
			case RestrictionSite::Sau3AI:
				return "CTAG";
			case RestrictionSite::PvuII:
				return "GTCGAC";
			case RestrictionSite::SmaI:
				return "GGGCCC";
			case RestrictionSite::HaeIII:
				return "CCGG";
			case RestrictionSite::HgaI:
				return "CTGCG";
			case RestrictionSite::AluI:
				return "TCGA";
			case RestrictionSite::EcoRV:
				return "CTATAG";
			case RestrictionSite::EcoP15I:
				return "GTCGTCNNNNNNNNNNNNNNNNNNNNNNNNNNN";
			case RestrictionSite::KpnI:
				return "CCATGG";
			case RestrictionSite::PstI:
				return "GACGTC";
			case RestrictionSite::SacI:
				return "CTCGAG";
			case RestrictionSite::SaII:
				return "CAGCTG";
			case RestrictionSite::ScaI:
				return "TCATGA";
			case RestrictionSite::SpeI:
				return "TGATCA";
			case RestrictionSite::SphI:
				return "CGTACG";
			case RestrictionSite::StuI:
				return "TCCGGA";
			case RestrictionSite::XbaI:
				return "AGATCT";
			default:
				return "";
			}
		}

		std::string GetRestrictionSite(std::string& rs)
		{
			return GetRestrictionSite(Str2RestrictionSite.at(rs));
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

		std::pair<bool, std::string> FindSequenceInProject(std::string& SequenceName, Parser::Project* Proj, std::string Stem)
		{
			if (Proj->Sequences.contains(SequenceName))
			{
				return { true, Stem };
			}

			//Recursively search through namespaces for the sequence
			for (auto Namespace : Proj->Namespaces)
			{
				auto Output = FindSequenceInProject(SequenceName, Namespace.second, Stem + "::" + Namespace.first);
				if (Output.first)
					return Output;
			}

			return { false, "" };
		}

		GILAPI std::pair<std::string, std::string> FindSequenceInIntermediates(std::string& SequenceName)
		{
			//First, loop through files in intermediate directory
			CDB::Application& app = CDB::Application::Get();
			std::filesystem::recursive_directory_iterator it(app.OpenProject->Path + app.OpenProject->PreBuildDir);
			for (auto file : it)
			{
				if (file.path().extension() == ".cgil")
				{
					if (file.path().filename().string()[0] == '¬')
						continue;
					//Now, check if this file has the sequence
					CDB::scoped_ptr<Parser::Project> Proj = Parser::Project::Load(file.path().string());
					auto Output = FindSequenceInProject(SequenceName, Proj.raw(), file.path().stem().string());
					if (Output.first)
						return { std::filesystem::relative(file.path(), app.PathToEXE + "/" + app.OpenProject->PreBuildDir).stem().string(), Output.second};
				}
			}

			return { "", "" };
		}
	}
}