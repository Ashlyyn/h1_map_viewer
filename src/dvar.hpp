#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "com_defs.hpp"

void Dvar_Init();
void Dvar_Shutdown();

enum DvarType {
	DVAR_TYPE_BOOL,
	DVAR_TYPE_INT,
	DVAR_TYPE_FLOAT,
	DVAR_TYPE_STRING,
	DVAR_TYPE_ENUM,
	DVAR_TYPE_VEC2,
	DVAR_TYPE_VEC3,
	DVAR_TYPE_VEC4
};

union DvarDomain {
	struct { int   imin, imax; };
	struct { float fmin, fmax; };
};

union DvarValue {
	bool b;
	int i;
	int e;
	float f;
	glm::vec2 v2;
	glm::vec3 v3;
	glm::vec4 v4;
};

enum dvarFlags_t {
	DVAR_FLAG_ARCHIVE = 0x00000001,
};

struct dvar_t;
void Dvar_SetInt(dvar_t& d, int i);
void Dvar_SetFloat(dvar_t& d, float f);
void Dvar_SetString(dvar_t& d, const std::string& s);
// TODO - SetEnum
void Dvar_SetVec2(dvar_t& d, glm::vec2 v);
void Dvar_SetVec3(dvar_t& d, glm::vec3 v);
void Dvar_SetVec4(dvar_t& d, glm::vec4 v);

struct dvar_t {
	DvarType    type;
	DvarDomain  domain;
	DvarValue   value;
	dvarFlags_t flags;
	std::vector<std::string> e;

	inline dvar_t() {
		type = DVAR_TYPE_BOOL;
		domain.imin = 0;
		domain.imax = 0;
		value.b = false;
		flags = (dvarFlags_t)0;
		e = std::vector<std::string>();
	}

	inline dvar_t(dvarFlags_t flags, bool value) {
		type          = DVAR_TYPE_BOOL;
		this->value.b = value;
		this->flags   = flags;
	}

	inline dvar_t(dvarFlags_t flags, int value, int min = INT_MIN, int max = INT_MAX) {
		type        = DVAR_TYPE_INT;
		this->flags = flags;
		domain.imin = min;
		domain.imax = max;

		Dvar_SetInt(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, float value, float min = FLT_MIN, float max = FLT_MAX) {
		type          = DVAR_TYPE_FLOAT;
		domain.fmin   = min;
		domain.fmax   = max;
		this->flags   = flags;

		Dvar_SetFloat(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, std::string value) {
		type        = DVAR_TYPE_STRING;
		this->flags = flags;

		Dvar_SetString(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, int value, const std::vector<std::string>& domain) {
		type           = DVAR_TYPE_ENUM;
		this->value.e  = value;
		this->e        = domain;
		this->flags    = flags;
	}

	inline dvar_t(dvarFlags_t flags, glm::vec2 value, float min = FLT_MIN, float max = FLT_MAX) {
		type        = DVAR_TYPE_VEC2;
		domain.fmin = min;
		domain.fmax = max;
		this->flags = flags;

		Dvar_SetVec2(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, glm::vec3 value, float min = FLT_MIN, float max = FLT_MAX) {
		type = DVAR_TYPE_VEC3;
		domain.fmin = min;
		domain.fmax = max;
		this->flags = flags;

		Dvar_SetVec3(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, glm::vec4 value, float min = FLT_MIN, float max = FLT_MAX) {
		type = DVAR_TYPE_VEC4;
		domain.fmin = min;
		domain.fmax = max;
		this->flags = flags;

		Dvar_SetVec4(*this, value);
	}

	inline ~dvar_t() noexcept {

	}

	dvar_t& operator=(const dvar_t&) = default;
};

bool Dvar_GetBool(const dvar_t& d);
int  Dvar_GetInt(const dvar_t& d);
int  Dvar_GetFloat(const dvar_t& d);
std::string_view Dvar_GetString(const dvar_t& d);
std::string_view Dvar_GetEnum(const dvar_t& d);
glm::vec2 Dvar_GetVec2(const dvar_t& d);
glm::vec3 Dvar_GetVec3(const dvar_t& d);
glm::vec4 Dvar_GetVec4(const dvar_t& d);

void Dvar_SetBool(dvar_t& d, bool b);

bool Dvar_Exists(const std::string& name);
dvar_t* Dvar_Find(const std::string& name);

bool Dvar_RegisterNewBool(const std::string& name, dvarFlags_t flags, bool value, OUT dvar_t& d);
bool Dvar_RegisterNewInt(const std::string& name, dvarFlags_t flags, int value, int min, int max);
bool Dvar_RegisterNewFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max);
bool Dvar_RegisterNewString(const std::string& name, dvarFlags_t flags, std::string value);
bool Dvar_RegisterNewEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain);
bool Dvar_RegisterNewVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max);
bool Dvar_RegisterNewVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max);
bool Dvar_RegisterNewVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max);

bool Dvar_ReregisterBool(const std::string& name, dvarFlags_t flags, bool value, OUT dvar_t& d);
bool Dvar_ReregisterInt(const std::string& name, dvarFlags_t flags, int value, int min, int max);
bool Dvar_ReregisterFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max);
bool Dvar_ReregisterString(const std::string& name, dvarFlags_t flags, std::string value);
bool Dvar_ReregisterEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain);
bool Dvar_ReregisterVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max);
bool Dvar_ReregisterVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max);
bool Dvar_ReregisterVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max);

dvar_t& Dvar_RegisterBool(const std::string& name, dvarFlags_t flags, bool value);
void Dvar_RegisterInt(const std::string& name, dvarFlags_t flags, int value, int min, int max);
void Dvar_RegisterFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max);
void Dvar_RegisterString(const std::string& name, dvarFlags_t flags, const std::string& value);
void Dvar_RegisterEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain);
void Dvar_RegisterVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max);
void Dvar_RegisterVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max);
void Dvar_RegisterVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max);

bool Dvar_Unregister(const std::string& name);
void Dvar_ClearDvars();
bool Dvar_SetFromString(dvar_t& dvar, dvarFlags_t flags, const std::vector<std::string>& v);
bool Dvar_SetFromString(dvar_t& dvar, dvarFlags_t flags, std::string_view value);
bool Dvar_RegisterNewFromString(const std::string& name, dvarFlags_t flags, std::string_view value);
bool Dvar_RegisterNewFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v);
bool Dvar_ReregisterFromString(const std::string& name, dvarFlags_t flags, std::string_view value);
bool Dvar_ReregisterFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v);
void Dvar_RegisterFromString(const std::string& name, dvarFlags_t flags, std::string_view value);
void Dvar_RegisterFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v);

void Dvar_Set_f();
void Dvar_SetA_f();