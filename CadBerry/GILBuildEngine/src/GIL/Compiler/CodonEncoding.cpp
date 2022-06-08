#include "gilpch.h"
#include "CodonEncoding.h"
#include "GIL/Lexer/LexerMacros.h"

#include "Core.h"

namespace GIL
{
	namespace Compiler
	{
        //A dictionary to convert codons to their amino acids
        std::unordered_map<std::string, char> Codon2Letter = std::unordered_map<std::string, char>({
            {"gga", 'g'},
            {"ggt", 'g'},
            {"ggg", 'g'},
            {"ggc", 'g'},
            {"gca", 'a'},
            {"gct", 'a'},
            {"gcg", 'a'},
            {"gcc", 'a'},
            {"gta", 'v'},
            {"gtt", 'v'},
            {"gtg", 'v'},
            {"gtc", 'v'},
            {"cta", 'l'},
            {"ctt", 'l'},
            {"ctg", 'l'},
            {"ctc", 'l'},
            {"ttg", 'l'},
            {"tta", 'l'},
            {"att", 'i'},
            {"ata", 'i'},
            {"atc", 'i'},
            {"atg", 'm'},
            {"cca", 'p'},
            {"cct", 'p'},
            {"ccg", 'p'},
            {"ccc", 'p'},
            {"ttt", 'f'},
            {"ttc", 'f'},
            {"tgg", 'w'},
            {"tca", 's'},
            {"tct", 's'},
            {"tcg", 's'},
            {"tcc", 's'},
            {"agt", 's'},
            {"agc", 's'},
            {"aca", 't'},
            {"act", 't'},
            {"acg", 't'},
            {"acc", 't'},
            {"aat", 'n'},
            {"aac", 'n'},
            {"caa", 'q'},
            {"cag", 'q'},
            {"tat", 'y'},
            {"tac", 'y'},
            {"tgt", 'c'},
            {"tgc", 'c'},
            {"aaa", 'k'},
            {"aag", 'k'},
            {"aga", 'r'},
            {"agg", 'r'},
            {"cga", 'r'},
            {"cgt", 'r'},
            {"cgg", 'r'},
            {"cgc", 'r'},
            {"cat", 'h'},
            {"cac", 'h'},
            {"gat", 'd'},
            {"gac", 'd'},
            {"gaa", 'e'},
            {"gag", 'e'},
            {"taa", 'x'},
            {"tga", 'x'},
            {"tag", 'x'},
        });

        std::string Empty = "";

        CodonEncoding::CodonEncoding(std::string Organism)
        {
        CheckFile:
            if (Organism == "")
            {
                CDB_BuildError("Error no target organism specified");
                return;
            }
            std::ifstream OrganismFile(DataPath + "Organism\\" + Organism + ".gilEncoding");

            if (!OrganismFile.is_open())
            {
                CDB_BuildError("Error invalid compilation target: could not open gilEncoding file at location {0}",
                    DataPath + "Organism\\" + Organism + ".gilEncoding");
                OrganismFile.close();
                return;
            }

            std::string Line;
            std::vector<std::string> Lines;

            //Check if it's goto
            std::getline(OrganismFile, Line, '\n');
            if (Line[1] != ' ')    //if the second char is not ' ' we know it's probably goto
            {
                std::stringstream ss(Line);
                std::string word;
                if (std::getline(ss, word, ' '))
                {
                    if (word == "goto")
                    {
                        if (std::getline(ss, word))
                        {
                            Organism = word;
                            goto CheckFile;
                        }
                    }
                }
            }

            Lines.push_back(Line);
            while (std::getline(OrganismFile, Line, '\n'))
                Lines.push_back(Line);
            OrganismFile.close();
            for (std::string Line : Lines)
            {
                std::stringstream ss(Line);
                std::string Code;
                std::vector<std::string*>* Codons = new std::vector<std::string*>();    //Has to be allocated on the heap because we'll pass around the pointer
                if (std::getline(ss, Code, ':'))
                {
                    std::string Codon;
                    while (std::getline(ss, Codon, ' '))
                    {
                        std::string* CurrentCodon = new std::string(Codon);
                        for (int i = 0; i < CurrentCodon->length(); ++i)
                        {
                            (*CurrentCodon)[i] = std::tolower((*CurrentCodon)[i]);
                        }
                        Codons->push_back(CurrentCodon);
                    }
                }
                this->Code2Codon[Code[0]] = Codons;
            }
        }

        std::string* CodonEncoding::GetFromLetter(char l, int idx)
        {
            l = std::tolower(l);

            if (IsWhiteSpace(l))
                return &Empty;
            return Code2Codon[l]->operator[](idx);
        }

        std::pair<char, int> CodonEncoding::CodonToLetter(std::string codon)
        {
            for (int i = 0; i < codon.length(); ++i)    //Convert codon to lower case
            {
                codon[i] = std::tolower(codon[i]);
            }
            char c = Codon2Letter[codon];
            std::vector<std::string*>* Codons = Code2Codon[c];
            
            for (int i = 0; i < Codons->size(); ++i)
            {
                if (*(*Codons)[i] == codon)
                {
                    return { c, i };
                }
            }
            return { c, 0 };
        }

        char CodonEncoding::CodonToLetterOnly(std::string codon)
        {
            return Codon2Letter[codon];
        }

        CodonEncoding::~CodonEncoding()
        {
            for (auto c : Code2Codon)
            {
                for (auto str : *c.second)
                {
                    delete str;
                }
            }
        }
        
        std::string CodonEncoding::ToString() const
        {
            std::stringstream ss;
            ss << "Codon encoding\n{\n";
            for (auto c : this->Code2Codon)
            {
                ss << c.first << " : ";
                for (std::string* codon : *c.second)
                {
                    ss << *codon << ", ";
                }
                ss << std::endl;

            }
            ss << "}" << std::endl;
            return ss.str();
        }
	}
}