#pragma once

#include "Typedefs.h"

// Rendering APIs
#ifdef BP_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <dxgi1_3.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#undef LoadImage
#undef DELETE
#undef CreateWindow
#undef DrawText
#elif BP_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#endif

// Networking APIs
#ifdef BP_PLATFORM_WINDOWS
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <optional>
#include <functional>
#include <filesystem>
#include <unordered_set>
#include <mutex>
#include <queue>

#include "Macros.h"
#include "Allocators.h"
#include "Utils.h"
#include "CommandLine.h"
#include "Paths.h"
#include "ThreadPool.h"