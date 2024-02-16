workspace "Project"
	architecture "x32"
	
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
		"%{prj.name}/**.c",
		"%{prj.name}/**.inl",
		"%{prj.name}/vendor/imgui-1.51/**.cpp",
		"%{prj.name}/vendor/imgui-1.51/**.h"
	}
	
	includedirs
	{
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/stb_image/include",
		"%{prj.name}/vendor/tiny_obj/include",
		"%{prj.name}/vendor/glew/include",
		"%{prj.name}/vendor/glfw/include",
		"%{prj.name}/vendor/imgui-1.51",
		"%{prj.name}/include"
	}

project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	links { "Shard" }
	
	targetdir ("binn/" .. outputdir .. "/%{prj.name}")
	objdir ("binn-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.c",
		"%{prj.name}/**.inl",
		"%{prj.name}/vendor/imgui-1.51/**.cpp",
		"%{prj.name}/vendor/imgui-1.51/**.h"
	}
	
	includedirs
	{
		"%{wks.location}/Shard/vendor/glm",
		"%{wks.location}/Shard/vendor/stb_image/include",
		"%{wks.location}/Shard/vendor/tiny_obj/include",
		"%{wks.location}/Shard/vendor/glad/include",
		"%{wks.location}/Shard/vendor/glfw/include",
		"%{wks.location}/Shard/include",
		"%{wks.location}/Shard/vendor/imgui-1.51",
		"%{prj.name}/include"
	}

    libdirs
	{
		"%{wks.location}/Shard/vendor/glfw/lib-vc2022"
	}
	
	links
	{
		"glfw3_mt.lib",
		"opengl32.lib"
	}

    dependson
    {
		"opengl32.lib"
    }
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
		
		postbuildcommands
		{
		}
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		optimize "Off"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
