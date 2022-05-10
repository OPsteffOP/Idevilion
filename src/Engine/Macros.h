#pragma once

#define DEFINE_ENUM_FLAGS(name, ...)																		\
enum class name { __VA_ARGS__ };																			\
constexpr inline name operator|(name value1, name value2) { return name(int(value1) | int(value2)); }		\
constexpr inline bool operator&(name value1, name value2) { return int(value1) & int(value2); }				\
constexpr inline name& operator|=(name& value1, name value2) { return value1 = value1 | value2; }

#define DEFINE_ENUM(name, ...)																				\
enum class name { __VA_ARGS__, _COUNT }

#define LOG_DEBUG(format, ...) { printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_WARNING(format, ...) { printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_ERROR(format, ...) { printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_CRITICAL(format, ...) { printf(format, ##__VA_ARGS__); printf("\n"); }
#define DEBUG_ASSERT(condition, format, ...) if(!(condition)) LOG_CRITICAL(format, ##__VA_ARGS__)

#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#define SAFE_DELETE_ARRAY(ptr) delete[] ptr; ptr = nullptr

#define SAFE_RELEASE(ptr) if(ptr != nullptr) { ptr->Release(); ptr = nullptr; }

#ifdef BP_CONFIGURATION_DEBUG
#define DEBUG_ONLY(instruction) instruction
#else
#define DEBUG_ONLY(instruction) (void*)nullptr
#endif

#ifdef _MSC_VER
#define PACK(value, decl) __pragma( pack(push, value) ) decl __pragma( pack(pop))
#endif

#define DEV_BUILD
#define DEV_DEBUG_BUILD

#define INCLUDE_BAKING_CODE
#define ENABLE_LEAK_DETECTION

#define USE_TEXTURE_WIDTH -1.f
#define USE_TEXTURE_HEIGHT -1.f

#define UNREFERENCED_PARAMETER(P) (P)

#ifdef BP_MODULE_EXPORT
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif