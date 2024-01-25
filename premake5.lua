workspace "Project"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release"
	}

-- cfg.buildcfg: e.g. debug, release
-- cfg.system: e.g. windows
-- cfg.architecture: e.g. x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Shard"
	location "Shard"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.inl"
	}
	
	includedirs
	{
		"%{prj.name}/vendor/SDL2-2.28.5/include",
		"%{prj.name}/vendor/SDL2_ttf-2.22.0/include",
		"%{prj.name}/vendor/SDL2_image-2.8.2/include",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/include"
	}
	
	libdirs
	{
		"%{prj.name}/vendor/SDL2-2.28.5/lib/x64",
		"%{prj.name}/vendor/SDL2_ttf-2.22.0/lib/x64",
		"%{prj.name}/vendor/SDL2_image-2.8.2/lib/x64"
		
	}

	links
	{
		"SDL2.lib",
		"SDL2_ttf.lib",
		"SDL2_image.lib"
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
		"Shard",
		"Shard/include"
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
