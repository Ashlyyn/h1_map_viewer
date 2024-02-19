#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "com_defs.hpp"

NO_DISCARD std::vector<std::byte> FS_ReadFile(    std::filesystem::path path);
NO_DISCARD std::string            FS_ReadFileText(std::filesystem::path path);