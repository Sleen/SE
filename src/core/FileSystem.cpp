#include "FileSystem.h"
#include "Window.h"
#include "Stream.h"

SE_BEGIN

#ifdef WINDOWS
char FileSystem::Separator = '\\';
#else
char FileSystem::Separator = '/';
#endif

bool FileSystem::initialized = false;
string FileSystem::app_dir;

FileSystem& FileSystem::GetInstance(){
	static FileSystem instance;
	if(!initialized){
		initialized = true;
#ifdef WINDOWS
		instance.AddPath("res");
#	ifdef _MSC_VER
		instance.AddPath("E:\\Git\\SE\\res");
#	endif
#elif defined(ANDROID)
		instance.AddAsset(Window::state->activity->assetManager, "");
#endif
	}
	
	return instance;
}

const string& FileSystem::GetAppDir(){
	if(app_dir.empty()){
#ifdef WINDOWS
		char buf[128];
		GetModuleFileName(NULL, buf, 128);
		app_dir = buf;
		app_dir = app_dir.substr(0, app_dir.find_last_of(Separator)+1);
#elif defined(ANDROID)
		//JNIHelper::getInstance()->init(Window::state->activity);
		app_dir = JNIHelper::getInstance()->getExternalFilesDir() + Separator;
#endif
	}
	
	return app_dir;
}

bool Directory::exists(const string& path) const{
	return exists_file((name + path).c_str());// || exists_dir(path.c_str());
}

bool ZipFile::exists(const string& path) const{
	return unzLocateFile(zip, (this->path + path).c_str(), 0) == UNZ_OK;
}
bool FileSystem::AddPath(const string& path){
	if (exists_dir(path.c_str())){
		fss.push_back(new Directory(path));
		DEBUG_LOG("directory \"%s\" added\n", path.c_str());
		return true;
	}

	DEBUG_LOG("cannot add directory \"%s\"\n", path.c_str());
	return false;
}

bool FileSystem::AddZip(const string& file){
	if (exists_file(file.c_str())){
		ZipFile *zip = new ZipFile(file);

		if (zip->GetHandle()){
			fss.push_back(zip);
			DEBUG_LOG("zip file \"%s\" added\n", file.c_str());
			return true;
		}
	}

	DEBUG_LOG("cannot add zip file \"%s\"\n", file.c_str());
	return false;
}

bool FileSystem::AddZip(const string& file, const string& path){
	if (exists_file(file.c_str())){
		ZipFile *zip = new ZipFile(file, path);

		if (zip->GetHandle()){
			fss.push_back(zip);
			DEBUG_LOG("zip file with directory specified \"%s\" added\n", zip->GetName().c_str());
			return true;
		}
	}

	DEBUG_LOG("cannot add zip file \"%s\"\n", file.c_str());
	return false;
}

shared_ptr<Stream> Directory::open(const string& path) const{
	return shared_ptr<Stream>(new FileStream(name + path));
}

shared_ptr<Stream> ZipFile::open(const string& path) const{
	return shared_ptr<Stream>(new ZipStream(*this, this->path + path));
}

#ifdef ANDROID

bool FileSystem::AddAsset(AAssetManager* manager, const string& path){
	AAssetDir* d;
	if(!path.empty()) d = AAssetManager_openDir(manager, path.c_str());
	if (path.empty() || d){
		Asset* a = new Asset(manager, path);
		fss.push_back(a);
		DEBUG_LOG("asset directory \"%s\" added\n", path.c_str());

		return true;
	}

	if(d) AAssetDir_close(d);
	DEBUG_LOG("cannot add asset directory \"%s\"\n", path.c_str());
	return false;
}

bool Asset::exists(const string& path) const{
	AAsset* f = AAssetManager_open(manager, (name + path).c_str(), AASSET_MODE_UNKNOWN);
	return f != NULL;
}

shared_ptr<Stream> Asset::open(const string& path) const{
	return shared_ptr<Stream>(new AssetStream(manager, name + path));
}

#endif

SE_END
