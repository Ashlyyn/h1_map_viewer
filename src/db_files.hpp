#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "com_defs.hpp"
#include "fs_files.hpp"

NO_DISCARD std::filesystem::path  DB_ImagePath (std::string_view image_name );
NO_DISCARD std::string            DB_LoadShader(std::string_view shader_name);
NO_DISCARD std::vector<std::byte> DB_LoadFont  (std::string_view font_name  );
NO_DISCARD std::vector<std::byte> DB_LoadImage (std::string_view image_name );
NO_DISCARD StreamFile             DB_LoadMap   (std::string_view map_name   );