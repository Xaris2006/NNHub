project "NNHub"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "$(SolutionDir)Walnut/vendor/imgui",
      "$(SolutionDir)Walnut/vendor/implot",
      "$(SolutionDir)Walnut/vendor/glfw/include",

      "$(SolutionDir)Walnut/Walnut/Source",
      "$(SolutionDir)Walnut/Walnut/Platform/GUI",

      --"%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glmLS}",
      "%{IncludeDir.Arma}",
      "%{IncludeDir.Cuda}"
   }

    links
    {
        "Walnut",
        "%{Library.ArmaLib}",
        "%{Library.OpenBlas}"
    }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG", "ARMA_USE_BLAS", "ARMA_USE_LAPACK", "ARMA_USE_CUBLAS", "ARMA_EXTRA_DEBUG" }
      runtime "Debug"
      staticruntime "off"
      symbols "On"

   filter "configurations:Release"
      defines { "WL_RELEASE", "ARMA_USE_BLAS", "ARMA_USE_LAPACK", "ARMA_USE_CUBLAS", "ARMA_NO_DEBUG" }
      runtime "Release"
      staticruntime "off"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "WL_DIST", "ARMA_USE_BLAS", "ARMA_USE_LAPACK", "ARMA_USE_CUBLAS", "ARMA_NO_DEBUG" }
      runtime "Release"
      staticruntime "On"
      optimize "On"
      symbols "Off"