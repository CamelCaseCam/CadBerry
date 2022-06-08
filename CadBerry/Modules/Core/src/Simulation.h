#pragma once
#include "CadBerry/Math/Equation.h"

struct VarInfo
{
	std::string Name;
	bool Display;
	float InitialValue;
	std::vector<float> vals;
};

class Simulation
{
public:
	Simulation() {}
	Simulation(std::string src);

	void Reset();
	void SetTimestep(float t) { Timestep = t; }
	void Run(float TotalTime);
	void Run(int NumSteps);
	void Draw();
	void RunToCSV(int NumSteps, std::string Path);
	void RunToCSV(float TotalTime, std::string Path);

private:
	void GetVar(std::string Line);

	float FloorValue = 200.0f;
	float TotalDeltaTime = 0.0f;
	int idx = 0;
	std::vector<float> times;
	float Timestep = 0.1f;
	std::vector<VarInfo> Variables;
	std::vector<CDB::Math::Equation> Equations;
};