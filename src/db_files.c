#include "db_files.h"

#include <assert.h>

#include "com_print.h"

#ifndef A_PROJECT_ROOT
#define A_PROJECT_ROOT ""
#endif

typedef enum AssetType {
	AT_SHADER,
	AT_FONT,
	AT_IMAGE,
	AT_MAP
} AssetType;

const char* s_assetDirs[] = {
	[AT_SHADER] = "shaders",
	[AT_FONT]   = "fonts",
	[AT_IMAGE]  = "images",
	[AT_MAP]    = "maps"
};

A_NO_DISCARD static const char* DB_AssetDirForType(AssetType at) {
	return s_assetDirs[at];
}

static const char* s_assetsDir = A_PROJECT_ROOT "/assets";

A_NO_DISCARD static const char* DB_AssetPath(
	AssetType at, const char* assetName
) {
	return FS_BuildOsPath(s_assetsDir, DB_AssetDirForType(at), assetName, NULL);
}

A_NO_DISCARD const char* DB_ImagePath(const char* image_name) {
	return DB_AssetPath(AT_IMAGE, image_name);
}

A_NO_DISCARD static const char* DB_MapPath(const char* map_name) {
	return DB_AssetPath(AT_MAP, map_name);
}

A_NO_DISCARD static char* DB_LoadAsset_Text(
	AssetType assetType, const char* assetName, A_OPTIONAL_OUT size_t* sz
) {
	return FS_ReadFileText(DB_AssetPath(assetType, assetName), sz);
}

static void DB_UnloadAsset_Text(char* text) {
	FS_FreeFileText(text);
}

A_NO_DISCARD static void* DB_LoadAsset_Binary(
	AssetType assetType, const char* assetName, A_OPTIONAL_OUT size_t* sz
) {
	return FS_ReadFile(DB_AssetPath(assetType, assetName), sz);
}

static void DB_UnloadAsset_Binary(void* p) {
	FS_FreeFile(p);
}

A_NO_DISCARD char* DB_LoadShader(const char* shader_name) {
	return DB_LoadAsset_Text(AT_SHADER, shader_name, NULL);
}

void DB_UnloadShader(char* shader) {
	DB_UnloadAsset_Text(shader);
}

A_NO_DISCARD void* DB_LoadFont(const char* font_name, A_OUT size_t* sz) {
	return DB_LoadAsset_Binary(AT_FONT, font_name, sz);
}

void DB_UnloadFont(void* font) {
	DB_UnloadAsset_Binary(font);
}

A_NO_DISCARD void* DB_LoadImage(const char* image_name) {
	return DB_LoadAsset_Binary(AT_IMAGE, image_name, NULL);
}

void DB_UnloadImage(void* image) {
	DB_UnloadAsset_Binary(image);
}

A_NO_DISCARD StreamFile DB_LoadMap_Stream(const char* map_name) {
	return FS_StreamFile(DB_MapPath(map_name), 
		                 FS_SEEK_BEGIN, FS_STREAM_READ_EXISTING, 0);
}

void DB_UnloadMap_Stream(A_INOUT StreamFile* stream) {
	FS_CloseStream(stream);
}

A_NO_DISCARD FileMapping DB_LoadMap_Mmap(const char* map_name) {
	return Z_MapFile(DB_MapPath(map_name));
}

void DB_UnloadMap_Mmap(A_INOUT FileMapping* map) {
	Z_UnmapFile(map);
}
