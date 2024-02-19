#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <cstdlib>
#include <cstdio>

#include <SDL3/SDL.h>

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