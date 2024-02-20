#include "pm_pmove.hpp"

#include <algorithm>
#include <array>

#include "m_math.hpp"

std::array<pm_t, 4> s_pm;

pm_t& PM_GetLocalClientGlobals(int localClientNum) {
	return s_pm.at(localClientNum);
}

void PM_Init() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		pm_t& pm = PM_GetLocalClientGlobals(i);
		pm.pm.ps = &pm.ps;
	}
}

static void PM_Accelerate(
	INOUT pmove_t& pm, const pml_t& pml,
	const glm::vec3& wishdir, float wishspeed, float accel
) {
	float currentspeed = glm::dot(pm.ps->velocity, wishdir);
	float addspeed = wishspeed - currentspeed;

	if (addspeed <= 0)
		return;

	float accelspeed = accel * pml.frametime * wishspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	pm.ps->velocity += wishdir * accelspeed;
}

static void PM_NoclipMove(INOUT pmove_t& pm, const pml_t& pml) {
	float speed = glm::length(pm.ps->velocity);
	if (speed < 1.0f) {
		pm.ps->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	} else {
		float friction = PM_FRICTION * 1.5f;
		float control = speed < PM_STOPSPEED ? PM_STOPSPEED : speed;
		float drop = control * friction * pml.frametime;

		float newspeed = speed - drop;
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;

		pm.ps->velocity *= newspeed;
	}

	glm::vec3 wishvel = pml.forward * pm.cmd.vel.z - pml.right * pm.cmd.vel.x;
	wishvel.y += pm.cmd.vel.y;

	glm::vec3 wishdir = glm::normalize(wishvel);
	float wishspeed = glm::length(wishvel);
	if (wishvel == glm::vec3(0.0f, 0.0f, 0.0f)) {
		wishdir = glm::vec3(0.0f, 0.0f, 0.0f);
		wishspeed = 0;
	}

	PM_Accelerate(pm, pml, wishdir, wishspeed, PM_ACCELERATE);

	pm.ps->origin += pm.ps->velocity * pml.frametime;
}

void PM_UpdateViewAngles(INOUT playerState_t& ps, const usercmd_t& cmd) {
	if (ps.pm_type != PM_NOCLIP)
		return;

	ps.viewyaw   = ps.deltayaw  + cmd.yaw;
	ps.viewroll  = ps.deltaroll + cmd.roll;
	ps.viewpitch = std::clamp(ps.deltapitch + cmd.pitch, -89.0f, 89.0f);
}

void PmoveSingle(INOUT pmove_t& pm, INOUT pml_t& pml) {
	memset(&pml, 0, sizeof(pml));

	pml.msec = std::clamp((uint64_t)((float)pm.cmd.serverTime - (float)pm.ps->commandTime), (uint64_t)1, (uint64_t)200);
	pm.ps->commandTime = pm.cmd.serverTime;
	pml.frametime = pml.msec * 0.001f;

	PM_UpdateViewAngles(*pm.ps, pm.cmd);
	M_AngleVectors(pm.ps->viewyaw, pm.ps->viewpitch, pm.ps->viewroll, &pml.forward, &pml.right, &pml.up);

	if (pm.cmd.vel.y < 10.0f)
		pm.ps->pm_flags &= ~PMF_JUMP_HELD;

	if (pm.ps->pm_type == PM_NOCLIP) {
		PM_NoclipMove(pm, pml);
	}

	memset(&pm.cmd, 0, sizeof(pm.cmd));
}