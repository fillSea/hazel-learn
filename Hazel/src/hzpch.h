#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Hazel/debug/Instrumentor.h"
#include "hazel/core/Log.h"

#ifdef HZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif