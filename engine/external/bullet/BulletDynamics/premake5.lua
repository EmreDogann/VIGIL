	project "BulletDynamics"
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
		"Dynamics/*.cpp",
                "Dynamics/*.h",
                "ConstraintSolver/*.cpp",
                "ConstraintSolver/*.h",
                "Featherstone/*.cpp",
                "Featherstone/*.h",
                "MLCPSolvers/*.cpp",
                "MLCPSolvers/*.h",
                "Vehicle/*.cpp",
                "Vehicle/*.h",
                "Character/*.cpp",
                "Character/*.h"

	}

	filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
