#pragma once


namespace Helios::Assets {

	extern void Init(const std::string& basepath);

	extern bool Open(const std::string& arcname, bool create = false);
	extern bool Close(const std::string& arcname);

	extern std::vector<char> Load(const std::string& filename, const std::string& arcname = "");
//	extern bool Exist(const std::string& filename, const std::string& arcname = "");

//	extern bool Add(const std::string& filename, char* data, const std::string& arcname);
//	extern bool Remove(const std::string& filename, const std::string& arcname);


} // namespace Helios::Asset
