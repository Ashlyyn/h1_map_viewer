#include <glm/glm.hpp>

#include "com_defs.hpp"

enum pmType_t {
	PM_NOCLIP,
};

enum pmFlags_t {
	PMF_NONE      = 0x00000000,
	PMF_DUCKED    = 0x00000001,
	PMF_JUMP_HELD = 0x00000002,
};

inline void operator&=(pmFlags_t& lhs, int rhs) {
	lhs = (pmFlags_t)((int)lhs | rhs);
}

struct playerState_t {
	pmType_t  pm_type;
	pmFlags_t pm_flags;
	glm::vec3 origin;
	glm::vec3 velocity;
	float     speed;
	float     viewyaw,  viewpitch,  viewroll;
	float     deltayaw, deltapitch, deltaroll;
	uint64_t  commandTime;
};

struct usercmd_t {
	glm::vec3 vel;
	float     roll, yaw, pitch;
	uint64_t  serverTime;
};

struct pmove_t {
	playerState_t* ps;
	usercmd_t      cmd;
};

struct pml_t {
	glm::vec3 forward, right, up;
	float     frametime;
	uint64_t  msec;
};

struct pm_t {
	playerState_t ps;
	pmove_t pm;
	pml_t pml;
};

constexpr float PM_FRICTION   = 6.0f;
constexpr float PM_STOPSPEED  = 100.0f;
constexpr float PM_ACCELERATE = 10.0f;

pm_t& PM_GetLocalClientGlobals(int localClientNum);
void PM_Init();
void PmoveSingle(INOUT pmove_t& pm, INOUT pml_t& pml);