#pragma once

#include "Macro.h"

#ifdef WINDOWS
#	include <VersionHelpers.h>
#endif

SE_BEGIN

class System{
public:
	static int Major;
	static int Minor;

	static void Init(){
#ifdef _WIN32
#define VER(major, minor) Major = major; Minor = minor
		if (IsWindows8Point1OrGreater()){
			VER(8, 1);
		}
		else if (IsWindows8OrGreater()){
			VER(8, 0);
		}
		else if (IsWindows7SP1OrGreater()){
			VER(7, 1);
		}
		else if (IsWindows7OrGreater()){
			VER(7, 0);
		}
		else if (IsWindowsVistaSP2OrGreater()){
			VER(6, 2);
		}
		else if (IsWindowsVistaSP1OrGreater()){
			VER(6, 1);
		}
		else if (IsWindowsVistaOrGreater()){
			VER(6, 0);
		}
		else if (IsWindowsXPSP3OrGreater()){
			VER(5, 3);
		}
		else if (IsWindowsXPSP2OrGreater()){
			VER(5, 2);
		}
		else if (IsWindowsXPSP1OrGreater()){
			VER(5, 1);
		}
		else if (IsWindowsXPOrGreater()){
			VER(5, 0);
		}
		else{
			VER(0, 0);
		}
#endif
	}
};

SE_END