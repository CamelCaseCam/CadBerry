#include "gilpch.h"
#include "GBSequence.h"
#include "Core.h"

std::string GIL::Compiler::GBSequence::WriteToString(std::pair<std::vector<GIL::Parser::Region>, std::string> Output,
    std::string& FileName, GIL::Parser::Project* Proj)
{
    std::stringstream ss;
    ss << "LOCUS                               " << Output.second.length() << " bp  DNA linear\n";
    ss << "DEFINITION  " << FileName << " compiled with GIL version " << GIL_MAJOR << "." << GIL_MINOR << "." << GIL_FIX
        << " for target " << Proj->TargetOrganism << "\n";
    ss << "ACCESSION   \nSOURCE      \n  ORGANISM  " 
        << Proj->TargetOrganism << "\n\nFEATURES             Location/Qualifiers\n     source          1..";
    ss << Output.second.length() << "\n                     /organism=\"" << Proj->TargetOrganism << "\"\n";

    for (auto Region : Output.first)
    {
        ss << "     CDS             " << Region.Start << ".." << Region.End << "\n                     /label=\"" << Region.Name << "\"\n";
    }
    ss << "ORIGIN\n";

    int CurrentBase = 0;
    while (CurrentBase < Output.second.length())
    {
        std::string Num = std::to_string(CurrentBase + 1);
        for (int i = 0; i < 9 - Num.length(); ++i)
        {
            ss << " ";
        }
        ss << Num;

        for (int i = 0; i < 6; ++i)
        {
            ss << " ";
            if (CurrentBase + 10 < Output.second.length())
            {
                ss << Output.second.substr(CurrentBase, 10);
                CurrentBase += 10;
            }
            else
            {
                int RemainingLength = Output.second.length() - CurrentBase;
                ss << Output.second.substr(CurrentBase, RemainingLength);
                CurrentBase = Output.second.length();
                break;
            }
        }
        ss << "\n";
    }
    ss << "//\n";
    return ss.str();
}
