#include <string_view>
#include <functional>
#include <unordered_map>

std::unordered_map<std::string_view, std::function<void(void)>> s_cmds;

bool Cmd_AddCommand(std::string_view cmdName, std::function<void(void)> fn) {
	bool exists = s_cmds.contains(cmdName);
	s_cmds[cmdName] = fn;
	return exists;
}
