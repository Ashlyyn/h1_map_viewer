#include "fs_files.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <cstdlib>
#include <cstdio>

#include "com_defs.hpp"
#include "com_print.hpp"

NO_DISCARD std::vector<std::byte> FS_ReadFile(std::filesystem::path path) {
	SDL_RWops* ops = SDL_RWFromFile(path.string().c_str(), "rb");
	if (ops == NULL) {
		Com_Println(
			CON_DEST_INFO, "Failed to open file '{}' for reading: {}",
			path.string(), SDL_GetError()
		);
		return std::vector<std::byte>();
	}

	Sint64 len = SDL_RWsize(ops);

	std::vector<std::byte> v;
	v.resize(len);

	size_t c = SDL_RWread(ops, v.data(), len);
	if ((Sint64)c < len)
		Com_Println(
			CON_DEST_ERR, "Truncated read of file '{}' (expected {} bytes, got {}).",
			path.string(), len, c
		);

	return v;
}

NO_DISCARD std::string FS_ReadFileText(std::filesystem::path path) {
	SDL_RWops* ops = SDL_RWFromFile(path.string().c_str(), "r");
	if (ops == NULL) {
		Com_Println(
			CON_DEST_INFO, "Failed to open file '{}' for reading: {}", 
			path.string(), SDL_GetError()
		);
		return std::string();
	}

	Sint64 len = SDL_RWsize(ops);

	std::string s;
	s.resize(len);

	size_t c = SDL_RWread(ops, s.data(), len);
	if ((Sint64)c < len)
		Com_Println(
			CON_DEST_ERR, "Truncated read of file '{}' (expected {} bytes, got {}).",
			path.string(), len, c
		);

	if (s.at(len - 1) != '\0')
		s.push_back('\0');

	return s;
}

long long FS_SeekStream(INOUT StreamFile& file, SeekFrom from, size_t off) {
	if (off == 0)
		return 0;

	Sint64 begin_pos = SDL_RWtell(file.f);
	Sint64 res = -1;
	switch (from) {
	case FS_SEEK_BEGIN:
		res = SDL_RWseek(file.f, off, SDL_RW_SEEK_SET);
		break;
	case FS_SEEK_END:
		res = SDL_RWseek(file.f, off, SDL_RW_SEEK_END);
		break;
	case FS_SEEK_CUR:
		res = SDL_RWseek(file.f, off, SDL_RW_SEEK_CUR);
		break;
	case FS_SEEK_ABS:
		res = SDL_RWseek(file.f, 0, SDL_RW_SEEK_SET);
		if(res == 0)
			res = SDL_RWseek(file.f, off, SDL_RW_SEEK_CUR);
		break;
	default:
		assert(false);
	}
	file.pos = res;
	return (long long)file.pos - (long long)begin_pos;
}

std::vector<std::byte> FS_ReadStream(const StreamFile& file, size_t count) {
	std::vector<std::byte> v;
	v.resize(count);
	size_t sz = SDL_RWread(file.f, v.data(), count);
	if(sz != v.size())	
		v.resize(sz);
	return v;
}

NO_DISCARD StreamFile FS_StreamFile(std::filesystem::path path, SeekFrom from, size_t off) {
	SDL_RWops* f = SDL_RWFromFile(path.string().c_str(), "rb");
	StreamFile s = { .f = f, .pos = 0 };
	FS_SeekStream(s, from, off);
	return s;
}