#include <cstdint>

#include "gfx.hpp"
#include "cg_cgame.hpp"
#include "cl.hpp"
#include "devgui.hpp"
#include "devcon.hpp"
#include "cmd_commands.hpp"
#include "dvar.hpp"

void Sys_Init();
bool Sys_HandleEvent();
void R_Init();
void R_DrawFrame(uint64_t deltaTime);
void CG_Init();
void CG_Frame(uint64_t deltaTime);
void Font_Init();
uint64_t Sys_Milliseconds();

static uint64_t s_lastFrameTime;
static uint64_t s_deltaTime;

//int com_maxfps = 30;

void Com_Quit_f() {
    Sys_NormalExit(3);
}

bool Com_Init() {
    Cmd_Init();
    Cmd_AddCommand("quit", Com_Quit_f);
    Dvar_Init();
    Font_Init();
    R_Init();
    CG_Init();
    CL_Init();
    CL_EnableFpsCounter(true);
    DevGui_Init();
    s_lastFrameTime = Sys_Milliseconds();
    s_deltaTime = s_lastFrameTime;
    return true;
}

bool Com_Frame() {
    s_deltaTime = Sys_Milliseconds() - s_lastFrameTime;
    s_lastFrameTime = Sys_Milliseconds();

    while (Sys_HandleEvent())
        ;

    CG_Frame(s_deltaTime);
    CL_Frame();
    DevGui_Frame();
    R_DrawFrame(s_deltaTime);

    if (DevCon_HasText()) {
        std::string text = DevCon_TakeText();
        Com_DPrintln("> {}", text);
        Cmd_TakeInput(text);

        std::function<void(void)> fn;
        if (Cmd_GetCommand(Cmd_Argv(0), fn))
            fn();
    }

	return true;
}

uint64_t Com_LastFrameTimeDelta() {
    return s_deltaTime;
}

uint64_t Com_LastFrameTime() {
    return s_lastFrameTime;
}

void Com_Shutdown() {
    DevGui_Shutdown();
    CL_Shutdown();
    CG_Shutdown();
    R_Shutdown();
    Font_Shutdown();
    Dvar_Shutdown();
    Cmd_Shutdown();
}