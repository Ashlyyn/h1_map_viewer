#include <cstdint>

#include "gfx.hpp"
#include "cg_cgame.hpp"
#include "cl.hpp"
#include "devgui.hpp"
#include "devcon.hpp"
#include "cmd_commands.hpp"
#include "dvar.hpp"
#include "con_console.hpp"
#include "input.hpp"

void Sys_Init();
bool Sys_HandleEvent();
void R_Init();
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
    CG_Init();
    R_Init();
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

    IN_Frame();

    while (Sys_HandleEvent())
        ;

    CG_Frame(s_deltaTime);
    CL_Frame();
    DevGui_Frame();
    for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
        cg_t& cg = CG_GetLocalClientGlobals(i);
        if (!cg.active)
            continue;

        if (DevGui_HasText(i))
            Con_ProcessInput(DevGui_TakeText(i));

        R_DrawFrame(i);
    }
    
    if (DevCon_HasText())
        Con_ProcessInput(DevCon_TakeText());

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
