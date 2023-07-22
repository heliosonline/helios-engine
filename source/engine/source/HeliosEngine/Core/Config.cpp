#include "pch.h"
#include "Config.h"


namespace Helios {


	static std::string s_ConfigFile;
	static bool s_Changed = false;

	std::unordered_map<std::string, std::string> Config::Data = {};


	bool Config::Init(const std::string filename, const std::string basepath)
	{
		s_ConfigFile = std::filesystem::path(
			(basepath.empty() ? "" : basepath + "/") + (filename.empty() ? "config.cfg" : filename)
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


	bool Config::Save()
	{
		if (!s_Changed)
			return true;

		std::ofstream file(s_ConfigFile, std::ios::out | std::ios::trunc);
		if (file.is_open())
		{
			for (auto& kv : Data)
				file << kv.first << '=' << kv.second << '\n';
			file.close();

			s_Changed = false;
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


	void Config::Set(const std::string& key, const std::string& value)
	{
		Data[key] = value;
		s_Changed = true;
	}


} // namespace Helios
