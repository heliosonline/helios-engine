-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "helios.engine"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	kind          "StaticLib"

	targetdir (dir_bin   .. dir_group .. dir_config)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "source/pch.cpp"

	-- Libraries
	VendorGlfw{}
	VendorSpdlog{}


	includedirs {
		"source",
	}


	files {
		-- precompiled header
		"source/pch.h",
		"source/pch.cpp",
		-- project itself
		"source/HeliosEngine/**.h",
		"source/HeliosEngine/**.cpp",
		-- dedicated platform detection
		"source/Platform/PlatformDetection.h",
	}


	filter "configurations:Debug"

		defines {
		}


	filter "configurations:Release"

		defines {
		}


	filter "platforms:Windows"

		defines {
			--"BUILDWITH_RENDERER_DIRECTX",
			--"BUILDWITH_RENDERER_METAL",
			--"BUILDWITH_RENDERER_OPENGL",
			"BUILDWITH_RENDERER_VULKAN",
		}

		files {
			"source/Platform/System/Windows/**.h",
			"source/Platform/System/Windows/**.cpp",
			"source/Platform/Renderer/DirectX/**.h",
			"source/Platform/Renderer/DirectX/**.cpp",
			--"source/Platform/Renderer/Metal/**.h",
			--"source/Platform/Renderer/Metal/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
		}


	filter "platforms:Linux"

		defines {
			--"BUILDWITH_RENDERER_DIRECTX",
			--"BUILDWITH_RENDERER_METAL",
			"BUILDWITH_RENDERER_OPENGL",
			"BUILDWITH_RENDERER_VULKAN",
		}

		files {
			"source/Platform/System/Linux/**.h",
			"source/Platform/System/Linux/**.cpp",
			--"source/Platform/Renderer/DirectX/**.h",
			--"source/Platform/Renderer/DirectX/**.cpp",
			--"source/Platform/Renderer/Metal/**.h",
			--"source/Platform/Renderer/Metal/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
		}


	filter "platforms:MacOS"

		defines {
			--"BUILDWITH_RENDERER_DIRECTX",
			"BUILDWITH_RENDERER_METAL",
			"BUILDWITH_RENDERER_OPENGL",
			"BUILDWITH_RENDERER_VULKAN",
		}

		files {
			"source/Platform/System/MacOS/**.h",
			"source/Platform/System/MacOS/**.cpp",
			--"source/Platform/Renderer/DirectX/**.h",
			--"source/Platform/Renderer/DirectX/**.cpp",
			"source/Platform/Renderer/Metal/**.h",
			"source/Platform/Renderer/Metal/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
		}


	filter {}



--	prebuildmessage "Updating version information..."
--	prebuildcommands {
--		"\"%{wks.location}Tools/build_inc/_bin/build_inc_" .. os.host() .. "\" -bfile \"%{prj.location}Source/Config/Version.h\" -bdef VERSION_BUILD"
--	}
--
--
--	postbuildmessage "Copying assets to the target folder..."
--	postbuildcommands {
--		"{COPYDIR} %{wks.location}Source/Client/GameEngine/Assets " .. dir_bin .. dir_group .. dir_config .. "Assets",
--		"{COPYDIR} %{prj.location}Assets " .. dir_bin .. dir_group .. dir_config .. "Assets"
--	}
