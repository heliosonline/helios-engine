#pragma once

// This will create an uint32_t with the spezified version
//  -  3 bits: reserved, set to 0
//  -  7 bits: major version
//  - 10 bits: minor version
//  - 12 bits: patch number
#define HE_MAKE_VERSION(reserved, major, minor, patch) \
    ((((uint32_t)(reserved)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define HE_VERSION_RESERVED(version) ((uint32_t)(version) >> 29U)
#define HE_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
#define HE_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define HE_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

// Version of the HeliosEngine
#define HE_VERSION HE_MAKE_VERSION(0, 0, 1, 0)
