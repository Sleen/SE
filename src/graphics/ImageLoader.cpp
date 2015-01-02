#include "ImageLoader.h"
#include "MyTime.h"
#include "Stream.h"
#include "FileSystem.h"

SE_BEGIN

ImageLoader* ImageLoader::GetLoader(const string& file){
	DEBUG_LOG("load image \"%s\" ...\n", file.c_str());

	string ex = file.substr(file.length() - 4);
	transform(ex.begin(), ex.end(), ex.begin(), (int(*)(int))tolower);
	if (ex == ".bmp"){
		return BMPLoader::GetInstance();
	}
	else if (ex == ".png"){
		return PNGLoader::GetInstance();
	}
	/*else if (ex == L".jpg"){
	return new JPGLoader();
	}*/
	return NULL;
}

bool ImageLoader::Load(const string& file, unsigned& width, unsigned& height, GLubyte*& data){
	auto f = FileSystem::GetInstance().Open(file);
	if (f.get() == NULL)
	{
		DEBUG_LOG("failed to open file \"%s\" !\n", file.c_str());
		return false;
	}
	return Load(*f, width, height, data);
}

bool BMPLoader::Load(Stream& stream, unsigned& width, unsigned& height, GLubyte*& data){
	if (stream.Size()>0){
		int64_t t = GetMicroSecond();
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER ih;
		stream.Read(&fh, sizeof(fh), 1);
		stream.Read(&ih, sizeof(ih), 1);
		
		if (ih.biBitCount == 24 && ih.biCompression == 0){
			width = ih.biWidth;
            height = abs(ih.biHeight);
            int padding = ih.biSizeImage / height - width*3;
            int padding_buf;
			GLubyte *d;
			if (ih.biHeight < 0){
				data = new GLubyte[width*height * 4];
                for (unsigned int i = 0; i < height; i++){
                    for (unsigned int j = 0; j < width; j++)
                    {
                        d = data + (i*width + j) * 4;
                        GLubyte c[3];
                        stream.Read(c, 1, 3);
                        d[0] = c[2];
                        d[1] = c[1];
                        d[2] = c[0];
                        d[3] = 255;
                    }
                    if (padding > 0){
                        stream.Read(&padding_buf, padding);
                    }
                }
			}
			else{
				data = new GLubyte[width*height * 4];
                for (int i = height - 1; i >= 0; i--){
                    for (unsigned int j = 0; j < width; j++)
                    {
                        d = data + (i*width + j) * 4;
                        GLubyte c[3];
                        stream.Read(c, 1, 3);
                        d[0] = c[2];
                        d[1] = c[1];
                        d[2] = c[0];
                        d[3] = 255;
                    }
                    if (padding > 0){
                        stream.Read(&padding_buf, padding);
                    }
                }
			}
			DEBUG_LOG("load success, size: %d x %d, time consumed: %.3f ms\n", width, height, (GetMicroSecond() - t) / 1000.0f);
			return true;
		}
		else{
			DEBUG_LOG("load failed from stream, only 24-bit non-compression BMP files are supported\n");
		}
	}
	else{
		DEBUG_LOG("load failed from stream, too short\n");
	}
	return false;
}

bool BMPLoader::Save(const string& file, unsigned width, unsigned height, GLubyte* data){
	FILE *f = fopen(file.c_str(), "wb");
	if (f){
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER ih;
		memset(&fh, 0, sizeof(fh));
		memset(&ih, 0, sizeof(ih));
		fh.bfType = 0x4D42;
		fh.bfOffBits = 54;
		fh.bfSize = 1;
		ih.biSize = 40;
		ih.biWidth = width;
		ih.biHeight = height;
		ih.biPlanes = 1;
		ih.biBitCount = 24;
        int line = width * 3;
        int padding = 0;
        char padding_char = 0;
        if (line % 4 != 0){
            padding = 4 - line % 4;
            line += padding;
        }
        ih.biSizeImage = line * height;
		fwrite(&fh, sizeof(fh), 1, f);
		fwrite(&ih, sizeof(ih), 1, f);
        for (int i = height - 1; i >= 0; i--){
            for (int j = 0; j < width; j++)
            {
                GLubyte *d = data + (i*width + j) * 4;
                GLubyte c[3];
                c[0] = d[2];
                c[1] = d[1];
                c[2] = d[0];
                fwrite(c, 3, 1, f);
            }
            if (padding > 0){
                fwrite(&padding_char, 1, padding, f);
            }
        }
		fclose(f);
		return true;
	}
	else{
		DEBUG_LOG("\"%s\"  save failed\n", file.c_str());
	}

	return false;
}

bool PNGLoader::Load(Stream& stream, unsigned& width, unsigned& height, GLubyte*& data){
	png_image image;
	int64_t t = GetMicroSecond();
	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;
	MemoryStream ms(stream);
	if (png_image_begin_read_from_memory(&image, ms.GetBuffer(), ms.Size())){
		image.format = PNG_FORMAT_RGBA;
		data = new GLubyte[PNG_IMAGE_SIZE(image)];
		if (data && png_image_finish_read(&image, NULL, data, 0, NULL)){
			width = image.width;
			height = image.height;
			DEBUG_LOG("load success, size: %d x %d, time consumed: %.3f ms\n", width, height, (GetMicroSecond() - t) / 1000.0f);
			return true;
		}
	}
	DEBUG_LOG("load failed form stream\n");
	return false;
}

bool PNGLoader::Save(const string& file, unsigned width, unsigned height, GLubyte* data){
	png_image image;
	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;
	image.format = PNG_FORMAT_RGBA;
	image.width = width;
	image.height = height;
	bool r = png_image_write_to_file(&image, file.c_str(), 0, data, 0, NULL) != 0;
	if (r)
		DEBUG_LOG("image saved to \"%s\" successfully ! \n", file.c_str());
	else
		DEBUG_LOG("failed saving image to \"%s\" ! \n", file.c_str());
	return r;
}

SE_END