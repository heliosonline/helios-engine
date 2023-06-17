-------------------------------------------------------------------------------
--  Custom Libraries
-------------------------------------------------------------------------------


--function LibCommon()
--	includedirs "%{wks.location}/Source/Libs/LibCommon/Source"
--	links "LibCommon"
--end


--function LibNet()
--	includedirs "%{wks.location}/Source/Libs/LibNet/Source"
--	links "LibNet"
--
--	VendorAsio {}
--end


--function LibLuaHelper()
--	includedirs "%{wks.location}/Source/Libs/LibLuaHelper/Source"
--end


function LibHeliosEngine()
	-- libraries
--	LibLuaHelper{}
--	VendorGlad{}
	VendorGlfw{}
--	VendorGlm{}
--	VendorImgui{}
--	VendorLua{}
	VendorSpdlog{}
--	VendorStb{}

	-- the engine itself
	includedirs "%{wks.location}/source/engine/source"
	links "helios.engine"
end


-------------------------------------------------------------------------------
--  External Dependancies
-------------------------------------------------------------------------------


function VendorVulkan()
	if os.getenv("VULKAN_SDK") then
		includedirs "%VULKAN_SDK%/Include"
		libdirs "%VULKAN_SDK%/Lib"
		links "vulkan-1"

		defines "BUILDWITH_RENDERER_VULKAN"
		files {
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
		}
	end
end


function VendorOpenGL()
--	defines "BUILDWITH_RENDERER_OPENGL"
--	files {
--		"source/Platform/Renderer/OpenGL/**.h",
--		"source/Platform/Renderer/OpenGL/**.cpp",
--	}
end


function VendorMetal()
--	defines "BUILDWITH_RENDERER_METAL"
--	files {
--		"source/Platform/Renderer/Metal/**.h",
--		"source/Platform/Renderer/Metal/**.cpp",
--	}
end


function VendorDirectX()
--	defines "BUILDWITH_RENDERER_DIRECTX"
--	files {
--		"source/Platform/Renderer/DirectX/**.h",
--		"source/Platform/Renderer/DirectX/**.cpp",
--	}
end


--function VendorAsio()
--	includedirs "%{wks.location}/Source/Vendor/asio/asio-1.24.0/include"
--end


--function VendorFreetype()
--	includedirs "%{wks.location}/Source/Vendor/freetype/include"
--	links "Vendor.freetype"
--end


--function VendorGlad()
--	includedirs "%{wks.location}/Source/Vendor/glad/include"
--	links "Vendor.glad"
--end


function VendorGlfw()
	includedirs "%{wks.location}/vendor/glfw/include"
	defines "GLFW_INCLUDE_NONE"
	links "vendor.glfw"
end


--function VendorGlm()
--	includedirs "%{wks.location}/Source/Vendor/glm"
--end


--function VendorImgui()
--	includedirs "%{wks.location}/Source/Vendor/imgui"
--	links "Vendor.imgui"
--end


--function VendorJson()
--	includedirs "%{wks.location}/Source/Vendor/json/single_include/nlohmann"
--end


--function VendorLua()
--	includedirs "%{wks.location}/Source/Vendor/lua/lua-5.4.4"
--	links "Vendor.lua"
--end


--function VendorMsdfAtlas()
--	includedirs "%{wks.location}/Source/Vendor/msdf-atlas"
--	links "Vendor.msdf-atlas"
--
--	VendorFreetype{}
--	VendorMsdfGen{}
--end


--function VendorMsdfGen()
--	includedirs "%{wks.location}/Source/Vendor/msdf-gen"
--	links "Vendor.msdf-gen"
--end


function VendorSpdlog()
	includedirs "%{wks.location}/vendor/spdlog/include"
end


--function VendorStb()
--	includedirs "%{wks.location}/Source/Vendor/stb"
--	links "Vendor.stb"
--end
