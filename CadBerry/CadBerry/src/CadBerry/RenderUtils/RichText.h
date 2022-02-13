#pragma once
#include <cdbpch.h>
#include "CadBerry/Utils/memory.h"
#include "CadBerry/Rendering/Renderer.h"
#include "CadBerry/Core.h"

namespace CDB
{
	/*
	Rich text inputs:
	!#ffffff - Change text colour
	*/
	void RichText(std::string& Text, float x, float y, float width, float height);
}