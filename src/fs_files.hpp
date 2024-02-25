#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "com_defs.hpp"

struct StreamFile {
	SDL_RWops* f;
	size_t pos;
};

NO_DISCARD std::vector<std::byte> FS_ReadFile    (std::filesystem::path path);
NO_DISCARD std::string            FS_ReadFileText(std::filesystem::path path);

enum SeekFrom {
	FS_SEEK_BEGIN,
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_ABS
};

long long FS_SeekStream(INOUT StreamFile& file, SeekFrom from, size_t off);
NO_DISCARD StreamFile FS_StreamFile(
    std::filesystem::path path, SeekFrom from = FS_SEEK_ABS, size_t off = 0
);