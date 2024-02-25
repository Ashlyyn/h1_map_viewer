#pragma once

void M_AngleVectors(
	float yaw, float pitch, float roll, OPTIONAL_OUT glm::vec3* forward,
	OPTIONAL_OUT glm::vec3* right, OPTIONAL_OUT glm::vec3* up
);

template<typename T>
NO_DISCARD T inline M_atan(T t) {
	return atan(t);
}

template<typename T>
NO_DISCARD T inline M_tan(T t) {
	return tan(t);
}