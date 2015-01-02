#pragma once

#include "Macro.h"
#include "unzip.h"

SE_BEGIN

class Stream;


inline bool exists_dir(const char* dir){
#ifdef WINDOWS
	struct _stat fileStat;
	return (_stat(dir, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR);
#else
	struct stat fileStat;
	return (stat(dir, &fileStat) == 0) && S_ISDIR(fileStat.st_mode);
#endif
}

inline bool exists_file(const char* file){
	FILE* f = fopen(file, "rb");
	if (f) fclose(f);;
	return f != NULL;
}

class FileSystem{
protected:
	vector<FileSystem*> fss;
	string name;

	static bool initialized;
	static string app_dir;
	
	FileSystem(const string& name){ this->name = name; }

	virtual bool exists(const string& path) const{ return false; }
	virtual shared_ptr<Stream> open(const string& path) const{ return NULL; }

public:
	static char Separator;

	static FileSystem& GetInstance();
	static const string& GetAppDir();
	
	FileSystem(){ }

	virtual ~FileSystem(){
		for (auto fs : fss)
			delete fs;
	}

	bool Exists(const string& path) const{
		if (exists(path)) return true;
		for (auto fs : fss)
		if (fs->Exists(path))
			return true;
		return false;
	}

	virtual string GetName() const{
		return name;
	}

	shared_ptr<Stream> Open(const string& path) const{
		if (exists(path))
			return open(path);
		for (auto fs : fss)
		if (fs->Exists(path))
			return fs->Open(path);
		ASSERT(false, "cannot find file \"" + path + "\" in whole file system !");
		return NULL;
	}

	bool AddPath(const string& path);

	bool AddZip(const string& file);

	bool AddZip(const string& file, const string& path);

#ifdef ANDROID
	bool AddAsset(AAssetManager* manager, const string& path = "");
#endif

	bool AddFileSystem(FileSystem *fs){
		fss.push_back(fs);
	}
};

class Directory : public FileSystem{
protected:
	virtual bool exists(const string& path) const;
	virtual shared_ptr<Stream> open(const string& path) const;

public:
	Directory(const string& path) {
		char c = path[path.length() - 1];
		if (c != '/' && c != '\\')
			name = path + Separator;
		else
			name = path;
	}

};

class ZipFile : public FileSystem{
private:
	unzFile zip;
	string path;

protected:
	virtual bool exists(const string& path) const;
	virtual shared_ptr<Stream> open(const string& path) const;

public:
	ZipFile(const string& zip) : FileSystem(zip) {
		this->zip = unzOpen64(zip.c_str());
		ASSERT(this->zip, "cannot load " + zip);
	}

	ZipFile(const string& zip, const string& path) : FileSystem(zip) {
		this->zip = unzOpen64(zip.c_str());
		ASSERT(this->zip, "cannot load " + zip);

		char c = path[path.length() - 1];
		if (c != '/' && c != '\\')
			this->path = path + '/';
		else
			this->path = path;
	}

	const unzFile& GetHandle() const{ return zip; }

	virtual string GetName() const{
		if (path.empty())
			return name;
		return name + " -> " + path;
	}

};

#ifdef ANDROID

class Asset : public FileSystem{
protected:
	AAssetManager* manager;

	virtual bool exists(const string& path) const;
	virtual shared_ptr<Stream> open(const string& path) const;

public:
	Asset(AAssetManager* manager, const string& path = "") {
		this->manager = manager;
		if(path=="")
			name = path;
		else{
			char c = path[path.length() - 1];
			if (c != '/' && c != '\\')
				name = path + Separator;
			else
				name = path;
		}
	}
};

#endif

SE_END
