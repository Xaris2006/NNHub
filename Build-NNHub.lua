-- premake5.lua
workspace "NNHub"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "NNHub"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Vendor"
   include "vendor/Web"
group ""

include "Build-Walnut-External.lua"
include "NNHub/Build-NNHub-App.lua"