#pragma once


namespace Helios {


	class  Config
	{
	public:
		static bool Init(const std::string file, const std::string path);
		static bool Read();
		static bool Save();

		static std::string Get(const std::string& key, const std::string& default_value = "");
		static void Set(const std::string& key, const std::string& value);

	public:
		static std::unordered_map<std::string, std::string> Data;
	};


} // namespace Helios
