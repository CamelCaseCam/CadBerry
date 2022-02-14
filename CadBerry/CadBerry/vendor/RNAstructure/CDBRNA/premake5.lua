project "CDBRNA"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. OutputDir .. "/%{prj.name}")

	files
	{
		"src/**.cpp",
<<<<<<< HEAD
		"include/**.h",
=======
		"src/**.h",
		"include/**.h"
	}

	includedirs
	{
		"src",
		"include",
		"../",
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands
		{
<<<<<<< HEAD
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CDBRNA/CDBRNA.pdb ../bin/" .. OutputDir .. "/Berry")
=======
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../../../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} bin/" .. OutputDir .. "/CDBRNA/CDBRNA.pdb ../../../../bin/" .. OutputDir .. "/Berry")
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807
		}

		defines
		{
<<<<<<< HEAD
			"RNA_LIBRARY"
=======
			"RNA_DLL"
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		buildoptions "/MD"
		optimize "On"