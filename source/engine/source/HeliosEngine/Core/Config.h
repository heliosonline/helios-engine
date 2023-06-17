#pragma once


namespace Helios {


	class  Config
	{
	public:
		static bool Init(const std::string file, const std::string path);
		static bool Read();
		static bool Update();

		static std::unordered_map<std::string, std::string> Data;
	};


} // namespace Helios
