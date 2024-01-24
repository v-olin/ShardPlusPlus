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
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/stb_image/include",
		"%{prj.name}/vendor/tiny_obj/include",
		"%{prj.name}/vendor/glew/include",
		"%{prj.name}/include"
	}
	
	libdirs
	{
		"%{prj.name}/vendor/SDL2-2.28.5/lib/x64",
		"%{prj.name}/vendor/glew/lib"
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
