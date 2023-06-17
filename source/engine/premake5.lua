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

		files {
			"source/Platform/System/Windows/**.h",
			"source/Platform/System/Windows/**.cpp",
		}
		VendorDirectX{}
		--VendorMetal{}
		VendorVulkan{}
		VendorOpenGL{}



	filter "platforms:Linux"

		files {
			"source/Platform/System/Linux/**.h",
			"source/Platform/System/Linux/**.cpp",
		}
		--VendorDirectX{}
		--VendorMetal{}
		VendorVulkan{}
		VendorOpenGL{}


	filter "platforms:MacOS"

		files {
			"source/Platform/System/MacOS/**.h",
			"source/Platform/System/MacOS/**.cpp",
		}
		--VendorDirectX{}
		VendorMetal{}
		VendorVulkan{}
		VendorOpenGL{}


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
