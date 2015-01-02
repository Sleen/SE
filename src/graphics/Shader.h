#pragma once

#include "Macro.h"

SE_BEGIN

class Stream;

class Shader{
protected:
	GLuint id = 0;
	string src;

	Shader(const string src){
		this->src = src;
	}

	virtual bool Compile() = 0;
	virtual void PrintLog() const;

public:

	GLuint GetID() const{
		return id;
	}

	const string& GetSource() const{
		return src;
	}
};

class VertexShader : public Shader{
private:
	const static string header;

	bool Compile(){
		id = glCreateShader(GL_VERTEX_SHADER);
		return Shader::Compile();
	}

public:
	VertexShader(const string& src)
		: Shader(header + src) {
		Compile();
	}

	const string& Header();
};

class FragmentShader : public Shader{
private:
	const static string header;

	bool Compile(){
		id = glCreateShader(GL_FRAGMENT_SHADER);
		return Shader::Compile();
	}

public:
	FragmentShader(const string& src)
		: Shader(header + src) {
		Compile();
	}

	const string& Header();
};

enum ShaderLoc{
	POSITION_ATTRIB_LOC,
	COLOR_ATTRIB_LOC,
	NORMAL_ATTRIB_LOC,
	TEXCOORD_ATTRIB_LOC
};

class Program{
private:
	GLint valid;

protected:
	GLuint prog = 0;

	static Program *current;
	
	void CreateProgram(GLuint vs, GLuint fs);
	void PrintLog();
	void BindVars();
	void ReadLocs();

public:

	GLuint uMMatrix;
	GLuint uPMatrix;
	GLuint uTMatrix;
	GLuint uTex;
	GLuint uTex2;
	GLuint uTexWidth;
	GLuint uTexHeight;
	GLuint uColor;
	GLuint useTex2;
	GLuint useTex;
	GLuint useAColor;
	GLuint useUColor;
	GLuint uClipRect;
	GLuint uClipMode;

	Program(GLuint vs, GLuint fs);
	Program(Stream& vs, Stream& fs);
	Program(const VertexShader& vs, const FragmentShader& fs);
	Program(const char* vs, const char* fs);

	bool IsValid(){ return valid != 0; }
	void Use();
	GLint GetUniLoc(const GLchar *name);
	GLint GetAttrLoc(const GLchar *name);

	static Program* CurrentShader(){
		return current;
	}

};

SE_END
