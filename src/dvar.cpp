#include <unordered_map>

#include "dvar.hpp"
#include "com_print.hpp"
#include "cmd_commands.hpp"

void Dvar_Init() {
	Cmd_AddCommand("set",  Dvar_Set_f);
	Cmd_AddCommand("seta", Dvar_SetA_f);
}

bool Dvar_GetBool(const dvar_t& d) {
	if (d.type != DVAR_TYPE_BOOL)
		return false;
	return d.value.b;
}

int Dvar_GetInt(const dvar_t& d) {
	if (d.type != DVAR_TYPE_INT)
		return 0;
	return d.value.i;
}

int Dvar_GetFloat(const dvar_t& d) {
	if (d.type != DVAR_TYPE_FLOAT)
		return 0;
	return d.value.f;
}

std::string_view Dvar_GetString(const dvar_t& d) {
	if (d.type != DVAR_TYPE_STRING)
		return std::string_view();
	return d.e[0];
}

std::string_view Dvar_GetEnum(const dvar_t& d) {
	if (d.type != DVAR_TYPE_ENUM)
		return std::string_view();
	return d.e[d.value.e];
}

glm::vec2 Dvar_GetVec2(const dvar_t& d) {
	if (d.type != DVAR_TYPE_VEC2)
		return glm::vec2(0.0f, 0.0f);
	return d.value.v2;
}

glm::vec3 Dvar_GetVec3(const dvar_t& d) {
	if (d.type != DVAR_TYPE_VEC3)
		return glm::vec3(0.0f, 0.0f, 0.0f);
	return d.value.v3;
}

glm::vec4 Dvar_GetVec4(const dvar_t& d) {
	if (d.type != DVAR_TYPE_VEC4)
		return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	return d.value.v4;
}

void Dvar_SetBool(dvar_t& d, bool b) {
	if (d.type == DVAR_TYPE_BOOL)
		d.value.b = b;
}

void Dvar_SetInt(dvar_t& d, int i) {
	if (d.type == DVAR_TYPE_INT &&
		(i <= d.domain.imax && i >= d.domain.imax)
	) {
		d.value.i = i;
	}
}

void Dvar_SetFloat(dvar_t& d, float f) {
	if (d.type == DVAR_TYPE_FLOAT &&
		(f <= d.domain.fmax && f >= d.domain.fmin)
	) {
		d.value.f = f;
	}
}

void Dvar_SetString(dvar_t& d, const std::string& s) {
	if (d.type == DVAR_TYPE_STRING)
		d.e[0] = s;
}

// TODO - SetEnum

void Dvar_SetVec2(dvar_t& d, glm::vec2 v) {
	if (d.type == DVAR_TYPE_VEC2 &&
		(v.x <= d.domain.fmax && v.x >= d.domain.fmin) &&
		(v.y <= d.domain.fmax && v.y >= d.domain.fmin)
	) {
		d.value.v2 = v;
	}
}

void Dvar_SetVec3(dvar_t& d, glm::vec3 v) {
	if (d.type == DVAR_TYPE_VEC3 &&
		(v.x <= d.domain.fmax && v.x >= d.domain.fmin) &&
		(v.y <= d.domain.fmax && v.y >= d.domain.fmin) &&
		(v.z <= d.domain.fmax && v.z >= d.domain.fmin)
	) {
		d.value.v3 = v;
	}
}

void Dvar_SetVec4(dvar_t& d, glm::vec4 v) {
	if (d.type == DVAR_TYPE_VEC4 &&
		(v.x <= d.domain.fmax && v.x >= d.domain.fmin) &&
		(v.y <= d.domain.fmax && v.y >= d.domain.fmin) &&
		(v.z <= d.domain.fmax && v.z >= d.domain.fmin) &&
		(v.w <= d.domain.fmax && v.w >= d.domain.fmin)
	) {
		d.value.v4 = v;
	}
}

void Dvar_AddFlags(dvar_t& d, dvarFlags_t flags) {
	d.flags = (dvarFlags_t)((int)d.flags | flags);
}

std::unordered_map<std::string, dvar_t> s_dvars;

bool Dvar_Exists(const std::string& name) {
	return s_dvars.contains(name);
}

dvar_t* Dvar_Find(const std::string& name) {
	return &s_dvars.find(name)->second;
}

bool Dvar_RegisterNewBool(const std::string& name, dvarFlags_t flags, bool value, OUT dvar_t*& d) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);

	*d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewInt(const std::string& name, dvarFlags_t flags, int value, int min, int max) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_RegisterNewFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_RegisterNewString(const std::string& name, dvarFlags_t flags, std::string value) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);
	return true;
}

bool Dvar_RegisterNewEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, domain);
	return true;
}

bool Dvar_RegisterNewVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_RegisterNewVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_RegisterNewVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max) {
	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_ReregisterBool(const std::string& name, dvarFlags_t flags, bool value, OUT dvar_t*& d) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value);

	*d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterInt(const std::string& name, dvarFlags_t flags, int value, int min, int max) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_ReregisterFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_ReregisterString(const std::string& name, dvarFlags_t flags, std::string value) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);
	return true;
}

bool Dvar_ReregisterEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, domain);
	return true;
}

bool Dvar_ReregisterVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_ReregisterVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);
	return true;
}

bool Dvar_ReregisterVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max) {
	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);
	return true;
}

dvar_t& Dvar_RegisterBool(const std::string& name, dvarFlags_t flags, bool value) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterBool(name, flags, value, d);
	else
		Dvar_RegisterNewBool(name, flags, value, d);

	return *d;
}

void Dvar_RegisterInt(const std::string& name, dvarFlags_t flags, int value, int min, int max) {
	if (Dvar_Exists(name))
		Dvar_ReregisterInt(name, flags, value, min, max);
	else
		Dvar_RegisterNewInt(name, flags, value, min, max);
}

void Dvar_RegisterFloat(const std::string& name, dvarFlags_t flags, float value, float min, float max) {
	if (Dvar_Exists(name))
		Dvar_ReregisterFloat(name, flags, value, min, max);
	else
		Dvar_RegisterNewFloat(name, flags, value, min, max);
}

void Dvar_RegisterString(const std::string& name, dvarFlags_t flags, const std::string& value) {
	if (Dvar_Exists(name))
		Dvar_ReregisterString(name, flags, value);
	else
		Dvar_RegisterNewString(name, flags, value);
}

void Dvar_RegisterEnum(const std::string& name, dvarFlags_t flags, int value, const std::vector<std::string>& domain) {
	if (Dvar_Exists(name))
		Dvar_ReregisterEnum(name, flags, value, domain);
	else
		Dvar_RegisterNewEnum(name, flags, value, domain);
}

void Dvar_RegisterVec2(const std::string& name, dvarFlags_t flags, const glm::vec2& value, float min, float max) {
	if (Dvar_Exists(name))
		Dvar_ReregisterVec2(name, flags, value, min, max);
	else
		Dvar_RegisterNewVec2(name, flags, value, min, max);
}

void Dvar_RegisterVec3(const std::string& name, dvarFlags_t flags, const glm::vec3& value, float min, float max) {
	if (Dvar_Exists(name))
		Dvar_ReregisterVec3(name, flags, value, min, max);
	else
		Dvar_RegisterNewVec3(name, flags, value, min, max);
}

void Dvar_RegisterVec4(const std::string& name, dvarFlags_t flags, const glm::vec4& value, float min, float max) {
	if (Dvar_Exists(name))
		Dvar_ReregisterVec4(name, flags, value, min, max);
	else
		Dvar_RegisterNewVec4(name, flags, value, min, max);
}

bool Dvar_Unregister(const std::string& name) {
	return s_dvars.erase(name) == 1;
}

void Dvar_ClearDvars() {
	s_dvars.clear();
}

bool Dvar_SetFromString(dvar_t& dvar, dvarFlags_t flags, const std::vector<std::string>& v) {
	dvar.flags = flags;

	if (v.size() < 1)
		return false;

	glm::vec4 v4;
	if (v.size() >= 4 && Com_Parse({ v[0], v[1], v[2], v[3] }, v4)) {
		dvar.type = DVAR_TYPE_VEC4;
		dvar.value.v4 = v4;
		dvar.domain.fmin = FLT_MIN;
		dvar.domain.fmax = FLT_MAX;
		return true;
	}

	glm::vec3 v3;
	if (v.size() >= 3 && Com_Parse( { v[0], v[1], v[2] }, v3)) {
		dvar.type = DVAR_TYPE_VEC3;
		dvar.value.v3 = v3;
		dvar.domain.fmin = FLT_MIN;
		dvar.domain.fmax = FLT_MAX;
		return true;
	}

	glm::vec2 v2;
	if (v.size() >= 2 && Com_Parse( { v[0], v[1] }, v2)) {
		dvar.type = DVAR_TYPE_VEC2;
		dvar.value.v2 = v2;
		dvar.domain.fmin = FLT_MIN;
		dvar.domain.fmax = FLT_MAX;
		return true;
	}

	float f;
	if (Com_Parse(v[0], f)) {
		dvar.type = DVAR_TYPE_FLOAT;
		dvar.value.f = f;
		dvar.domain.fmin = FLT_MIN;
		dvar.domain.fmax = FLT_MAX;
		return true;
	}

	int i;
	if (Com_Parse(v[0], i)) {
		dvar.type = DVAR_TYPE_INT;
		dvar.value.i = i;
		dvar.domain.imin = INT_MIN;
		dvar.domain.imax = INT_MAX;
		return true;
	}

	bool b;
	if (Com_Parse(v[0], b)) {
		dvar.type = DVAR_TYPE_BOOL;
		dvar.value.b = b;
		return true;
	}

	return false;
}

bool Dvar_SetFromString(dvar_t& dvar, dvarFlags_t flags, std::string_view value) {
	std::vector<std::string> v;
	if (!Com_Split(value, v))
		return false;

	return Dvar_SetFromString(dvar, flags, v);
}

bool Dvar_RegisterNewFromString(const std::string& name, dvarFlags_t flags, std::string_view value) {
	if (s_dvars.contains(name))
		return false;

	dvar_t d;
	Dvar_SetFromString(d, flags, value);

	s_dvars[name] = d;
	return true;
}

bool Dvar_RegisterNewFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v) {
	if (s_dvars.contains(name))
		return false;

	dvar_t d;
	Dvar_SetFromString(d, flags, v);

	s_dvars[name] = d;
	return true;
}

bool Dvar_ReregisterFromString(const std::string& name, dvarFlags_t flags, std::string_view value) {
	if (!s_dvars.contains(name))
		return false;

	return Dvar_SetFromString(s_dvars[name], flags, value);
}

bool Dvar_ReregisterFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v) {
	if (!s_dvars.contains(name))
		return false;

	return Dvar_SetFromString(s_dvars[name], flags, v);
}

void Dvar_RegisterFromString(const std::string& name, dvarFlags_t flags, std::string_view value) {
	if (Dvar_Exists(name))
		Dvar_ReregisterFromString(name, flags, value);
	else
		Dvar_RegisterNewFromString(name, flags, value);
}

void Dvar_RegisterFromString(const std::string& name, dvarFlags_t flags, const std::vector<std::string>& v) {
	if (Dvar_Exists(name))
		Dvar_ReregisterFromString(name, flags, v);
	else
		Dvar_RegisterNewFromString(name, flags, v);
}

void Dvar_Set_f() {
	int argc = Cmd_Argc();
	if (argc < 3) {
		Com_Println(CON_DEST_OUT, "USAGE: set <variable> <value>");
		return;
	}

	std::string name = std::string(Cmd_Argv(1));
	
	std::vector<std::string> v;

	if (argc == 3)
		v.push_back(std::string(Cmd_Argv(2)));
	if (argc == 4)
		v.push_back(std::string(Cmd_Argv(3)));
	if (argc == 5)
		v.push_back(std::string(Cmd_Argv(4)));
	if (argc == 6)
		v.push_back(std::string(Cmd_Argv(5)));

	dvar_t* d = Dvar_Find(name);
	if (d) {
		if (!Dvar_SetFromString(*d, v))
			Dvar_ReregisterFromString(name, (dvarFlags_t)0, v);
	}

	Dvar_RegisterFromString(name, (dvarFlags_t)0, v);
}

void Dvar_SetA_f() {
	int argc = Cmd_Argc();
	if (argc < 3) {
		Com_Println(CON_DEST_OUT, "USAGE: seta <variable> <value>");
		return;
	}

	Dvar_Set_f();
	std::string name = std::string(Cmd_Argv(1));
	dvar_t* d = Dvar_Find(name);
	Dvar_AddFlags(*d, DVAR_FLAG_ARCHIVE);
}

void Dvar_Shutdown() {
	Cmd_RemoveCommand("set");
	Cmd_RemoveCommand("seta");
}