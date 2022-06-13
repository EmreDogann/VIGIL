project "LinearMath"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    systemversion "latest"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("inter/" .. outputdir .. "/%{prj.name}")

	warnings "Off"
	includedirs {
		"..",
	}
	files {
		"*.cpp",
		"*.h",
		"TaskScheduler/*.cpp",
		"TaskScheduler/*.h"
	}

	filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
