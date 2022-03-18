#include <cdbpch.h>
#include "InputVector.h"
#include "CadBerry/Log.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace CDB
{
	bool InputStringVectorElement(const char* name, std::string& str, const char* ElementName = "element");

	//Element name is what we should call the elements when adding them
	void InputStringVector(const std::string& name, std::vector<std::string>& Vec, const char* ElementName, const char* StringName)
	{
		ImGui::BeginChild((int)&name);
		ImGui::Text(name.c_str());

		int ToBeRemoved = -1;
		for (int i = 0; i < Vec.size(); ++i)
		{
			if (InputStringVectorElement(StringName, Vec[i], ElementName))
			{
				CDB_BuildInfo("Removing element {0}", i);
				ToBeRemoved = i;
			}
		}
		if (ToBeRemoved != -1)
		{
			Vec.erase(Vec.begin() + ToBeRemoved);    //I really hate how the std doesn't have something like C#'s List.RemoveAt()
		}

		if (ImGui::Button((std::string("Add ") + ElementName).c_str()))
		{
			Vec.push_back("Enter text");
		}


		ImGui::EndChild();
	}

	bool InputStringVectorElement(const char* name, std::string& str, const char* ElementName)
	{
		//ImGui::BeginChild((int)&str);
		std::string End = "##" + std::to_string((int)&str);
		ImGui::InputText((std::string(name) + End).c_str(), &str);
		ImGui::SameLine();
		bool output = ImGui::Button((std::string("Delete ") + ElementName + End).c_str());

		//ImGui::EndChild();
		return output;
	}
}