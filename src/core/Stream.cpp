#include "Stream.h"
#include "FileSystem.h"

SE_BEGIN

ZipStream::ZipStream(const ZipFile& zip, const string& file){
	this->zip = zip.GetHandle();
	size = 0;
	if (unzLocateFile(this->zip, file.c_str(), 0) != UNZ_OK){
		ASSERT(false, string("cannot find file \"") + file + "\" in " + zip.GetName() + " !");
	}
	else{
		unz_file_info64 fi;
		int err = unzGetCurrentFileInfo64(this->zip, &fi, const_cast<char*>(file.c_str()), sizeof(file.c_str()), NULL, 0, NULL, 0);
		if (err != UNZ_OK){
			ASSERT(false, "error with zipfile in unzGetCurrentFileInfo64 !");
			return;
		}

		err = unzOpenCurrentFilePassword(this->zip, NULL);
		if (err != UNZ_OK)
		{
			ASSERT(false, "error with zipfile in unzOpenCurrentFilePassword !");
			return;
		}

		size = (int)fi.uncompressed_size;

	}
}

#ifdef ANDROID

AssetStream::AssetStream(AAssetManager* manager, const string& file){
	asset = AAssetManager_open(manager, file.c_str(), AASSET_MODE_UNKNOWN);
	if (asset == NULL){
		DEBUG_LOG("failed to open \"%s\"\n", file.c_str());
		return;
	}

	size = AAsset_getLength(asset);
}

#endif

SE_END
