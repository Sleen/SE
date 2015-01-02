#pragma once

#include "Macro.h"
#include "png.h"
//#include "libs\libjpeg\jpeglib.h"

#ifndef WINDOWS
typedef struct{
	uint16_t	bfType;
	uint32_t	bfSize;
	uint16_t	bfReserved1;
	uint16_t	bfReserved2;
	uint32_t	bfOffBits;
} BITMAPFILEHEADER;

typedef struct{
	uint32_t	biSize;
	uint32_t	biWidth;
	uint32_t	biHeight;
	uint16_t	biPlanes;
	uint16_t	biBitCount;
	uint32_t	biCompression;
	uint32_t	biSizeImage;
	uint32_t	biXPelsPerMeter;
	uint32_t	biYPelsPerMeter;
	uint32_t	biClrUsed;
	uint32_t	biClrImportant;
} BITMAPINFOHEADER;
#endif

SE_BEGIN

class Stream;

class ImageLoader{
public:
	virtual bool Load(const string& file, unsigned& width, unsigned& height, GLubyte*& data);
	virtual bool Load(Stream& stream, unsigned& width, unsigned& height, GLubyte*& data) = 0;
	virtual bool Save(const string& file, unsigned width, unsigned height, GLubyte* data) = 0;
	static ImageLoader*  GetLoader(const string& file);
};

class BMPLoader : public ImageLoader{
public:
	bool Load(Stream& stream, unsigned& width, unsigned& height, GLubyte*& data);
	bool Save(const string& file, unsigned width, unsigned height, GLubyte* data);
	static BMPLoader* GetInstance(){
		static BMPLoader loader;
		return &loader;
	}
};

class PNGLoader : public ImageLoader{
public:
	bool Load(Stream& stream, unsigned& width, unsigned& height, GLubyte*& data);
	bool Save(const string& file, unsigned width, unsigned height, GLubyte* data);
	static PNGLoader* GetInstance() {
		static PNGLoader loader;
		return &loader;
	}
};
/*
class JPGLoader : public ImageLoader{
public:
	bool Load(const String& file, unsigned& width, unsigned& height, GLubyte*& data){
		struct jpeg_decompress_struct cinfo;
		FILE * infile;
		JSAMPARRAY buffer;
		int row_stride;

		if ((infile = _wfopen(file.Data(), L"rb")) == NULL) {
			return false;
		}

		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, infile);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);
		
		row_stride = cinfo.output_width * cinfo.output_components;
		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, buffer, 1);
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
		width = cinfo.image_width;
		height = cinfo.image_height;
		data = new GLubyte[width*height * 4];
		memcpy(data, buffer, sizeof(data));
		free(buffer);

		return true;
	}

	bool Save(const String& file, unsigned width, unsigned height, GLubyte* data){
		png_image image;
		memset(&image, 0, sizeof(image));
		image.version = PNG_IMAGE_VERSION;
		image.format = PNG_FORMAT_RGBA;
		return png_image_write_to_file(&image, file.ToCStr(), 0, data, 0, NULL);
	}
};
*/

SE_END