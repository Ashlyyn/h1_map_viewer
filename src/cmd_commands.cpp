#include <string_view>
#include <functional>
#include <unordered_map>

#include "cmd_commands.hpp"
#include "com_print.hpp"

std::unordered_map<std::string_view, std::function<void(void)>> s_cmds;

CmdArgs cmd_args;

void Cmd_Init() {
	s_cmds.clear();
}

bool Cmd_AddCommand(std::string_view cmdName, std::function<void(void)> fn) {
	bool exists = s_cmds.contains(cmdName);
	s_cmds[cmdName] = fn;
	return exists;
}

bool Cmd_GetCommand(std::string_view cmdName, OUT std::function<void(void)>& fn) {
	if (!s_cmds.contains(cmdName))
		return false;

	fn = s_cmds.at(cmdName);
	return true;
}

void Cmd_RemoveCommand(std::string_view cmdName) {
	s_cmds.erase(cmdName);
}

void Cmd_ClearCommands() {
	s_cmds.clear();
}

int Cmd_Argc() {
	return cmd_args.args.size();
}

std::string_view Cmd_Argv(int i) {
	if (i >= Cmd_Argc()) {
		Com_DPrintln("Cmd_Argv: index {} out of range [0..{})", i, Cmd_Argc());
		return "";
	}

	return cmd_args.args.at(i);
}

bool Cmd_TakeInput(std::string_view input) {	
	cmd_args.args.clear();

	Com_Split(input, cmd_args.args);
	
	return true;
}

void Cmd_Shutdown() {
	Cmd_ClearCommands();
}