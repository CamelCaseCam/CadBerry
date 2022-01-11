#include "Simulation.h"
#include "CadBerry/Log.h"

#include "CadBerry/Application.h"

#include "ImPlot.h"

Simulation::Simulation(std::string src)
{
	int NumChars = 0;
	std::stringstream ss(src);
	std::string line;
	
	bool DefVars = true;

	while (std::getline(ss, line))
	{
		if (DefVars)
		{
			if (line == "math:" || line == "math: ")
			{
				DefVars = false;
			}
			else
			{
				GetVar(line);
			}
		}
		else
		{
			if (line == "")
				continue;

			this->Equations.push_back(CDB::Math::Equation(line));
		}
	}
}

void Simulation::Reset()
{
	idx = 0;
	times.clear();

	ImPlot::CreateContext();
	CDB::Math::MathContext::Init();
	for (VarInfo vi : this->Variables)
	{
		if (vi.Display)
		{
			vi.vals.clear();
		}
		(*CDB::Math::MathContext::NextTimestepVariables)[vi.Name] = vi.InitialValue;
	}
}

void Simulation::GetVar(std::string Line)
{
	int EqIdx = Line.find('=');
	std::string VarName = Line.substr(0, EqIdx);
	CDB_BuildInfo(VarName);
	std::string DefaultValue;
	std::string Show;

	int ColonIdx = Line.find(':', EqIdx);
	if (ColonIdx == -1)
	{
		DefaultValue = Line.substr(EqIdx);
	}
	else
	{
		DefaultValue = Line.substr(EqIdx + 1, EqIdx - ColonIdx);
		Show = Line.substr(ColonIdx + 1);
	}

	VarInfo CurrentVar;
	CurrentVar.Name = VarName;
	CDB_BuildInfo(DefaultValue);
	CurrentVar.InitialValue = std::stof(DefaultValue);
	CurrentVar.Display = Show == "show";

	Variables.push_back(CurrentVar);
}

void Simulation::Draw()
{
	ImPlot::BeginPlot("TestPlot");
	ImPlot::SetupAxes("Time", "Value");
	for (VarInfo& vi : this->Variables)
	{
		if (vi.Display)
		{
			ImPlot::PlotLine<float>(vi.Name.c_str(), this->times.data(), vi.vals.data(), this->times.size());
		}
	}

	ImPlot::EndPlot();
}

void Simulation::RunToCSV(float TotalTime, std::string Path)
{
	int NumSteps = 1 + TotalTime / this->Timestep;
	RunToCSV(NumSteps, Path);
}

void Simulation::RunToCSV(int NumSteps, std::string Path)
{
	std::ofstream OutputFile(Path);
	OutputFile << "Time";

	for (VarInfo& vi : this->Variables)
		OutputFile << "," << vi.Name;
	OutputFile << "\n";

	for (int i = 0; i < NumSteps; ++i)
	{
		CDB::Math::MathContext::BeginTimestep(this->Timestep);
		OutputFile << this->Timestep * (i + 1);

		for (CDB::Math::Equation eq : this->Equations)
			eq.Calculate();

		for (VarInfo& vi : this->Variables)
		{
			if (vi.Display)
			{
				OutputFile << "," << (*CDB::Math::MathContext::NextTimestepVariables)[vi.Name];
			}
		}
		OutputFile << "\n";
	}
}

void Simulation::Run(float TotalTime)
{
	int NumSteps = 1 + TotalTime / this->Timestep;
	Run(NumSteps);
}

void Simulation::Run(int NumSteps)
{
	this->times.reserve(NumSteps);
	for (VarInfo& vi : this->Variables)
	{
		if (vi.Display)
			vi.vals.reserve(NumSteps);
	}

	for (int i = 0; i < NumSteps; ++i)
	{
		CDB::Math::MathContext::BeginTimestep(this->Timestep);
		this->times.push_back(this->Timestep * (i + 1));

		for (CDB::Math::Equation eq : this->Equations)
			eq.Calculate();
		
		for (VarInfo& vi : this->Variables)
		{
			if (vi.Display)
			{
				vi.vals.push_back((*CDB::Math::MathContext::NextTimestepVariables)[vi.Name]);
				CDB_BuildInfo("{0}:{1}", vi.Name, (*CDB::Math::MathContext::NextTimestepVariables)[vi.Name]);
			}
		}
	}
}