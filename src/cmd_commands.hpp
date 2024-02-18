#pragma once

#include <vector>
#include <string>
#include <functional>

#include "com_defs.hpp"

struct CmdArgs {
	std::vector<std::string> args;
};

void             Cmd_Init();
bool             Cmd_AddCommand(std::string_view cmdName, std::function<void(void)> fn);
bool             Cmd_GetCommand(std::string_view cmdName, OUT std::function<void(void)>& fn);
void             Cmd_RemoveCommand(std::string_view cmdName);
void             Cmd_ClearCommands();
int              Cmd_Argc();
std::string_view Cmd_Argv(int i);
bool             Cmd_TakeInput(std::string_view input);
void             Cmd_Shutdown();