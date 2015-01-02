#include "Bitmap.h"
#include "ImageLoader.h"
#include "Texture.h"

SE_BEGIN

void Bitmap::setColor(unsigned x, unsigned y, int color){
	GLubyte *d = data + (y*width + x) * 4;
	d[0] = R(color);
	d[1] = G(color);
	d[2] = B(color);
	d[3] = A(color);
}

Bitmap::Bitmap(unsigned width, unsigned height, int color){
	data = new GLubyte[width*height * 4];
	this->width = width;
	this->height = height;
	if (color == Color::Transparent){
		memset(data, 0, sizeof(data));
		return;
	}
	for (unsigned i = 0; i < width; i++)
	for (unsigned j = 0; j < height; j++)
		setColor(i, j, color);
}

Bitmap::Bitmap(const Bitmap& bmp){
	width = bmp.width;
	height = bmp.height;
	unsigned size = width*height * 4;
	data = new GLubyte[size];
	memcpy(data, bmp.data, size);
}

Bitmap::Bitmap(unsigned width, unsigned height, unsigned checkSize, int c1, int c2){
	this->width = width;
	this->height = height;
	data = new GLubyte[width*height * 4];

	for (unsigned i = 0; i < width; i++)
	for (unsigned j = 0; j < height; j++)
	{
		if ((i / checkSize + j / checkSize) % 2)
			setColor(i, j, c2);
		else
			setColor(i, j, c1);
	}
}

Bitmap::Bitmap(const string& file){
	ImageLoader *loader = ImageLoader::GetLoader(file);
	if (loader == NULL)
		DEBUG_LOG("\"%s\"  unsupported image format\n", file.c_str());
	else
		loader->Load(file, width, height, data);
}

Bitmap::Bitmap(Stream& stream, const string& format){
	ASSERT(stream.Size() != 0, "tried to construct a bitmap with an empty stream !");
	ImageLoader *loader = ImageLoader::GetLoader(format);
	if (loader == NULL)
		DEBUG_LOG("\"%s\"  unsupported image format\n", format.c_str());
	else
		loader->Load(stream, width, height, data);
}

void Bitmap::setup(GLuint textureID, int left, int top, int width, int height){
	this->width = width;
	this->height = height;
	unsigned size = width*height * 4;
	data = new GLubyte[size];

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	glReadPixels(left, top, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
}

Bitmap::Bitmap(GLuint textureID, int left, int top, int width, int height){
	setup(textureID, left, top, width, height);
}

Bitmap::Bitmap(const Texture& texture, int left, int top, int width, int height){
	setup(texture.GetID(), 0, 0, width, height);
}

Bitmap::Bitmap(const Texture& texture){
	setup(texture.GetID(), 0, 0, texture.GetWidth(), texture.GetHeight());
}

bool Bitmap::Save(const string& file){
	ImageLoader *loader = ImageLoader::GetLoader(file);
	if (loader == NULL)
		DEBUG_LOG("\"%s\"  unsupported image format\n", file.c_str());
	else
		return loader->Save(file, width, height, data);
	return false;
}

// format can be ".bmp" or ".png"
bool Bitmap::Save(const string& file, const string& format){
	ImageLoader *loader = ImageLoader::GetLoader(format);
	if (loader == NULL)
		DEBUG_LOG("\"%s\"  unsupported image format\n", file.c_str());
	else
		return loader->Save(file, width, height, data);
	return false;
}

SE_END