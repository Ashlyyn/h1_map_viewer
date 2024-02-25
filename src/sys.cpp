#include "sys.hpp"

#include <cstdio>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "cl_client.hpp"
#include "devcon.hpp"
#include "dvar.hpp"
#include "gfx.hpp"
#include "in_input.hpp"

SDL_Window* g_sdlWindow;

dvar_t* vid_xpos;
dvar_t* vid_ypos;
dvar_t* vid_width;
dvar_t* vid_height;

static uint64_t s_timeBase;

void Sys_InitThreads() {
    bool b = Sys_SpawnDevConThread(DevCon_Thread);
    assert(b);
}

void Sys_Init() {
    s_timeBase = (uint64_t)SDL_GetTicks();

    SDL_Init(SDL_INIT_VIDEO);

    vid_width  = &Dvar_RegisterInt("vid_width", DVAR_FLAG_NONE, VID_WIDTH_DEFAULT,  1, INT_MAX);
    vid_height = &Dvar_RegisterInt("vid_hight", DVAR_FLAG_NONE, VID_HEIGHT_DEFAULT, 1, INT_MAX);

    g_sdlWindow = SDL_CreateWindow(
        "Halo 1 Map Viewer",
        Dvar_GetInt(*vid_width),
        Dvar_GetInt(*vid_height),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    int x, y;
    SDL_GetWindowPosition(g_sdlWindow, &x, &y);
    vid_xpos = &Dvar_RegisterInt("vid_xpos", DVAR_FLAG_NONE, x, 0, INT_MAX);
    vid_ypos = &Dvar_RegisterInt("vid_ypos", DVAR_FLAG_NONE, y, 0, INT_MAX);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowFullscreenMode(g_sdlWindow, NULL);

    if (g_sdlWindow == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        Sys_NormalExit(-2);
    }

    Sys_InitThreads();
    IN_Init();
}

// Returns true if an event was handled, false if not 
// (most likely, if event queue was empty).
bool Sys_HandleEvent() {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_EVENT_KEY_DOWN:
            if (ev.key.keysym.sym == SDLK_ESCAPE)
                Sys_NormalExit(1);
            else
                IN_Key_Down(CL_ClientWithKbmFocus(), ev.key.keysym.sym);
            break;
        case SDL_EVENT_KEY_UP:
            IN_Key_Up(CL_ClientWithKbmFocus(), ev.key.keysym.sym);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            IN_Mouse_Down(CL_ClientWithKbmFocus(), ev.button.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            IN_Mouse_Up(CL_ClientWithKbmFocus(), ev.button.button);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            IN_Mouse_Move(CL_ClientWithKbmFocus(), ev.motion.xrel, ev.motion.yrel);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            Dvar_SetInt(*vid_width, ev.window.data1);
            Dvar_SetInt(*vid_height, ev.window.data2);
            R_WindowResized();
            break;
        case SDL_EVENT_WINDOW_MOVED:
            Dvar_SetInt(*vid_xpos, ev.window.data1);
            Dvar_SetInt(*vid_ypos, ev.window.data2);
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            SDL_DestroyWindow(g_sdlWindow);
            break;
        case SDL_EVENT_WINDOW_DESTROYED:
            Sys_NormalExit(2);
        }

        return true;
    }

    return false;
}

uint64_t Sys_Milliseconds() {
    return (uint64_t)SDL_GetTicks() - s_timeBase;
}

std::array<SDL_Thread*, 32> sys_hThreads;
std::array<int(*)(void*), 32> sys_threadFuncs;

int Sys_ThreadMain(void* data) {
    return sys_threadFuncs.at((size_t)data)(data);
}

bool Sys_CreateThread(thread_t thread, const std::string& name, int(*f)(void*)) {
    sys_threadFuncs.at(thread) = f;
    SDL_Thread* t = SDL_CreateThread(Sys_ThreadMain, name.c_str(), (void*)thread);
    if (t == nullptr)
        return false;

    sys_hThreads.at(thread) = t;
    return true;
}

void Sys_DestroyThread(thread_t /*thread*/) {
    
}

bool Sys_SpawnDevConThread(int(*DevConThread)(void*)) {
    return Sys_CreateThread(THREAD_DEVCON, "devcon", DevConThread);
}

NO_RETURN Sys_Exit(int ec) {
    exit(ec);
}

void Sys_Shutdown() {
    IN_Shutdown();
    SDL_DestroyWindow(g_sdlWindow);
    SDL_Quit();
}

NO_RETURN Sys_NormalExit(int ec) {
    Com_Shutdown();
    Sys_Shutdown();
    Sys_Exit(ec);
}