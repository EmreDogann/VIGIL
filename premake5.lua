-- Implement the workspace files command for solution-scope files
require('vstudio')
premake.api.register {
    name = "workspace_files",
    scope = "workspace",
    kind = "list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
    if wks.workspace_files and #wks.workspace_files > 0 then
        premake.push('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "configs", "configs", "{' .. os.uuid("configs:"..wks.name) .. '}"')
        premake.push("ProjectSection(SolutionItems) = preProject")
        for _, path in ipairs(wks.workspace_files) do
            premake.w(path.." = "..path)
        end
        premake.pop("EndProjectSection")
        premake.pop("EndProject")
    end
    base(wks)
end)

-- workspace is the solution
workspace "engine"
    architecture "x64"
    startproject "game"

    configurations
    {
        "Debug",
        "Release", -- more like a faster debug
        "Dist" -- distribution build
    }

    workspace_files 
    {
        ".editorconfig"
    }
    
-- example: debug-win-x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to root folder (sln dir)
IncludeDir = {}
IncludeDir["spdlog"]        = "engine/external/spdlog/include"
IncludeDir["GLFW"]          = "engine/external/GLFW/include"
IncludeDir["Glad"]          = "engine/external/Glad/include"
IncludeDir["glm"]           = "engine/external/glm"
IncludeDir["stb_image"]     = "engine/external/stb_image"
IncludeDir["ImGui"]         = "engine/external/imgui"
IncludeDir["assimp"]        = "engine/external/assimp/include"
IncludeDir["assimpcfg"]     = "engine/external/assimp/config"
IncludeDir["assimpzlib"]    = "engine/external/assimp/contrib/zlib"
IncludeDir["bullet"]        = "engine/external/bullet"
IncludeDir["fmod"]          = "engine/external/FMOD/include"
IncludeDir["freetype"]      = "engine/external/freetype/include/"

-- library directories relative to root folder (sln dir)
LibDir = {}
LibDir["stb_image"]         = "engine/external/stb_image/lib/%{cfg.buildcfg}"
LibDir["GLFW"]              = "engine/external/GLFW/lib/%{cfg.buildcfg}"
LibDir["Glad"]              = "engine/external/Glad/lib/%{cfg.buildcfg}"
LibDir["assimp"]            = "engine/external/assimp/lib/%{cfg.buildcfg}"
LibDir["assimpIrrXML"]      = "engine/external/assimp/contrib/irrXML/%{cfg.buildcfg}"
LibDir["assimpzlib"]        = "engine/external/assimp/contrib/zlib/%{cfg.buildcfg}"
LibDir["bullet_cls"]        = "engine/external/bullet/BulletCollision/lib/%{cfg.buildcfg}"
LibDir["bullet_dnc"]        = "engine/external/bullet/BulletDynamics/lib/%{cfg.buildcfg}"
LibDir["bullet_lm"]         = "engine/external/bullet/LinearMath/lib/%{cfg.buildcfg}"
LibDir["fmod"]              = "engine/external/FMOD/lib/x64/"
LibDir["freetype"]		    = "engine/external/freetype/lib/%{cfg.buildcfg}"


group "dependencies"
    -- iclude other premake files
    include "engine/external/GLFW"
    include "engine/external/Glad"
    include "engine/external/stb_image"
    include "engine/external/bullet/BulletCollision"
    include "engine/external/bullet/BulletDynamics"
    include "engine/external/bullet/LinearMath"
    include "engine/external/freetype"
group""

-- === Core Project: engine =======================================================
project "engine"
    -- location makes sure that everything below will be relative to the project directory
    location "engine"
    kind "StaticLib" -- Static library (.lib)
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("inter/" .. outputdir .. "/%{prj.name}")
    
    pchheader "pch.h"
    pchsource "engine/src/pch.cpp"

    files
    {
        -- ** means recursively search down that folder
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/external/glm/glm/**.hpp", 
        "%{prj.name}/external/glm/glm/**.inl",
        "%{prj.name}/external/FMOD/include/**.h", 
        "%{prj.name}/external/FMOD/include/**.hpp", 
        "%{prj.name}/external/FMOD/include/**.cs",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.assimpcfg}",
		"%{IncludeDir.assimpzlib}",
        "%{IncludeDir.bullet}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.fmod}",
        "%{IncludeDir.freetype}",
    }

    libdirs
    {
        "%{LibDir.stb_image}",
        "%{LibDir.GLFW}",
        "%{LibDir.Glad}",
        "%{LibDir.assimp}",
		"%{LibDir.assimpIrrXML}",
		"%{LibDir.assimpzlib}",
        "%{LibDir.bullet_cls}",
        "%{LibDir.bullet_dnc}",
        "%{LibDir.bullet_lm}",
        "%{LibDir.fmod}",
		"%{LibDir.freetype}",
    }

    links
    {
        "GLFW.lib",
        "Glad.lib",
        "opengl32.lib",
        "stb_image.lib",
        "assimp.lib",
		"IrrXMLd.lib",
		"zlibd.lib",
		"zlibstaticd.lib",
        "BulletCollision.lib",
        "BulletDynamics.lib",
        "LinearMath.lib",
        "fmod_vc.lib",
        "freetype.lib",
    }

	postbuildcommands
    {
		('{COPY} ../%{LibDir.assimpzlib}/zlibd.dll ../bin/' .. outputdir .. '/game/zlibd.dll*'),
        ('{COPY} ../%{LibDir.fmod}/fmod_vc.lib ../bin/' .. outputdir .. '/game/fmod_vc.lib*'),
		('{COPY} ../%{LibDir.fmod}/fmod.dll ../bin/' .. outputdir .. '/game/fmod.dll*')
    }

    -- filters are used to apply property to some specific configurations only
    filter "system:windows"
        systemversion "latest" -- windows SDK version

        defines
        {
            "ENGINE_PLATFORM_WIN",
            "ENGINE_BUILD_DLL",
            "GLFW_INCLUDE_NONE",
            "_WIN32",       -- for freetype
            "DLL_IMPORT",   -- for freetype
        }

    filter "configurations:Debug"
        defines "ENGINE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ENGINE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ENGINE_DIST"
        runtime "Release"
        optimize "on"

-- === Core Project: sanbox =======================================================
project "game"
    location "game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
     objdir ("inter/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "engine/src",
        "engine/external",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.bullet}",
        "%{IncludeDir.freetype}",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.assimpcfg}",
    }

    links
    {
        "engine",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "ENGINE_PLATFORM_WIN",
        }

    filter "configurations:Debug"
        defines "ENGINE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ENGINE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ENGINE_DIST"
        runtime "Release"
        optimize "on"
