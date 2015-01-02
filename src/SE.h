#pragma once

#if defined(_MSC_VER) && !defined(_DEBUG)
#  pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

#include "Core.h"
#include "my_math.h"
#include "graphicsLib.h"
#include "App.h"
#include "Gui.h"

extern void SE_main();

#if PLATFORM == PLATFORM_ANDROID
void android_main(struct android_app* state){
	SE::Window::state = state;
#else
int main(){
	if (setlocale(LC_ALL, "chs") == NULL){
		DEBUG_LOG("failed to set locale\n");
	}
#endif
	
	SE::System::Init();
    SE::Unit::Init();

	SE_main();

#if PLATFORM != PLATFORM_ANDROID
	return 0;
#endif
}