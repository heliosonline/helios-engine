#include "pch.h"

#include "HeliosEngine/Core/Config.h"


namespace Helios {


	static std::string s_ConfigFile;
	std::unordered_map<std::string, std::string> Config::Data = {};


	bool Config::Init(const std::string filename, const std::string path)
	{
		s_ConfigFile = std::filesystem::path(
			(path.empty() ? "" : path + "/") + (filename.empty() ? "config.cfg" : filename)
		).make_preferred().string();

		return Read();
	}


	bool Config::Read()
	{
		std::ifstream file;
		std::string line;

		file.open(s_ConfigFile, std::ifstream::in);
		if (file.is_open())
		{
			while (!file.eof())
			{
				std::getline(file, line);
				if (line.length() > 0)
				{
					std::string key = line.substr(0, line.find_first_of('='));
					std::string value = line.substr(line.find_first_of('=') + 1);

					if (key.length() > 0 and value.length() > 0 and line.find_first_of('=') != std::string::npos)
						Data[key] = value;
				}
			}
			file.close();
		}

		return true;
	}


	bool Config::Update()
	{
		std::ofstream file(s_ConfigFile, std::ios::out | std::ios::trunc);
		if (file.is_open())
		{
			for (auto& kv : Data)
				file << kv.first << '=' << kv.second << '\n';
			file.close();

			return true;
		}

		return false;
	}


	std::string Config::Get(const std::string& key, const std::string& default_value)
	{
		if (Data.contains(key))
			return Data[key];
		else
			return default_value;
	}


} // namespace Helios
