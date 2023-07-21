---------------------------------
-- [ WORKSPACE CONFIGURATION ] --
---------------------------------
workspace "Helios Engine"

	configurations {
		"Debug",
		"Release"
	}

	platforms {
		"Windows",
		"Linux",
		"MacOS"
	}
	
	startproject "helios.engine.sandbox"


	--------------------------------
	-- [ PLATFORM CONFIGURATION ] --
	--------------------------------
	filter "platforms:Windows"
		system  "windows"

	filter "platforms:Linux"
		system  "linux"

	filter "platforms:MacOS"
		system  "macosx"

	-- Windows/MSVC only
	filter { "system:windows", "action:vs*" }
		flags { "MultiProcessorCompile", "NoMinimalRebuild" }
		systemversion "latest"


	-------------------------------------
	-- [ DEBUG/RELEASE CONFIGURATION ] --
	-------------------------------------
	filter "configurations:Debug"
		defines {
			"BUILD_DEBUG",
			"_DEBUG",
		}
		symbols  "On"
		optimize "Off"
		runtime  "Debug"

	filter "configurations:Release"
		defines {
			"BUILD_RELEASE",
			"NDEBUG",
		}
		symbols  "Off"
		optimize "Off"
--		optimize "On"
--		optimize "Speed"
--		optimize "Full"
		runtime  "Release"

	filter {}


	-------------------------------
	-- [ LIBRARIES (UTIL-FUNC) ] --
	-------------------------------
	
	include("premake5_libs.lua")


	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	-------------------------------

	dir_bin     = "%{wks.location}/_bin/"
	dir_build   = "%{wks.location}/_build/"
	dir_config  = "%{string.lower(cfg.platform)}-%{string.lower(cfg.buildcfg)}/"
	dir_project = "%{string.lower(prj.name)}"

	group "helios"
		dir_group = "helios/"
		include("source/engine/")
		include("source/engine-sandbox/")

--	group "Client"
--		dir_group = "Client/"
--		include("Source/Client/")

--	group "Server"
--		dir_group = "Server/"
--		include("Source/Server/")

--	group "Libs"
--		dir_group = "Libs/"
--		include("Source/Libs/")

	group "vendor"
		dir_group = "vendor/"
		include("vendor/")

--	group "Misc"
--		dir_group = "Misc/"
--		include("Docu/")
--		include("Tools/")

	group ""

