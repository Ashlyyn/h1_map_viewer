#pragma once

#include "com_defs.hpp"
#include "gfx_defs.hpp"
#include "cl.hpp"

struct cg_t {
	float fov;
	float sensitivity;
	GfxCamera camera;
	bool hasKbmFocus;
	bool active;
	RectDef viewport;
	float nearPlane, farPlane;
	KeyFocus keyfocus;
};

void  CG_Init();
cg_t& CG_GetLocalClientGlobals(int localClientNum);
void  CG_MoveForward (int localClientNum, float vel);
void  CG_MoveBackward(int localClientNum, float vel);
void  CG_StrafeLeft  (int localClientNum, float vel);
void  CG_StrafeRight (int localClientNum, float vel);
void  CG_Ascend      (int localClientNum, float vel);
void  CG_Descend     (int localClientNum, float vel);
	  
void  CG_Frame(uint64_t deltaTime);
void  CG_Shutdown();