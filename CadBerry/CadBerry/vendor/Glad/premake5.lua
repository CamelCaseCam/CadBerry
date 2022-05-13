project "Glad"
    kind "StaticLib"
    language "C"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. OutputDir .. "/%{prj.name}")

	files
	{
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c",
    }

    includedirs
    {
        "include"
    }

    filter "system:linux"
        pic "On"

	filter "system:windows"
        systemversion "latest"
        staticruntime "On"
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
