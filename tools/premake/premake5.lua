local path = require "./path"
PROJECT_NAME = path.getProjectName( 2 )

workspace (PROJECT_NAME)
	configurations { "Debug", "Release", "Package" }
	platforms { "x64", "x86" }
	location "../../"
	startproject (PROJECT_NAME)

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "DEBUG", "DEBUG_SHADER" }
		symbols "On"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "Speed"
		symbols "On"
		flags { "LinkTimeOptimization" }

	filter{"configurations:Package"}
		defines { "RELEASE", "PACKAGE" }
		optimize "Full"
		symbols "Off"
		flags { "LinkTimeOptimization" }
	
	filter{}

group "framework"

include "../../libs/glfw.lua"
include "../../libs/glad.lua"
include "../../libs/glm.lua"
include "../../libs/imgui.lua"

group ""

project (PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir "../../bin"
	objdir "../../bin/obj/"
	
	location "../../build"

	includedirs { 
		"../../src/",
		"../../include/", 
		"../../libs/glad/include/", 
		"../../libs/glfw/include/", 
		"../../libs/glm/", 
		"../../libs/imgui/", 
		"../../libs/imgui/examples" 
	}

	files { 
		"../../src/**.h", 
		"../../src/**.cpp" 
	}

	links { "GLFW", "GLM", "GLAD", "ImGui" }

	filter "system:linux"
		links { "dl", "pthread" }

		defines { "_X11" }

	filter "system:windows"
		defines { "_WINDOWS" }
