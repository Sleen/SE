#pragma once


#define PLATFORM_UNKNOWN	0
#define PLATFORM_WINDOWS	1
#define PLATFORM_ANDROID	2
#define PLATFORM_LINUX		3


#if defined(_WIN32)
#	ifndef WINDOWS
#		define WINDOWS
#	endif
#	define PLATFORM PLATFORM_WINDOWS
#elif defined(ANDROID_NDK)
#	ifndef ANDROID
#		define ANDROID
#	endif
#	define PLATFORM PLATFORM_ANDROID
#elif defined(__LINUX__) || defined(__linux__)
#	ifndef LINUX
#		define LINUX
#	endif
#	define PLATFORM PLATFORM_LINUX 
#else
#	define PLATFORM PLATFORM_UNKNOWN
#endif

#ifdef WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

#if defined(WINDOWS) || defined(LINUX)
#	define GLEW_STATIC
#	include "GL/glew.h"
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#	pragma comment(lib, "glew.lib")
#endif

#ifdef ANDROID
#	include <GLES2/gl2.h>
#	include <jni.h>
#	include <errno.h>
#	include <EGL/egl.h>
#	include <GLES/gl.h>
#	include <android/sensor.h>
#	include <android/log.h>
#	include <android/window.h>
#	include <android/asset_manager.h>
#	include <android_native_app_glue.h>
#	include <NDKHelper.h>
#	ifndef LOGI
#		define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#	endif
#	ifndef LOGW
#		define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#	endif

	using namespace ndkHelper;

#elif defined(LINUX)
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#	include <X11/keysym.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#ifdef WINDOWS
#	include <direct.h>
#	include <io.h>
#else
#	include <unistd.h>
#endif

#ifdef WINDOWS
#	include "GL/wglew.h"
#	ifdef _MSC_VER
#		pragma warning(disable : 4309)
#		pragma warning(disable : 4305)
#		pragma warning(disable : 4996)
#		if _MSC_VER >= 1600
#			include <stdint.h>
#		else
			typedef __int8              int8_t;
			typedef __int16             int16_t;
			typedef __int32             int32_t;
			typedef __int64             int64_t;
			typedef unsigned __int8     uint8_t;
			typedef unsigned __int16    uint16_t;
			typedef unsigned __int32    uint32_t;
			typedef unsigned __int64    uint64_t;
#			define INT8_MIN            _I8_MIN
#			define INT8_MAX            _I8_MAX
#			define INT16_MIN           _I16_MIN
#			define INT16_MAX           _I16_MAX
#			define INT32_MIN           _I32_MIN
#			define INT32_MAX           _I32_MAX
#			define INT64_MIN           _I64_MIN
#			define INT64_MAX           _I64_MAX
#			define UINT8_MAX           _UI8_MAX
#			define UINT16_MAX          _UI16_MAX
#			define UINT32_MAX          _UI32_MAX
#			define UINT64_MAX          _UI64_MAX
#		endif
#	endif		// _MSC_VER
#	if defined(SE_CORE_DLL_EXPORT)
#		define SE_CORE __declspec(dllexport)
#	elif defined(SE_CORE_DLL_IMPORT)
#		define SE_CORE __declspec(dllimport)
#	else
#		define WM5_CORE_ITEM
#	endif
#elif defined(ANDROID)
#	ifndef GLES
#		define GLES
#	endif
#endif

// standard headers
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <utility>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>

using namespace std;

// math
#ifndef MIN
#	define MIN(a, b) ((a)<(b) ? (a) : (b))
#endif
#ifndef MAX
#	define MAX(a, b) ((a)>(b) ? (a) : (b))
#endif
#ifndef M_PI
#	define M_PI	3.1415926535898
#endif
#define DEG_TO_RAD 0.01745329252
#define FIXED(x) (x * 0x10000)

// namespace
#define SE_BEGIN 	namespace SE{
#define SE_END		}
#define USING_SE		using namespace SE;

// debug
#ifdef _DEBUG
#	define ASSERT_ENABLED
#	ifdef ANDROID
#		define DEBUG_LOG(...) LOGI(__VA_ARGS__)
#	else
#		define DEBUG_LOG(...) printf(__VA_ARGS__)
#	endif
#else
#	define DEBUG_LOG(...) 
#endif

#define DEBUG_MESSAGE_ON_DIALOG
#ifdef DEBUG_MESSAGE_ON_DIALOG
#	ifdef WINDOWS
#		define DEBUG_MESSAGE(msg) MessageBox(NULL, msg, "Debug Message", 0)
#	else
#		define DEBUG_MESSAGE(msg)	cout<<msg<<endl;
#	endif
#else
#	define DEBUG_MESSAGE(msg)	cout<<msg<<endl;
#endif

template<typename T>
inline string toStr(T t){
	ostringstream s;
	s << t;
	return s.str();
}

//#define debug_break	_asm int 3
#define debug_break	exit(0);
#ifdef ASSERT_ENABLED
#ifdef WINDOWS
#	define ASSERT(exp, msg)	\
		if (exp){} \
		else{ \
			DEBUG_MESSAGE((string("Assertion failure ! \n\nExpression: \t") + #exp + "\nMessage: \t" + msg + "\n\nFile: \t" + __FILE__ + "\nFunction: \t" + __FUNCTION__ + "\nLine: \t" + toStr(__LINE__)).c_str()); \
			debug_break \
		}
#elif defined(LINUX)
#	include <assert.h>
#	define ASSERT(exp, msg)	\
		if (exp){} \
		else{ \
			DEBUG_MESSAGE((string("Assertion failure ! \n\nExpression: \t") + #exp + "\nMessage: \t" + msg + "\n\nFile: \t" + __FILE__ + "\nFunction: \t" + __FUNCTION__ + "\nLine: \t" + toStr(__LINE__)).c_str()); \
			assert(exp); \
		}
#else
#	define ASSERT(exp, msg) exp
#endif
#else
#	define ASSERT(exp, msg) exp
#endif
