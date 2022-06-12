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
IncludeDirs["BlockingCollection"] = "CadBerry/vendor/BlockingCollection"
IncludeDirs["ImPlot"] = "CadBerry/vendor/implot"
IncludeDirs["CDBRNA_inc"] = "CadBerry/vendor/RNAstructure/CDBRNA/include"
IncludeDirs["CDBRNA"] = "CadBerry/vendor/RNAstructure"

include "CadBerry/vendor/GLFW"
include "CadBerry/vendor/RNAstructure/CDBRNA"
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
		"%{IncludeDirs.ImPlot}",
		"CadBerry/vendor/BlockingCollection",
		"CadBerry/vendor/cpr/include",
		"CadBerry/vendor/cpr/cpr_generated_includes",
		"CadBerry/vendor/cpr/_deps/curl-src/include",
		"CadBerry/vendor/yaml-cpp/include",
		"%{IncludeDirs.CDBRNA}",
		"%{IncludeDirs.CDBRNA_inc}",
	}

	libdirs
	{
		"%{prj.name}/vendor/SFML/lib",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"WhereAmI",
		"CDBRNA"
	}

	filter {"system:linux", "system:macosx"}
		cppdialect "C++20"
		staticruntime "Off"
		pic "On"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_LINUX",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2",
			"FMT_HEADER_ONLY"
		}

		libdirs
		{
			"CadBerry/vendor/cpr/lib",
			"CadBerry/vendor/yaml-cpp",
			"CadBerry/vendor/GLFW/bin/Debug-linux-x86_64/GLFW",
			"CadBerry/vendor/nfd/build/lib/Release/x64/"
		}

		links
		{
			"GL",
			"cpr",
			"yaml-cpp",
			"GLFW",
			"nfd",
			"gtk-3"
		}
	filter {"system:linux", "configurations:Debug"}
		buildoptions "-g"

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"CDB_BUILD_DLL",
			"CDB_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2",
			"FMT_HEADER_ONLY"
		}

		links
		{
			"opengl32.lib",
			"cpr",
		}

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CadBerry/CadBerry.pdb ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CadBerry/CadBerry.pdb ../bin/" .. OutputDir .. "/CadBerry_test"),
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"

		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
		libdirs
		{
			"CadBerry/vendor/yaml-cpp/Debug",
			"CadBerry/vendor/cpr/lib/Debug",
			"CadBerry/vendor/nfd/build/lib/Debug/x64",
			"CadBerry/vendor/RNAstructure/CDBRNA/bin/Debug-windows-x86_64/CDBRNA"
		}

	filter {"configurations:Debug", "system:windows"}
		links
		{
			"yaml-cppd",
			"nfd_d"
		}

	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"

		optimize "On"
		links
		{
			"nfd",
			"yaml-cpp"
		}
		libdirs
		{
			"CadBerry/vendor/yaml-cpp/RelWithDebInfo",
			"CadBerry/vendor/cpr/lib/RelWithDebInfo",
			"CadBerry/vendor/nfd/build/lib/Release/x64",
			"CadBerry/vendor/RNAstructure/CDBRNA/bin/Release-windows-x86_64/CDBRNA"
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
		"CadBerry/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"CadBerry/vendor/BlockingCollection",
		"%{IncludeDirs.ImPlot}",
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
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_WINDOWS",
			"FMT_HEADER_ONLY"
		}

	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
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
		runtime "Release"
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


project "CadBerry_test"
	location "CadBerry_test"
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
		"CadBerry/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"CadBerry/vendor/BlockingCollection",
		"%{IncludeDirs.ImPlot}",
		"CadBerry_test/vendor/Catch2/src/catch2",
		"CadBerry_test/vendor/Catch2/src",
		"CadBerry_test/vendor/Catch2/build/generated-includes",
		"GILBuildEngine/src",
	}

	libdirs
	{
		"CadBerry/vendor/SFML/lib"
	}

	links
	{
		"CadBerry",
		"GILBuildEngine"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_WINDOWS",
			"FMT_HEADER_ONLY"
		}

	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"
		links
		{
			"sfml-system-d",
			"sfml-window-d",
			"sfml-graphics-d",
			"sfml-network-d",
			"sfml-audio-d",
			"Catch2d",
			"Catch2Maind",
		}

		libdirs
		{
			"CadBerry_test/vendor/Catch2/build/src/Debug"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"
		links
		{
			"sfml-system",
			"sfml-window",
			"sfml-graphics",
			"sfml-network",
			"sfml-audio",
			"Catch2",
			"Catch2Main",
		}

		libdirs
		{
			"CadBerry_test/vendor/Catch2/build/src/Release"
		}

	architecture "x64"
	configurations
	{
		"Debug",
		"Release"
	}

project "CadBerry_updater"
	location "CadBerry_updater"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.cpp"
	}

	includedirs
	{
		"CadBerry/vendor/cpr/include",
		"CadBerry/vendor/cpr/cpr_generated_includes",
		"CadBerry/vendor/cpr/_deps/curl-src/include",
	}

	links
	{
		"cpr"
	}

	filter {"system:linux", "system:macosx"}
		defines
		{
			"CDB_PLATFORM_LINUX",
		}

	filter "system:windows"
		cppdialect "C++20"

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CadBerry_updater/CadBerry_updater.pdb ../bin/" .. OutputDir .. "/Berry"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/CadBerry_updater/CadBerry_updater.pdb ../bin/" .. OutputDir .. "/CadBerry_test"),
		}

	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"

		libdirs
		{
			"CadBerry/vendor/cpr/lib/Debug"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"

		libdirs
		{
			"CadBerry/vendor/cpr/lib/RelWithDebInfo"
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
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImPlot}",
	}

	links
	{
		"CadBerry",
		"ImGui",
	}

	filter {"system:linux", "system:macosx"}
		defines
		{
			"CDB_PLATFORM_LINUX",
		}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/VSCodeIntegration/VSCodeIntegration.pdb ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/CadBerry_test/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/VSCodeIntegration/VSCodeIntegration.pdb ../../bin/" .. OutputDir .. "/CadBerry_test/Modules")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"

		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"

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
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImPlot}",
	}

	links
	{
		"CadBerry",
		"ImGui",
	}

	defines
	{
		"GIL_BUILD_DLL",
	}

	filter {"system:linux", "system:macosx"}
		cppdialect "C++20"
		staticruntime "Off"
		pic "On"
		systemversion "latest"

		defines
		{
			"CDB_PLATFORM_LINUX",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD2",
			"FMT_HEADER_ONLY"
		}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} ../bin/" .. OutputDir .. "/GILBuildEngine/GILBuildEngine.pdb ../bin/" .. OutputDir .. "/Berry/Build"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test/Build"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/GILBuildEngine/GILBuildEngine.pdb ../bin/" .. OutputDir .. "/CadBerry_test/Build"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/GILBuildEngine/GILBuildEngine.pdb ../bin/" .. OutputDir .. "/CadBerry_test"),
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"

		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"

		optimize "On"

-- GIL Modules
project "utils"
	location "utils"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

	pchheader("utilspch.h")
	pchsource("utils/src/utilspch.cpp")

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
		"utils/src",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.ImPlot}",
	}

	links
	{
		"CadBerry",
		"ImGui",
		"GILBuildEngine",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry/GILModules"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/utils/utils.pdb ../bin/" .. OutputDir .. "/Berry/GILModules"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test/GILModules"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/utils/utils.pdb ../bin/" .. OutputDir .. "/CadBerry_test/GILModules"),
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"

project "sensing"
	location "sensing"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin/" .. OutputDir .. "/%{prj.name}")

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
		"utils/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.ImPlot}",
	}

	links
	{
		"CadBerry",
		"ImGui",
		"GILBuildEngine",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Berry/GILModules"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/sensing/sensing.pdb ../bin/" .. OutputDir .. "/Berry/GILModules"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/CadBerry_test/GILModules"),
			("{COPYFILE} ../bin/" .. OutputDir .. "/sensing/sensing.pdb ../bin/" .. OutputDir .. "/CadBerry_test/GILModules")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"

project "Core"
	location "Modules/Core"
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
		"Core/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.ImPlot}",
		"%{IncludeDirs.nfd}",
	}

	links
	{
		"CadBerry",
		"ImGui",
		"GLFW",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/Core/Core.pdb ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/CadBerry_test/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/Core/Core.pdb ../../bin/" .. OutputDir .. "/CadBerry_test/Modules")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
		links
		{
			"nfd_d"
		}
		libdirs
		{
			"CadBerry/vendor/nfd/build/lib/Debug/x64",
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"
		links
		{
			"nfd"
		}
		libdirs
		{
			"CadBerry/vendor/nfd/build/lib/Release/x64",
		}

project "IRESGenerator"
	location "Modules/IRESGenerator"
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
		"IRESGenerator/src",
		"GILBuildEngine/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.BlockingCollection}",
		"%{IncludeDirs.ImPlot}",
		"%{IncludeDirs.nfd}",
		"%{IncludeDirs.CDBRNA}",
		"%{IncludeDirs.CDBRNA_inc}",
	}

	links
	{
		"CadBerry",
		"ImGui",
		"GLFW",
		"GILBuildEngine",
		"CDBRNA",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
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
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/IRESGenerator/IRESGenerator.pdb ../../bin/" .. OutputDir .. "/Berry/Modules"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../../bin/" .. OutputDir .. "/CadBerry_test/Modules"),
			("{COPYFILE} ../../bin/" .. OutputDir .. "/IRESGenerator/IRESGenerator.pdb ../../bin/" .. OutputDir .. "/CadBerry_test/Modules")
		}
	filter "configurations:Debug"
		defines "CDB_DEBUG"
		runtime "Debug"
		symbols "On"
		defines
		{
			"CDB_ENABLE_ASSERTS"
		}
		links
		{
			"nfd_d"
		}
		libdirs
		{
			"CadBerry/vendor/nfd/build/lib/Debug/x64",
			"CadBerry/vendor/RNAstructure/CDBRNA/bin/Debug-windows-x86_64/CDBRNA",
		}
	filter "configurations:Release"
		defines "CDB_RELEASE"
		runtime "Release"
		optimize "On"
		links
		{
			"nfd"
		}
		libdirs
		{
			"CadBerry/vendor/nfd/build/lib/Release/x64",
			"CadBerry/vendor/RNAstructure/CDBRNA/bin/Release-windows-x86_64/CDBRNA"
		}
