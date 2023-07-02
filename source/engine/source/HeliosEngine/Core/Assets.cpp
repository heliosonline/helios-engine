#include "pch.h"

#include "HeliosEngine/Core/Assets.h"


namespace Helios::Assets {


	#define MAGIC "HeliosArc\00\00\00\00\00\00"
	struct ArcHeader
	{
		char magic[16] = MAGIC;
		int32_t numFiles;       // Number of files in archive
		int32_t posFirstFile;   // Absolute pos of the first file header (0 if none)
	};


	struct FileHeader
	{
		char name[256];  // Name of the file
		int32_t size;    // Size of the file in bytes
		int32_t posData; // Absolute pos of the file data
		int32_t posNext; // Absolute pos of the next file header (0 if none)
	};


	std::vector<char> LoadRealFile(const std::string& filename);


	std::string s_BasePath;


	void Init(const std::string& basepath)
	{
		s_BasePath = std::filesystem::path(
			(basepath.empty() ? "" : basepath + "/")
		).make_preferred().string();
	}


	std::vector<char> Load(const std::string& filename, const std::string& arcname)
	{
		return LoadRealFile(filename);
	}


    std::vector<char> LoadRealFile(const std::string& filename)
    {
		// Open file
		std::string filepath(s_BasePath + filename);
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			LOG_CORE_ASSERT("Failed to open file \"{}\"", filename);
		}

		// Create buffer with target size
		size_t filesize(static_cast<size_t>(file.tellg()));
		std::vector<char> buffer(filesize);

		// Read file
		file.seekg(0);
		file.read(buffer.data(), filesize);

		// Close file
		file.close();

        return buffer;
    }

} // namespace Helios::Asset
