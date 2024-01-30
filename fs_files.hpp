#pragma once

#include <vector>
 
#include <string>

std::vector<std::byte> FS_ReadFile(    const std::string& fname);
std::string            FS_ReadFileText(const std::string& fname);