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
			(basepath.empty() ? "" : basepath + "/") + "Assets/"
		).make_preferred().string();
	}


	bool Open(const std::string& arcname, bool create)
	{
		LOG_CORE_DEBUG("Opening archive: \"{}\".", arcname);
		return true;
	}


	bool Close(const std::string& arcname)
	{
		LOG_CORE_DEBUG("Closing archive: \"{}\".", arcname);
		return true;
	}


	std::vector<char> Load(const std::string& filename, const std::string& arcname)
	{
		LOG_CORE_DEBUG("Loading file \"{}\" from archive \"{}\".", filename, arcname);

		if (arcname.empty())
			return LoadRealFile(filename);
		else
			return LoadRealFile(arcname + "/" + filename);
	}


    std::vector<char> LoadRealFile(const std::string& filename)
    {
		// Open file
		std::string filepath = std::filesystem::path(s_BasePath + filename)
			.make_preferred().string();
		LOG_CORE_DEBUG("Loading real file \"{}\".", filepath);
		std::ifstream file;
		file.open(filepath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			LOG_CORE_ASSERT(0, "Failed to open real file: \"" + filename + "\"");
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
