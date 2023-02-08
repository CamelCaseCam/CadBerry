#pragma once
#include <cdbpch.h>
#pragma warning(push, 0)
#include <imgui.h>
#pragma warning(pop)

const std::vector<ImVec4> Colours = { {0.133, 0.192, 0.152, 1.0}, {0.765, 0.259, 0.247, 1.0}, {0.388, 0.725, 0.584, 1.0}, {0.859, 0.702, 0.694, 1.0},
	{0.349, 0.247, 0.384, 1.0}, {0.137, 0.122, 0.125, 1.0}, {0.0, 0.0, 0.8, 1.0}, {0.733, 0.266, 0.188, 1.0}, {0.255, 0.478, 0.745, 1.0}, {0.0, 0.0, 0.8, 1.0},
	{0.953, 0.875, 0.635, 1.0}, {0.553, 0.580, 0.729, 1.0} };
extern int CurrentColour;
inline int NextColour()
{
	int colour = CurrentColour;
	++CurrentColour;
	if (CurrentColour > Colours.size())
		CurrentColour = 0;
	return CurrentColour;
}