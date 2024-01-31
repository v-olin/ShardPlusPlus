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

	-- This project will not be built
	-- targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	-- objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.inl"
	}
	
	includedirs
	{
		-- I have swapped prj.name to wks.location because
		-- VS is dumb as shit and can't read
		"%{wks.location}/Shard/vendor/SDL2-2.28.5/include",
		"%{wks.location}/Shard/vendor/SDL2_ttf-2.22.0/include",
		"%{wks.location}/Shard/vendor/SDL2_image-2.8.2/include",
		"%{wks.location}/Shard/vendor/glm",
		"%{wks.location}/Shard/vendor/stb_image/include",
		"%{wks.location}/Shard/vendor/tiny_obj/include",
		"%{wks.location}/Shard/vendor/glew/include",
		"%{wks.location}/Shard/include"
	}
	
	-- No need for binaries, will not be built
	-- libdirs
	-- {
	-- 	"%{prj.name}/vendor/SDL2-2.28.5/lib/x64",
	-- 	"%{prj.name}/vendor/SDL2_ttf-2.22.0/lib/x64",
	-- 	"%{prj.name}/vendor/SDL2_image-2.8.2/lib/x64",
	-- 	-- "%{prj.name}/vendor/GLFW/lib-vc2022",
	-- 	"%{prj.name}/vendor/glew/lib"
	-- }
	
	-- links
	-- {
	-- 	"SDL2.lib",
	-- 	"SDL2_ttf.lib",
	-- 	"SDL2_image.lib",
	-- 	"opengl32.lib",
	-- 	"glew32.lib",
	-- 	-- "glfw3.lib",
	-- 	"glu32.lib"
	-- }
	
	-- Will not be built
	-- filter "system:windows"
	-- 	cppdialect "C++20"
	-- 	staticruntime "On"
	-- 	systemversion "latest"
	
	-- filter "configurations:Debug"
	-- 	symbols "On"
	-- 	optimize "Off"
	
	-- filter "configurations:Release"
	-- 	symbols "Off"
	-- 	optimize "On"

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
		-- Not sure if these below are necessary (from glm/inc)
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
		-- glew > glfw?
		-- "%{prj.name}/vendor/GLFW/include/GLFW",
		-- include all files from Shared Items Project
		"%{wks.location}/Shard/include",
		"%{prj.name}/include"
	}
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
		
		postbuildcommands
		{
			-- No Shard.dll to copy
			-- ("{COPY} ../bin/" .. outputdir .. "/Shard/Shard.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}")
			-- Inherited from old Shard dll project
            ("{COPY} ../bin/" .. outputdir .. "/Shard/SDL2.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
            ("{COPY} ../bin/" .. outputdir .. "/Shard/SDL2_ttf.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
            ("{COPY} ../bin/" .. outputdir .. "/Shard/SDL2_image.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}"),
            ("{COPY} ../bin/" .. outputdir .. "/Shard/glew32.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}")
		}

	-- Now Game has to depend on binaries
	-- since they are not compiled into Shard.dll
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
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		optimize "Off"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
