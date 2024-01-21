workspace "Project"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

SHARD_DLL_SRC = ""
SHARD_DLL_DST = ""

project "Shard"
	location "Shard"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/vendor/SDL2-2.28.5/include"
	}
	
	libdirs
	{
		"%{prj.name}/vendor/SDL2-2.28.5/lib/x64"
	}

	links
	{
		"SDL2.lib"
	}
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
	
	filter "configurations:Debug"
		symbols "On"
		optimize "Off"
	
	filter "configurations:Release"
		symbols "Off"
		optimize "On"

project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}
	
	includedirs
	{
		"Shard"
	}
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
		
		postbuildcommands
		{
			
			("{COPY} ../bin/" .. outputdir .. "/Shard/Shard.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}")
		}

    links
    {
        "Shard"
    }

    dependson
    {
        "Shard"
    }
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		optimize "Off"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"