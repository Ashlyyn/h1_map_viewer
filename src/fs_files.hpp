#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "com_defs.hpp"

NO_DISCARD std::vector<std::byte> FS_ReadFile(    std::filesystem::path path);
NO_DISCARD std::string            FS_ReadFileText(std::filesystem::path path);