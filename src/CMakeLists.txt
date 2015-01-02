project(SE_lib)

include_directories(
	./
	${PROJECT_SOURCE_DIR}/core 
	${PROJECT_SOURCE_DIR}/math
	${PROJECT_SOURCE_DIR}/graphics
	${PROJECT_SOURCE_DIR}/app
	${PROJECT_SOURCE_DIR}/gui
	${SE_SOURCE_DIR}/lib/freetype2/include
	${SE_SOURCE_DIR}/lib/glew/include
	${SE_SOURCE_DIR}/lib/minizip/include
	${SE_SOURCE_DIR}/lib/png/include
	${SE_SOURCE_DIR}/lib/z/include
	${SE_SOURCE_DIR}/lib/lua/include
	${NDK_ROOT}/platforms/android-19/arch-x86/usr/include
	${NDK_ROOT}/sources/android/native_app_glue
)

if(WIN32 OR APPLE)
else()
	find_package(X11)
	if(X11_FOUND)
		include_directories(${X11_INCLUDE_DIR})
	else()
		message("fatal error: x11 lib not found !")
	endif()
endif()

set(SRC 
	SE.cpp
	SE.h

	core/json11.cpp
	core/Impl.cpp
	core/Core.h
	core/Type.h
	core/Type.cpp
	core/Macro.h
	core/Event.h
	core/MyTime.h
	core/MyTime.cpp
	core/FileSystem.h
	core/FileSystem.cpp
	core/Stream.h
	core/Stream.cpp
	core/System.h
	core/System.cpp

	math/my_math.h
	math/Tuple.h
	math/Vector.h
	math/Rect.h
	math/Matrix.h
	math/Matrix.cpp

	graphics/graphicsLib.h
	graphics/Color.h
	graphics/Brush.h
	graphics/Brush.cpp
	graphics/Pen.h
	graphics/Pen.cpp
	graphics/Graphics.h
	graphics/Graphics.cpp
	graphics/ImageLoader.h
	graphics/ImageLoader.cpp
	graphics/Bitmap.h
	graphics/Bitmap.cpp
	graphics/Transform.h
	graphics/Transform.cpp
	graphics/GLHelper.h
	graphics/GLHelper.cpp
	graphics/Shader.h
	graphics/Shader.cpp
	graphics/Texture.h
	graphics/Texture.cpp
	graphics/Model.h
	graphics/Model.cpp
	graphics/Font.h
	graphics/Font.cpp

	app/App.h
	app/Control.h
	app/Control.cpp
	app/Scene.h
	app/Window.h
	app/WindowCommon.cpp
	app/Win32Window.cpp
	app/AndroidWindow.cpp
	app/LinuxWindow.cpp
	app/Application.h
	app/View.h
	app/View.cpp
	
	gui/Gui.h
	gui/Label.h
	gui/Button.h
	gui/Button.cpp
	gui/CheckBox.h
	gui/RadioButton.h
	gui/ImageBox.h
	gui/GroupBox.h
	gui/TrackBar.h
)

link_directories(
	${SE_SOURCE_DIR}/lib/freetype2/bin
	${SE_SOURCE_DIR}/lib/glew/bin
	${SE_SOURCE_DIR}/lib/minizip/bin
	${SE_SOURCE_DIR}/lib/png/bin
	${SE_SOURCE_DIR}/lib/z/bin
	${SE_SOURCE_DIR}/lib/lua/bin
)

add_library(SE ${SRC})

target_link_libraries(SE 
	freetype2
	glew
	minizip
	png
	z
	lua
)

set(TEST_SRC
		test/test_main.cpp
		test/ImageButton.h
)
include_directories( ${PROJECT_SOURCE_DIR}/test ) 
add_executable(SE_TEST ${TEST_SRC})
#set_target_properties(SE PROPERTIES LINKER_LANGUAGE CXX)
link_directories(${SE_SOURCE_DIR}/bin)
target_link_libraries(SE_TEST SE glew)
#target_link_libraries(SE_TEST glew)

if(X11_FOUND)
	target_link_libraries(SE ${X11_LIBRARIES})
	#target_link_libraries(SE_TEST ${X11_LIBRARIES})
endif()

set(LIBRARY_OUTPUT_PATH ${SE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${SE_SOURCE_DIR}/bin/test)

install(DIRECTORY core/ DESTINATION include PATTERN "*.cpp" EXCLUDE)
install(DIRECTORY math/ DESTINATION include PATTERN "*.cpp" EXCLUDE)
install(DIRECTORY graphics/ DESTINATION include PATTERN "*.cpp" EXCLUDE)
install(DIRECTORY app/ DESTINATION include PATTERN "*.cpp" EXCLUDE)
install(FILES SE.h DESTINATION include)
