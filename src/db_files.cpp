#include "db_files.hpp"

#include <filesystem>
#include <map>

#include "com_defs.hpp"
#include "fs_files.hpp"

enum AssetType {
	AT_SHADER,
	AT_FONT,
	AT_IMAGE,
};

std::map<AssetType, std::string_view> s_assetDirs = {
	{ AT_SHADER, "shaders" },
	{ AT_FONT,   "fonts"   },
	{ AT_IMAGE,  "images"  }
};

std::string_view DB_AssetDirForType(AssetType at) {
	return s_assetDirs.at(at);
}

static const std::filesystem::path ASSETS_BASE_DIR = 
	std::filesystem::path("../../../assets");

NO_DISCARD std::filesystem::path DB_AssetPath(
	AssetType at, std::string_view assetName
) {
	return std::filesystem::path(ASSETS_BASE_DIR) / 
		DB_AssetDirForType(at) / 
		assetName;
}

NO_DISCARD std::filesystem::path DB_ImagePath(std::string_view image_name) {
	return DB_AssetPath(AT_IMAGE, image_name);
}

std::string DB_LoadAsset_Text(
	AssetType assetType, std::string_view assetName
) {
	return FS_ReadFileText(DB_AssetPath(assetType, assetName));
}

std::vector<std::byte> DB_LoadAsset_Binary(
	AssetType assetType, std::string_view assetName
) {
	return FS_ReadFile(DB_AssetPath(assetType, assetName));
}

std::string DB_LoadShader(std::string_view shader_name) {
	return DB_LoadAsset_Text(AT_SHADER, shader_name);
}

std::vector<std::byte> DB_LoadFont(std::string_view font_name) {
	return DB_LoadAsset_Binary(AT_FONT, font_name);
}

std::vector<std::byte> DB_LoadImage(std::string_view image_name) {
	return DB_LoadAsset_Binary(AT_IMAGE, image_name);
}