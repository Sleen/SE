#pragma once

#include "Macro.h"
#include "Color.h"
#include "Brush.h"
#include "Stream.h"

SE_BEGIN

class Texture;

class Bitmap{
private:
	GLubyte *data = NULL;
	unsigned width, height;

	void setColor(unsigned x, unsigned y, int color);
	void setup(GLuint textureID, int left, int top, int width, int height);

public:
	Bitmap(unsigned width, unsigned height, int color = Color::Transparent);
	Bitmap(const Bitmap& bmp);
	Bitmap(unsigned width, unsigned height, unsigned checkSize, int c1, int c2);
	Bitmap(const string& file);
	Bitmap(Stream& stream, const string& format = ".png");
	Bitmap(GLuint textureID, int left, int top, int width, int height);
	Bitmap(const Texture& texture, int left, int top, int width, int height);
	Bitmap(const Texture& texture);
	bool Save(const string& file);
	bool Save(const string& file, const string& format);	// format can be ".bmp" or ".png"

	Bitmap(){ }
	~Bitmap(){ if(data) delete[] data; }
	GLubyte *GetData(){ return data; }
	GLubyte *GetData() const{ return data; }
	unsigned GetWidth() const{ return width; }
	unsigned GetHeight() const{ return height; }
};

SE_END