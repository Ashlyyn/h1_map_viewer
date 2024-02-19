#include "con_console.hpp"

#include <functional>

#include "cmd_commands.hpp"
#include "dvar.hpp"

bool Con_ProcessInput(std::string_view input) {
    if (!Cmd_TakeInput(input))
        return false;

    std::function<void(void)> fn;
    if (Cmd_FindCommand(Cmd_Argv(0), fn)) {
        fn();
        return true;
    } else {
        dvar_t* d = Dvar_Find(std::string(Cmd_Argv(0)));
        if (d != nullptr) {
            std::deque<std::string> v;

            if (Cmd_Argc() == 1) {
                Com_Println(CON_DEST_OUT, "{}", Dvar_GetString(*d));
                return true;
            } else {
                v.push_back(std::string(Cmd_Argv(1)));

                if (Dvar_IsVec2(*d) || Dvar_IsVec3(*d) || Dvar_IsVec4(*d))
                    v.push_back(std::string(Cmd_Argv(2)));

                if (Dvar_IsVec3(*d) || Dvar_IsVec4(*d))
                    v.push_back(std::string(Cmd_Argv(3)));

                if (Dvar_IsVec4(*d))
                    v.push_back(std::string(Cmd_Argv(4)));

                return Dvar_SetFromString(*d, DVAR_FLAG_NONE, v);
            }
        }
    }
    return false;
}
