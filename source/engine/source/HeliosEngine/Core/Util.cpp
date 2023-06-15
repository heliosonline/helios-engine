#include "pch.h"

#include "HeliosEngine/Core/Util.h"


namespace Helios {


	std::string GetExePath()
	{
		std::string path;

		#if defined TARGET_PLATFORM_WINDOWS

			char ctemp[MAX_PATH];
			wchar_t wtemp[MAX_PATH];
			GetModuleFileName(NULL, wtemp, MAX_PATH);
			wcstombs_s(nullptr, ctemp, wtemp, MAX_PATH);

			std::string strtemp(ctemp);
			path = strtemp.substr(0, strtemp.find_last_of("/\\"));

		#elif defined TARGET_PLATFORM_LINUX

			// TODO... test GetExePath() for Linux
			char result[PATH_MAX];
			ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
			return std::string(result, (count > 0) ? count : 0);

		#elif defined TARGET_PLATFORM_MACOS

			// TODO... implement GetExePath() for MacOS
			// TODO... test GetExePath() for MacOS

			// see: _NSGetExecutablePath()
			// https://stackoverflow.com/questions/799679/programatically-retrieving-the-absolute-path-of-an-os-x-command-line-app/1024933#1024933
			#pragma error "Not implemented!"

		#endif

		return path;
	}

} // namespace Helios

