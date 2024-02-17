#include <cstdio>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include "sys.hpp"

extern SDL_Window* g_sdlWindow;

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
    SDL_GL_SetSwapInterval(0);

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

void RB_BeginFrame() {

}

void RB_EndFrame() {
    SDL_GL_SwapWindow(g_sdlWindow);
}

void RB_Shutdown() {
    SDL_GL_DeleteContext(s_glContext);
}