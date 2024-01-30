#include <vector>
#include <string_view>
#include <string>

#include <cstdio>
#include <cstdlib>

#include <SDL3/SDL.h>

#include "com_defs.hpp"
#include "com_print.hpp"

NO_DISCARD std::vector<std::byte> FS_ReadFile(const std::string& fname) {
	SDL_RWops* ops = SDL_RWFromFile(fname.c_str(), "rb");
	if (ops == NULL) {
		Com_Println(CON_DEST_INFO, "Failed to open file '{}' for reading: {}", fname, SDL_GetError());
		return std::vector<std::byte>();
	}

	Sint64 len = SDL_RWsize(ops);

	std::vector<std::byte> v;
	v.resize(len);

	size_t c = SDL_RWread(ops, v.data(), len);
	if (c < len)
		Com_Println(CON_DEST_ERR, "Truncated read of file '{}' (expected {} bytes, got {}).", fname, len, c);

	return v;
}

NO_DISCARD std::string FS_ReadFileText(const std::string& fname) {
	SDL_RWops* ops = SDL_RWFromFile(fname.c_str(), "r");
	if (ops == NULL) {
		Com_Println(CON_DEST_INFO, "Failed to open file '{}' for reading: {}", fname, SDL_GetError());
		return std::string();
	}

	Sint64 len = SDL_RWsize(ops);

	std::string s;
	s.resize(len);

	size_t c = SDL_RWread(ops, s.data(), len);
	if (c < len)
		Com_Println(CON_DEST_ERR, "Truncated read of file '{}' (expected {} bytes, got {}).", fname, len, c);

	if (s.at(len - 1) != '\0')
		s.push_back('\0');

	return s;
}