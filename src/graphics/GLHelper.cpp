#include "GLHelper.h"

SE_BEGIN

void PrintOpenGLError(){
	GLenum errorCode;
	//const GLubyte *errorString;
	while ((errorCode = glGetError()) != GL_NO_ERROR){
		//errorString = gluErrorString(errorCode);
		//wprintf(L"GL Error: %s\n", errorString);
		DEBUG_LOG("GL Error: %d\n", errorCode);
	}
}

SE_END
