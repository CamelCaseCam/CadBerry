#include <cdbpch.h>
#include "Update.h"
#include "LoadPackages.h"
#include "CadBerry/Application.h"

#include "imgui.h"

#include <sys/stat.h>
#include <stdlib.h>

namespace CDB
{
	void ShowPackageManager()
	{
		if (ImGui::CollapsingHeader("Out of date"))
		{
			for (Package& pkg : Packages)
			{
				if (pkg.changed)
				{
					ImGui::Text(("Name: " + pkg.Name).c_str());
					ImGui::Text(("Last updated: " + pkg.Downloaded).c_str());
					if (ImGui::Button("Update"))
					{
						std::string tmp1 = "\"https://github.com/" + pkg.Owner + "/" + pkg.Repo + "/raw/" + pkg.Branch + "/" + pkg.Path 
							+ "\" \"" + Application::Get().PathToEXE + "/\" " + std::to_string(pkg.Files.size()) + " ";
						for (std::string& file : pkg.Files)
						{
							tmp1 += "\"" + file + "\" ";
						}
						CDB_BuildInfo(tmp1);

						std::wstring tmp2 = std::wstring(tmp1.begin(), tmp1.end());

						LPCWSTR params = tmp2.c_str();

						std::string tmp3 = Application::Get().PathToEXE + "/CadBerry_updater";
						std::wstring tmp4 = std::wstring(tmp3.begin(), tmp3.end());
						LPCWSTR ExeName = tmp4.c_str();
						ShellExecute(nullptr, NULL, ExeName, params, 0, SW_SHOW);
						exit(0);
					}
				}
			}
		}

		if (ImGui::CollapsingHeader("Up to date"))
		{
			for (Package& pkg : Packages)
			{
				if (!pkg.changed)
				{
					ImGui::Text(("Name: " + pkg.Name).c_str());
					ImGui::Text(("Last updated: " + pkg.Downloaded).c_str());
				}
			}
		}
	}
}