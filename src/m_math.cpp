#include <cmath>

#include <glm/glm.hpp>

#include "com_defs.hpp"

void M_AngleVectors(
	float yaw, float pitch, float roll, OPTIONAL_OUT glm::vec3* forward, 
	OPTIONAL_OUT glm::vec3* right, OPTIONAL_OUT glm::vec3* up
) {
	float ay = glm::radians(yaw);
	float sy = sin(ay);
	float cy = cos(ay);
	float ap = glm::radians(pitch);
	float sp = sin(ap);
	float cp = cos(ap);
	float ar = glm::radians(roll);
	float sr = sin(ar);
	float cr = cos(ar);

	if (forward) {
		forward->x = cp * cy;
		forward->y = -sp;
		forward->z = cp * sy;
	}
	if (right) {
		right->x = (-1.0f * sr * sp * cy + -1 * cr * -sy);
		right->y = -1.0f * sr * cp;
		right->z = (-1.0f * sr * sp * sy + -1 * cr * cy);
	}
	if (up) {
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = cr * cp;
		up->z = (cr * sp * sy + -sr * cy);
	}
} 