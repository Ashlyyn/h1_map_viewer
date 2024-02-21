#include "gfx_backend.hpp"

#include <cstdio>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include "dvar.hpp"
#include "gfx.hpp"
#include "sys.hpp"

extern SDL_Window* g_sdlWindow;
extern dvar_t* r_vsync;
extern dvar_t* r_fullscreen;
extern dvar_t* r_noBorder;

static SDL_GLContext s_glContext;

void RB_Init() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
    );
#if _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // _DEBUG
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    s_glContext = SDL_GL_CreateContext(g_sdlWindow);
    if (s_glContext == nullptr) {
        printf("GL context creation failed: %s\n", SDL_GetError());
        Sys_NormalExit(-1);
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("GLEW init failed: %s", glewGetErrorString(err));
        Sys_NormalExit(-2);
    }
}

extern dvar_t* vid_width;
extern dvar_t* vid_height;
extern dvar_t* vid_xpos;
extern dvar_t* vid_ypos;

bool RB_EnableVsync(bool enable) {
    return SDL_GL_SetSwapInterval((int)enable) == 0;
}

void RB_BeginFrame() {
    if (Dvar_WasModified(*r_vsync)) {
        bool enable = Dvar_GetBool(*r_vsync);
        if (!RB_EnableVsync(enable)) {
            Com_Println(
                CON_DEST_ERR, "Failed to {} vsync: {}",
                enable ? "enable" : "disable", SDL_GetError()
            );
        }
    }

    if (Dvar_WasModified(*r_fullscreen)) {
        if (Dvar_GetBool(*r_fullscreen)) {
            Dvar_LatchValue(*vid_width);
            Dvar_LatchValue(*vid_height);
            Dvar_LatchValue(*vid_xpos);
            Dvar_LatchValue(*vid_ypos);
            SDL_DisplayID d = SDL_GetDisplayForWindow(g_sdlWindow);
            const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(d);
            SDL_SetWindowSize(g_sdlWindow, mode->w, mode->h + 1);
            Dvar_SetInt(*vid_width, mode->w);
            Dvar_SetInt(*vid_height, mode->h);
            R_WindowResized();
            SDL_SetWindowPosition(g_sdlWindow, 0, 0);
            SDL_SetWindowResizable(g_sdlWindow, SDL_FALSE);
            Dvar_SetBool(*r_fullscreen, true);
        }
        else {
            Dvar_SetBool(*r_fullscreen, false);
            SDL_SetWindowResizable(g_sdlWindow, SDL_TRUE);
            Dvar_RestoreValue(*vid_xpos);
            Dvar_RestoreValue(*vid_ypos);
            Dvar_RestoreValue(*vid_width);
            Dvar_RestoreValue(*vid_height);
            SDL_SetWindowPosition(g_sdlWindow, Dvar_GetInt(*vid_xpos), Dvar_GetInt(*vid_ypos));
            SDL_SetWindowSize(g_sdlWindow, Dvar_GetInt(*vid_width), Dvar_GetInt(*vid_height));
            R_WindowResized();
        }

        Dvar_ClearModified(*r_fullscreen);
    }

    if (Dvar_WasModified(*r_noBorder)) {
        if (Dvar_GetBool(*r_noBorder))
            SDL_SetWindowBordered(g_sdlWindow, SDL_FALSE);
        else
            SDL_SetWindowBordered(g_sdlWindow, SDL_TRUE);

        Dvar_ClearModified(*r_noBorder);
    }
}

void RB_EndFrame() {
    SDL_GL_SwapWindow(g_sdlWindow);
}

void RB_Shutdown() {
    SDL_GL_DeleteContext(s_glContext);
}