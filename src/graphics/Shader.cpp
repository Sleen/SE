#include "Shader.h"
#include "Stream.h"
#include "GLHelper.h"
#include "Transform.h"

SE_BEGIN

const string VertexShader::header = R"(
uniform mat4 uMMatrix;
uniform mat4 uPMatrix;
uniform mat4 uTMatrix;

attribute vec3 aPosition;
attribute vec4 aColor;
attribute vec2 aTexCoord;

varying vec3 vPosition;
varying vec3 vp, vp2;
varying vec4 vColor;
varying vec2 vTexCoord;

void prepare(){
	gl_Position = uPMatrix * uMMatrix * vec4(aPosition, 1.0);
	vColor = aColor;
	vPosition = vec3(uTMatrix * vec4(aPosition, 1.0));
	vp = aPosition;
	vp2 = vec3(uMMatrix * vec4(aPosition, 1.0));
	vTexCoord = aTexCoord;
}

)";

const string FragmentShader::header = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;
uniform sampler2D tex2;		// mask

uniform vec4 uColor;

uniform bool useTex2;
uniform bool useTex;
uniform bool useAColor;
uniform bool useUColor;
uniform int uTexWidth;
uniform int uTexHeight;
uniform ivec4 uClipRect;
uniform int uClipMode;

varying vec4 vColor;
varying vec3 vPosition;
varying vec3 vp, vp2;
varying vec2 vTexCoord;

bool prepare(){
	//if((uClipMode == 1 && (int(vp.x)<uClipRect[0] || int(vp.y)<uClipRect[1] || int(vp.x)>uClipRect[2] || int(vp.y)>uClipRect[3])) ||
	//	(uClipMode == 2 && (int(vp2.x)<uClipRect[0] || int(vp2.y)<uClipRect[1] || int(vp2.x)>uClipRect[2] || int(vp2.y)>uClipRect[3]))){
	//	discard;
	//	return false;
	//}else{
	gl_FragColor = vec4(1, 1, 1, 1);
	if(useTex2)
		gl_FragColor *= texture2D(tex2, vTexCoord);
	if(useTex)
		gl_FragColor *= texture2D(tex, vPosition.xy/vec2(uTexWidth, uTexHeight));
	if(useAColor)
		gl_FragColor *= vColor;
	if(useUColor)
		gl_FragColor *= uColor;
	//}
	return true;
}

)";


bool Shader::Compile(){
	if (id == 0)
	{
		DEBUG_LOG("tried to compile shader with invalid shader id\n");
		return false;
	}

	char *s = const_cast<char*>(src.c_str());
	glShaderSource(id, 1, (const GLchar**)&s, NULL);
	glCompileShader(id);

	PrintOpenGLError();
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (!result){
		//glDeleteShader(shader);
	}
	PrintLog();
	return result != 0;
}

void Shader::PrintLog() const{
	int infoLogLength = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	PrintOpenGLError();
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	PrintOpenGLError();

	if (infoLogLength > 0){
		infoLog = (GLchar*)malloc(infoLogLength);
		if (infoLog == NULL){
			DEBUG_LOG("ERROR: Cound not allocate InfoLog buffer\n");
			exit(1);
		}
		glGetShaderInfoLog(id, infoLogLength, &charsWritten, infoLog);
		DEBUG_LOG("Shader ID: %d\n", id);
		DEBUG_LOG("Log: %s\n\n", infoLog);
		free(infoLog);
	}
	PrintOpenGLError();
}

Program *Program::current = NULL;

void Program::CreateProgram(GLuint vs, GLuint fs){
	if (vs == 0 || fs == 0)
		return;
	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	BindVars();
	glLinkProgram(prog);
	PrintOpenGLError();
	glGetProgramiv(prog, GL_LINK_STATUS, &valid);
	PrintLog();
	if (!valid){
		prog = 0;
		DEBUG_LOG("failed to link shader program !\n");
	}
	else{
		glUseProgram(prog);
		ReadLocs();
	}
}

string TextFromStream(Stream& stream){
	int size = stream.Size();
	char *src = new char[size + 1];
	stream.Read(src, size);
	src[size] = 0;
	string s = src;
	delete[] src;
	return s;
}

Program::Program(GLuint vs, GLuint fs){
	CreateProgram(vs, fs);
}

Program::Program(const VertexShader& vs, const FragmentShader& fs){
	CreateProgram(vs.GetID(), fs.GetID());
}

Program::Program(Stream& vstream, Stream& fstream){
	VertexShader vs(TextFromStream(vstream));
	FragmentShader fs(TextFromStream(fstream));
	CreateProgram(vs.GetID(), fs.GetID());
}

Program::Program(const char* vstr, const char* fstr){
	VertexShader vs(vstr);
	FragmentShader fs(fstr);
	CreateProgram(vs.GetID(), fs.GetID());
}

void Program::BindVars(){
	glBindAttribLocation(prog, POSITION_ATTRIB_LOC, "aPosition");
	glBindAttribLocation(prog, COLOR_ATTRIB_LOC, "aColor");
	glBindAttribLocation(prog, NORMAL_ATTRIB_LOC, "aNormal");
	glBindAttribLocation(prog, TEXCOORD_ATTRIB_LOC, "aTexCoord");
}

void Program::ReadLocs(){
	uMMatrix = GetUniLoc("uMMatrix");
	uPMatrix = GetUniLoc("uPMatrix");
	uTMatrix = GetUniLoc("uTMatrix");
	uTex = GetUniLoc("tex");
	uTex2 = GetUniLoc("tex2");
	uTexWidth = GetUniLoc("uTexWidth");
	uTexHeight = GetUniLoc("uTexHeight");
	uColor = GetUniLoc("uColor");
	useTex2 = GetUniLoc("useTex2");
	useTex = GetUniLoc("useTex");
	useAColor = GetUniLoc("useAColor");
	useUColor = GetUniLoc("useUColor");
	uClipRect = GetUniLoc("uClipRect");
	uClipMode = GetUniLoc("uClipMode");
}

void Program::Use(){
	if (!IsValid())
		DEBUG_LOG("tried to use invalid shader program !\n");
	if (current != this){
		current = this;
		glUseProgram(prog);
		glUniform1i(uTex, 0);
		glUniform1i(uTex2, 1);
		glUniformMatrix4fv(uMMatrix, 1, false, ModelViewMatrix());
		glUniformMatrix4fv(uPMatrix, 1, false, ProjectionMatrix());
		glUniformMatrix4fv(uTMatrix, 1, false, TextureMatrix());
		//DEBUG_LOG("use shader %d\n", prog);
	}
}

GLint Program::GetUniLoc(const GLchar *name){
	GLint loc = glGetUniformLocation(prog, name);
	if (loc == -1)
		DEBUG_LOG("No such uniform named \"%s\"\n", name);
	PrintOpenGLError();
	return loc;
}

GLint Program::GetAttrLoc(const GLchar *name){
	GLint loc = glGetAttribLocation(prog, name);
	if (loc == -1)
		DEBUG_LOG("No such attribute named \"%s\"\n", name);
	PrintOpenGLError();
	return loc;
}

void Program::PrintLog(){
	int infoLogLength = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	PrintOpenGLError();
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);
	PrintOpenGLError();

	if (infoLogLength > 0){
		infoLog = (GLchar*)malloc(infoLogLength);
		if (infoLog == NULL){
			DEBUG_LOG("ERROR: Cound not allocate InfoLog buffer\n");
			exit(1);
		}
		glGetProgramInfoLog(prog, infoLogLength, &charsWritten, infoLog);
		DEBUG_LOG("Program InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
	PrintOpenGLError();
}

SE_END
