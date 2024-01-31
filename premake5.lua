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
	kind "SharedItems"
	language "C++"

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
		"%{prj.name}/vendor/stb_image/include",
		"%{prj.name}/vendor/tiny_obj/include",
		"%{prj.name}/vendor/glew/include",
		"%{prj.name}/include"
	}

project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	links { "Shard" }
	
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
		"%{wks.location}/Shard/vendor/SDL2-2.28.5/include",
		"%{wks.location}/Shard/vendor/SDL2_ttf-2.22.0/include",
		"%{wks.location}/Shard/vendor/SDL2_image-2.8.2/include",
		"%{wks.location}/Shard/vendor/glm",
		"%{wks.location}/Shard/vendor/stb_image/include",
		"%{wks.location}/Shard/vendor/tiny_obj/include",
		"%{wks.location}/Shard/vendor/glew/include",
		"%{wks.location}/Shard/include",
		"%{prj.name}/include"
	}

    libdirs
	{
		"%{wks.location}/Shard/vendor/SDL2-2.28.5/lib/x64",
		"%{wks.location}/Shard/vendor/SDL2_ttf-2.22.0/lib/x64",
		"%{wks.location}/Shard/vendor/SDL2_image-2.8.2/lib/x64",
		"%{wks.location}/Shard/vendor/glew/lib"
	}
	
	links
	{
		"SDL2.lib",
		"SDL2_ttf.lib",
		"SDL2_image.lib",
		"opengl32.lib",
		"glew32.lib",
		"glu32.lib"
	}

    dependson
    {
        "SDL2.lib",
		"SDL2_ttf.lib",
		"SDL2_image.lib",
		"opengl32.lib",
		"glew32.lib",
		"glu32.lib"
    }
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
		
		postbuildcommands
		{
			("{COPY} ../Shard/vendor/SDL2-2.28.5/lib/x64/SDL2.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
			("{COPY} ../Shard/vendor/SDL2_ttf-2.22.0/lib/x64/SDL2_ttf.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
			("{COPY} ../Shard/vendor/SDL2_image-2.8.2/lib/x64/SDL2_image.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
			("{COPY} ../Shard/vendor/glew/lib/glew32.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
		}
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		optimize "Off"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
