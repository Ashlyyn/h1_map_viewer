#pragma once

#include <string_view>

bool Con_ProcessInput(std::string_view input);
bool Con_ProcessInput(std::string_view input, size_t localClientNum);