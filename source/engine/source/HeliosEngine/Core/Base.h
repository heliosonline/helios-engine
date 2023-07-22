#pragma once


// Version
#include "HeliosEngine/HeliosVersion.h"


// Configuration
#include "HeliosEngine/HeliosEngineConfig.h"


// Platform
#include "Platform/PlatformDetection.h"
#if defined(TARGET_PLATFORM_WINDOWS)
#	include "Platform/System/Windows/WinBase.h"
#elif defined(TARGET_PLATFORM_LINUX)
#	include "Platform/System/Linux/LinuxBase.h"
#elif defined(TARGET_PLATFORM_MACOS)
#	include "Platform/System/MacOS/MacOSBase.h"
#endif


// Standart C/C++
#include <algorithm>
#include <utility>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

#include <fstream>
#include <iostream>
#include <sstream>

#include <array>
#include <string>

#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>


// Engine Misc
#include "HeliosEngine/Core/Util.h"
#include "HeliosEngine/Core/Log.h"
#include "HeliosEngine/Debug/Instrumentor.h"


// Libraries (GLM)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>


// Libraries (EnTT)
#include <entt/entt.hpp>


// Libraries (other)
//#include <Common/Misc.h>
