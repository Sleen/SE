#pragma once

#include "Macro.h"
#include "unzip.h"

SE_BEGIN

class Stream{
protected:
	int size;
	virtual int read(void *buf, int size, int count) = 0;

public:
	int Read(void *buf, int size, int count){
		int s = read(buf, size, count);
		this->size -= s;
		if (this->size < 0) this->size = 0;
		return s;
	}

	int Read(void* buf, int count){
		return Read(buf, 1, count);
	}

	int Size() const { return size; }
};

class FileStream : public Stream{
private:
	FILE *file;
public:
	FileStream(const string& file){
		this->file = fopen(file.c_str(), "rb");
#ifdef WINDOWS
		struct _stat st;
		if (_stat(file.c_str(), &st))
#else
		struct stat st;
		if (stat(file.c_str(), &st))
#endif
			size = 0;
		size = st.st_size;
	}

	~FileStream(){
		if (file)
			fclose(file);
	}

	virtual int read(void *buf, int size, int count) override{
		return fread(buf, size, count, file);
	}

};

class ZipFile;

class ZipStream : public Stream{
private:
	unzFile zip;
public:
	ZipStream(const ZipFile& zip, const string& file);

	virtual int read(void *buf, int size, int count) override{
		return unzReadCurrentFile(zip, buf, size*count);
	}
};

class MemoryStream : public Stream{
private:
	char* start = NULL;
	char* buf = NULL;

public:
	MemoryStream(const void* buf, int size){
		this->buf = (char*)buf;
		this->size = size;
	}

	MemoryStream(Stream& s){
		size = s.Size();
		if (size > 0)
		{
			start = buf = (char*)malloc(size);
			s.Read(buf, 1, size);
		}
	}

	~MemoryStream(){
		if (start)
			free(start);
	}

	virtual int read(void *buf, int size, int count) override{
		size = MIN(size*count, this->size);
		memcpy(buf, this->buf, size);
		this->buf += size;
		return size;
	}

	void *GetBuffer(){
		return buf;
	}
};

#ifdef ANDROID

class AssetStream : public Stream{
private:
	AAsset* asset;

public:
	AssetStream(AAssetManager* manager, const string& file);

	~AssetStream(){
		if(asset)
			AAsset_close(asset);
	}

	virtual int read(void *buf, int size, int count) override{
		return AAsset_read(asset, buf, size*count);
	}
};

#endif

SE_END
