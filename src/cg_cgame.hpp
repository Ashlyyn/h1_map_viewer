#pragma once

#include "com_defs.hpp"
#include "gfx_defs.hpp"

void CG_Init();
void CG_MoveForward(float vel);
void CG_MoveBackward(float vel);
void CG_StrafeLeft(float vel);
void CG_StrafeRight(float vel);
void CG_Ascend(float vel);
void CG_Descend(float vel);

void CG_Frame(uint64_t deltaTime);
NO_DISCARD GfxCamera& CG_Camera();
NO_DISCARD float CG_Fov();
void CG_Shutdown();