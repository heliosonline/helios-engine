#pragma once

namespace Helios {
	enum VersionType
	{
		Unknown = 0,
		Debug,
//		Alpha,
//		Beta,
//		ReleaseCandidate,
		Release
	};
}


// This will create an uint32_t with the spezified version
//  -  3 bits: Helios::VersionType
//  -  7 bits: major version
//  - 10 bits: minor version
//  - 12 bits: patch number
#ifdef BUILD_DEBUG
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::VersionType::Debug)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#else
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::VersionType::Release)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#endif


#define HE_VERSION_TYPE(version)  ((uint32_t)(version) >> 29U)
#define HE_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
#define HE_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define HE_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)


#define HE_VERSION_TYPE_STRING(version) (\
	(HE_VERSION_TYPE(version) == Helios::VersionType::Debug) ? "Debug" : \
	(HE_VERSION_TYPE(version) == Helios::VersionType::Release) ? "Release" : \
	"Unknown")


// Version of the HeliosEngine
#define HE_VERSION HE_MAKE_VERSION(0, 1, 0)
