#include <cstdio>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "com_defs.hpp"
#include "gfx.hpp"
#include "sys.hpp"
#include "gfx.hpp"
#include "input.hpp"
#include "devcon.hpp"

SDL_Window* g_sdlWindow;

int vid_xpos, vid_ypos;
int vid_width  = VID_WIDTH_DEFAULT;
int vid_height = VID_HEIGHT_DEFAULT;

static uint64_t s_timeBase;

void Sys_InitThreads() {
    assert(Sys_SpawnDevConThread(DevCon_Thread));
}

void Sys_Init() {
    SDL_Init(SDL_INIT_VIDEO);
        
    s_timeBase = (uint64_t)SDL_GetTicks();

    g_sdlWindow = SDL_CreateWindow(
        "Halo 1 Map Viewer",
        vid_width,
        vid_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDL_GetWindowPosition(g_sdlWindow, &vid_xpos, &vid_ypos);
    SDL_SetRelativeMouseMode(SDL_TRUE);

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
                IN_Key_Down(ev.key.keysym.sym);
            break;
        case SDL_EVENT_KEY_UP:
            IN_Key_Up(ev.key.keysym.sym);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            IN_Mouse_Down(ev.button.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            IN_Mouse_Up(ev.button.button);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            IN_Mouse_Move(ev.motion.xrel, ev.motion.yrel);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            vid_width = ev.window.data1;
            vid_height = ev.window.data2;
            R_WindowResized();
            break;
        case SDL_EVENT_WINDOW_MOVED:
            vid_xpos = ev.window.data1;
            vid_ypos = ev.window.data2;
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

void Sys_DestroyThread(thread_t thread) {
    
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