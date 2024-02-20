#pragma once

void M_AngleVectors(
	float yaw, float pitch, float roll, OPTIONAL_OUT glm::vec3* forward,
	OPTIONAL_OUT glm::vec3* right, OPTIONAL_OUT glm::vec3* up
);