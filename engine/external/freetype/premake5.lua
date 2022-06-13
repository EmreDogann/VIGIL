project "freetype"
  kind "StaticLib"
  language "C++"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("inter/" .. outputdir .. "/%{prj.name}")

  files {
    "src/autofit/autofit.c",
    "src/bdf/bdf.c",
    "src/cff/cff.c",
    "src/base/ftbase.c",
    "src/base/ftbitmap.c",
    "win32/ftdebug.c",
    "src/base/ftfstype.c",
    "src/base/ftgasp.c",
    "src/base/ftglyph.c",
    "src/gzip/ftgzip.c",
    "src/base/ftinit.c",
    "src/lzw/ftlzw.c",
    "src/base/ftstroke.c",
    "src/base/ftsystem.c",
    "src/smooth/smooth.c",
    "src/base/ftbbox.c",
    "src/base/ftmm.c",
    "src/base/ftpfr.c",
    "src/base/ftsynth.c",
    "src/base/fttype1.c",
    "src/base/ftwinfnt.c",
    "src/base/ftxf86.c",
    "src/base/ftlcdfil.c",
    "src/base/ftgxval.c",
    "src/base/ftotval.c",
    "src/base/ftpatent.c",
    "src/pcf/pcf.c",
    "src/pfr/pfr.c",
    "src/psaux/psaux.c",
    "src/pshinter/pshinter.c",
    "src/psnames/psmodule.c",
    "src/raster/raster.c",
    "src/sfnt/sfnt.c",
    "src/truetype/truetype.c",
    "src/type1/type1.c",
    "src/cid/type1cid.c",
    "src/type42/type42.c",
    "src/winfonts/winfnt.c",
    "include/ft2build.h",
    "include/ft2build.h",
    "include/freetype/config/ftconfig.h",
    "include/freetype/config/ftheader.h",
    "include/freetype/config/ftmodule.h",
    "include/freetype/config/ftoption.h",
    "include/freetype/config/ftstdlib.h",
  }
  
  includedirs{
      "include/",
  }
  defines{
    "FT2_BUILD_LIBRARY",
    "DLL_EXPORT"
  }

  filter "system:windows"
    systemversion "latest"
    cppdialect "C++17"
    staticruntime "On"
    
    defines{
      "WIN32",
      "_LIB",
    }

  filter "configurations:Debug"
      defines "PYRO_DEBUG"
      runtime "Debug"
      symbols "on"
    
      defines{
        "_DEBUG",
        "_CRT_SECURE_NO_WARNINGS",
        "FT_DEBUG_LEVEL_ERROR",
        "FT_DEBUG_LEVEL_TRACE",
        "_CRT_SECURE_NO_DEPRECATE",
      }

  filter "configurations:Release"
      defines "PYRO_RELEASE"
      runtime "Release"
      optimize "on"
    
      defines{
        "NDEBUG",
        "_CRT_SECURE_NO_WARNINGS",
      }

  filter "configurations:Dist"
      defines "PYRO_DIST"
      runtime "Release"
      optimize "on"
    
      defines{
        "NDEBUG",
        "_CRT_SECURE_NO_WARNINGS",
      }
