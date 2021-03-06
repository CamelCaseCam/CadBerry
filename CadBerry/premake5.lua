workspace "CadBerry"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release"
	}

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "CadBerry/vendor/GLFW/include"
IncludeDirs["Glad"] = "CadBerry/vendor/Glad/include"
IncludeDirs["ImGui"] = "CadBerry/vendor/IMGUI"
IncludeDirs["glm"] = "CadBerry/vendor/glm"
IncludeDirs["nfd"] = "CadBerry/vendor/nfd/src/include"
IncludeDirs["WhereAmI"] = "CadBerry/vendor/WhereAmI/src"

include "CadBerry/vendor/GLFW"
include "CadBerry/vendor/Glad"
include "CadBerry/vendor/IMGUI"
include "CadBerry/vendor/WhereAmI"
--include "CadBerry/vendor/nfd/build"

project "CadBerry"
	location "CadBerry"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	pchheader("cdbpch.h")
	pchsource("CadBerry/src/cdbpch.cpp")

	--linkoptions { '/DEFAULTLIB:"LIBCMT',  }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",

		"%{prj.name}/src/Events/**.h",
		"%{prj.name}/src/Events/**.hpp",
		"%{prj.name}/src/Events/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/SFML/include",
		"CadBerry/src",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.nfd}",
		"%{IncludeDirs.WhereAmI}",
	}

	libdirs
	{
		"%{prj.name}/vendor/SFML/lib",
		"CadBerry/vendor/nfd/build/lib/Debug/x64"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"WhereAmI",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CDB_BUILD_DLL",
			"CDB_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2"
		}

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CadBerry/CadBerry.pdb ../bin/" .. OutputDir .. "/Berry")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
		links
		{
			"nfd_d"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		buildoptions "/MD"
		optimize "On"
		links
		{
			"nfd"
		}
project "Berry"
	location "Berry"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"CadBerry/vendor/spdlog/include",
		"CadBerry/src"
	}

	libdirs
	{
		"CadBerry/vendor/SFML/lib"
	}

	links
	{
		"CadBerry"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CDB_DEBUG"
		symbols "On"
		links
		{
			"sfml-system-d",
			"sfml-window-d",
			"sfml-graphics-d",
			"sfml-network-d",
			"sfml-audio-d"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		optimize "On"
		links
		{
			"sfml-system",
			"sfml-window",
			"sfml-graphics",
			"sfml-network",
			"sfml-audio"
		}
	architecture "x64"
	configurations
	{
		"Debug",
		"Release"
	}

project "VSCodeIntegration"
	location "Modules/VSCodeIntegration"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	--linkoptions { '/DEFAULTLIB:"LIBCMT',  }

	files
	{
		"Modules/%{prj.name}/src/**.h",
		"Modules/%{prj.name}/src/**.hpp",
		"Modules/%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"CadBerry/vendor/spdlog/include",
		"CadBerry/src",
		"VSCodeIntegration/src",
		"%{IncludeDirs.ImGui}",
	}

	links
	{
		"CadBerry",
		"ImGui",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2"
		}

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/VSCodeIntegration/VSCodeIntegration.pdb ../../bin/" .. OutputDir .. "/Berry/Modules")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		buildoptions "/MD"
		optimize "On"

project "GILBuildEngine"
	location "GILBuildEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	pchheader("gilpch.h")
	pchsource("GILBuildEngine/src/gilpch.cpp")

	--linkoptions { '/DEFAULTLIB:"LIBCMT',  }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"CadBerry/vendor/spdlog/include",
		"CadBerry/src",
		"GILBuildEngine/src",
		"%{IncludeDirs.ImGui}",
	}

	links
	{
		"CadBerry",
		"ImGui",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2",
			"FMT_HEADER_ONLY"
		}

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry/Build"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/GILBuildEngine/GILBuildEngine.pdb ../bin/" .. OutputDir .. "/Berry/Build")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		buildoptions "/MD"
		optimize "On"