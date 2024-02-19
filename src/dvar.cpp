#include "dvar.hpp"

#include <unordered_map>

#include "cmd_commands.hpp"
#include "com_print.hpp"

enum DvarType {
	DVAR_TYPE_NONE,
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
	struct { int   min, max; } i;
	struct { float min, max; } f;
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

struct dvar_t {
	DvarType    type;
	DvarDomain  domain;
	DvarValue   value;
	dvarFlags_t flags;
	std::vector<std::string> e;

	inline dvar_t() {
		type = DVAR_TYPE_NONE;
		domain.i.min = 0;
		domain.i.max = 0;
		value.b = false;
		flags = DVAR_FLAG_NONE;
		e = std::vector<std::string>();
	}

	inline dvar_t(dvarFlags_t flags, bool value) {
		type = DVAR_TYPE_BOOL;
		this->value.b = value;
		e.push_back(Com_Format("{}", value));
		this->flags = flags;
	}

	inline dvar_t(
		dvarFlags_t flags, int value, int min = INT_MIN, int max = INT_MAX
	) {
		type = DVAR_TYPE_INT;
		this->flags = flags;
		domain.i.min = min;
		domain.i.max = max;
		e.push_back(Com_Format("{}", value));

		Dvar_SetInt(*this, value);
	}

	inline dvar_t(
		dvarFlags_t flags, float value, float min = FLT_MIN, float max = FLT_MAX
	) {
		type = DVAR_TYPE_FLOAT;
		domain.f.min = min;
		domain.f.max = max;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));

		Dvar_SetFloat(*this, value);
	}

	inline dvar_t(dvarFlags_t flags, std::string value) {
		type = DVAR_TYPE_STRING;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));

		Dvar_SetString(*this, value);
	}

	inline dvar_t(
		dvarFlags_t flags, int value, const std::vector<std::string>& domain
	) {
		type = DVAR_TYPE_ENUM;
		this->value.e = value;
		this->e = domain;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));
	}

	inline dvar_t(
		dvarFlags_t flags, glm::vec2 value,
		float min = FLT_MIN, float max = FLT_MAX
	) {
		type = DVAR_TYPE_VEC2;
		domain.f.min = min;
		domain.f.max = max;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));

		Dvar_SetVec2(*this, value);
	}

	inline dvar_t(
		dvarFlags_t flags, glm::vec3 value,
		float min = FLT_MIN, float max = FLT_MAX
	) {
		type = DVAR_TYPE_VEC3;
		domain.f.min = min;
		domain.f.max = max;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));

		Dvar_SetVec3(*this, value);
	}

	inline dvar_t(
		dvarFlags_t flags, glm::vec4 value,
		float min = FLT_MIN, float max = FLT_MAX
	) {
		type = DVAR_TYPE_VEC4;
		domain.f.min = min;
		domain.f.max = max;
		this->flags = flags;
		e.push_back(Com_Format("{}", value));

		Dvar_SetVec4(*this, value);
	}

	inline ~dvar_t() noexcept {

	}

	dvar_t& operator=(const dvar_t&) = default;
};

void Dvar_Init() {
	Cmd_AddCommand("set",  Dvar_Set_f);
	Cmd_AddCommand("seta", Dvar_SetA_f);
}

NO_DISCARD bool Dvar_IsBool(const dvar_t& d) {
	return d.type == DVAR_TYPE_BOOL;
}

NO_DISCARD bool Dvar_IsInt(const dvar_t& d) {
	return d.type == DVAR_TYPE_INT;
}

NO_DISCARD bool Dvar_IsFloat(const dvar_t& d) {
	return d.type == DVAR_TYPE_FLOAT;
}

NO_DISCARD bool Dvar_IsString(const dvar_t& d) {
	return d.type == DVAR_TYPE_STRING;
}

NO_DISCARD bool Dvar_IsEnum(const dvar_t& d) {
	return d.type == DVAR_TYPE_ENUM;
}

NO_DISCARD bool Dvar_IsVec2(const dvar_t& d) {
	return d.type == DVAR_TYPE_VEC2;
}

NO_DISCARD bool Dvar_IsVec3(const dvar_t& d) {
	return d.type == DVAR_TYPE_VEC3;
}

NO_DISCARD bool Dvar_IsVec4(const dvar_t& d) {
	return d.type == DVAR_TYPE_VEC4;
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

float Dvar_GetFloat(const dvar_t& d) {
	if (d.type != DVAR_TYPE_FLOAT)
		return 0;
	return d.value.f;
}

std::string_view Dvar_GetString(const dvar_t& d) {
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

void Dvar_SetBool(INOUT dvar_t& d, bool b) {
	if (d.type == DVAR_TYPE_BOOL) {
		d.value.b = b;
		d.e[0] = Com_Format("{}", b);
	}
}

void Dvar_SetInt(INOUT dvar_t& d, int i) {
	if (d.type == DVAR_TYPE_INT &&
		(i <= d.domain.i.max && i >= d.domain.i.max)
	) {
		d.value.i = i;
		d.e[0] = Com_Format("{}", i);
	}
}

void Dvar_SetFloat(INOUT dvar_t& d, float f) {
	if (d.type == DVAR_TYPE_FLOAT &&
		(f <= d.domain.f.max && f >= d.domain.f.min)
	) {
		d.value.f = f;
		d.e[0] = Com_Format("{}", f);
	}
}

void Dvar_SetString(INOUT dvar_t& d, const std::string& s) {
	if (d.type == DVAR_TYPE_STRING)
		d.e[0] = s;
}

// TODO - SetEnum

void Dvar_SetVec2(INOUT dvar_t& d, const glm::vec2& v) {
	if (d.type == DVAR_TYPE_VEC2 &&
		(v.x <= d.domain.f.max && v.x >= d.domain.f.min) &&
		(v.y <= d.domain.f.max && v.y >= d.domain.f.min)
	) {
		d.value.v2 = v;
		d.e[0] = Com_Format("{}", v);
	}
}

void Dvar_SetVec3(INOUT dvar_t& d, const glm::vec3& v) {
	if (d.type == DVAR_TYPE_VEC3 &&
		(v.x <= d.domain.f.max && v.x >= d.domain.f.min) &&
		(v.y <= d.domain.f.max && v.y >= d.domain.f.min) &&
		(v.z <= d.domain.f.max && v.z >= d.domain.f.min)
	) {
		d.value.v3 = v;
		d.e[0] = Com_Format("{}", v);
	}
}

void Dvar_SetVec4(INOUT dvar_t& d, const glm::vec4& v) {
	if (d.type == DVAR_TYPE_VEC4 &&
		(v.x <= d.domain.f.max && v.x >= d.domain.f.min) &&
		(v.y <= d.domain.f.max && v.y >= d.domain.f.min) &&
		(v.z <= d.domain.f.max && v.z >= d.domain.f.min) &&
		(v.w <= d.domain.f.max && v.w >= d.domain.f.min)
	) {
		d.value.v4 = v;
		d.e[0] = Com_Format("{}", v);
	}
}

void Dvar_AddFlags(INOUT dvar_t& d, dvarFlags_t flags) {
	d.flags = (dvarFlags_t)((int)d.flags | flags);
}

std::unordered_map<std::string, dvar_t> s_dvars;

bool Dvar_Exists(const std::string& name) {
	return s_dvars.contains(name);
}

dvar_t* Dvar_Find(const std::string& name) {
	auto d = s_dvars.find(name);
	if (d == s_dvars.cend())
		return nullptr;

	return &d->second;
}

bool Dvar_RegisterNewBool(
	const std::string& name, dvarFlags_t flags, bool value, OUT dvar_t*& d
) {
	d = nullptr; 

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewInt(
	const std::string& name, dvarFlags_t flags, 
	int value, int min, int max, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewFloat(
	const std::string& name, dvarFlags_t flags, 
	float value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewString(
	const std::string& name, dvarFlags_t flags, 
	std::string value, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewEnum(
	const std::string& name, dvarFlags_t flags, int value, 
	const std::vector<std::string>& domain, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, domain);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewVec2(
	const std::string& name, dvarFlags_t flags, const glm::vec2& value, 
	float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_RegisterNewVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterBool(
	const std::string& name, dvarFlags_t flags, 
	bool value, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterInt(
	const std::string& name, dvarFlags_t flags, 
	int value, int min, int max, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterFloat(
	const std::string& name, dvarFlags_t flags, 
	float value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterString(
	const std::string& name, dvarFlags_t flags, 
	std::string value, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterEnum(
	const std::string& name, dvarFlags_t flags, 
	int value, const std::vector<std::string>& domain, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars[name] = dvar_t(flags, value, domain);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterVec2(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec2& value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

bool Dvar_ReregisterVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max, OUT dvar_t*& d
) {
	d = nullptr;

	if (!Dvar_Exists(name))
		return false;

	s_dvars.at(name) = dvar_t(flags, value, min, max);

	d = Dvar_Find(name);
	return true;
}

dvar_t& Dvar_RegisterBool(
	const std::string& name, dvarFlags_t flags, bool value
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterBool(name, flags, value, d);
	else
		Dvar_RegisterNewBool(name, flags, value, d);

	return *d;
}

dvar_t& Dvar_RegisterInt(
	const std::string& name, dvarFlags_t flags, int value, int min, int max
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterInt(name, flags, value, min, max, d);
	else
		Dvar_RegisterNewInt(name, flags, value, min, max, d);

	return *d;
}

dvar_t& Dvar_RegisterFloat(
	const std::string& name, dvarFlags_t flags, float value, float min, float max
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterFloat(name, flags, value, min, max, d);
	else
		Dvar_RegisterNewFloat(name, flags, value, min, max, d);

	return *d;
}

dvar_t& Dvar_RegisterString(
	const std::string& name, dvarFlags_t flags, const std::string& value
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterString(name, flags, value, d);
	else
		Dvar_RegisterNewString(name, flags, value, d);

	return *d;
}

dvar_t& Dvar_RegisterEnum(
	const std::string& name, dvarFlags_t flags, 
	int value, const std::vector<std::string>& domain
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterEnum(name, flags, value, domain, d);
	else
		Dvar_RegisterNewEnum(name, flags, value, domain, d);

	return *d;
}

dvar_t& Dvar_RegisterVec2(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec2& value, float min, float max
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterVec2(name, flags, value, min, max, d);
	else
		Dvar_RegisterNewVec2(name, flags, value, min, max, d);

	return *d;
}

dvar_t& Dvar_RegisterVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterVec3(name, flags, value, min, max, d);
	else
		Dvar_RegisterNewVec3(name, flags, value, min, max, d);

	return *d;
}

dvar_t& Dvar_RegisterVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max
) {
	dvar_t* d;

	if (Dvar_Exists(name))
		Dvar_ReregisterVec4(name, flags, value, min, max, d);
	else
		Dvar_RegisterNewVec4(name, flags, value, min, max, d);

	return *d;
}

bool Dvar_Unregister(const std::string& name) {
	return s_dvars.erase(name) == 1;
}

void Dvar_ClearDvars() {
	s_dvars.clear();
}

bool Dvar_SetFromString(
	INOUT dvar_t& dvar, dvarFlags_t flags, const std::deque<std::string>& v
) {
	dvar.flags = flags;

	if (v.size() < 1)
		return false;

	glm::vec4 v4;
	if (v.size() >= 4 && Com_Parse({ v[0], v[1], v[2], v[3] }, v4)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_VEC4;
			dvar.domain.f.min = FLT_MIN;
			dvar.domain.f.max = FLT_MAX;
			Dvar_SetVec4(dvar, v4);
			return true;
		} else if (dvar.type == DVAR_TYPE_VEC4) {
			Dvar_SetVec4(dvar, v4);
			return true;
		} else {
			return false;
		}
	}

	glm::vec3 v3;
	if (v.size() >= 3 && Com_Parse({ v[0], v[1], v[2] }, v3)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_VEC3;
			dvar.domain.f.min = FLT_MIN;
			dvar.domain.f.max = FLT_MAX;
			Dvar_SetVec3(dvar, v3);
			return true;
		} else if (dvar.type == DVAR_TYPE_VEC3) {
			Dvar_SetVec3(dvar, v3);
			return true;
		} else {
			return false;
		}
	}

	glm::vec2 v2;
	if (v.size() >= 2 && Com_Parse({ v[0], v[1] }, v2)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_VEC2;
			dvar.domain.f.min = FLT_MIN;
			dvar.domain.f.max = FLT_MAX;
			Dvar_SetVec2(dvar, v2);
			return true;
		} else if (dvar.type == DVAR_TYPE_VEC2) {
			Dvar_SetVec2(dvar, v2);
			return true;
		} else {
			return false;
		}
	}

	bool b;
	if (v.size() >= 1 && Com_Parse(v[0], b)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_BOOL;
			Dvar_SetBool(dvar, b);
			return true;
		}
		else if (dvar.type == DVAR_TYPE_BOOL) {
			Dvar_SetBool(dvar, b);
			return true;
		}
		else {
			return false;
		}
	}

	int i;
	if (v.size() >= 1 && Com_Parse(v[0], i)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_INT;
			dvar.domain.i.min = INT_MIN;
			dvar.domain.i.max = INT_MAX;
			Dvar_SetInt(dvar, i);
			return true;
		} else if (dvar.type == DVAR_TYPE_INT) {
			Dvar_SetInt(dvar, i);
			return true;
		} else {
			return false;
		}
	}

	float f;
	if (v.size() >= 1 && Com_Parse(v[0], f)) {
		if (dvar.type == DVAR_TYPE_NONE) {
			dvar.type = DVAR_TYPE_FLOAT;
			dvar.domain.f.min = FLT_MIN;
			dvar.domain.f.max = FLT_MAX;
			Dvar_SetFloat(dvar, f);
			return true;
		}
		else if (dvar.type == DVAR_TYPE_FLOAT) {
			Dvar_SetFloat(dvar, f);
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

bool Dvar_SetFromString(
	INOUT dvar_t& dvar, dvarFlags_t flags, std::string_view value
) {
	std::deque<std::string> v;
	if (!Com_Split(value, v))
		return false;

	return Dvar_SetFromString(dvar, flags, v);
}

bool Dvar_RegisterNewFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
) {
	if (s_dvars.contains(name))
		return false;

	dvar_t d;
	Dvar_SetFromString(d, flags, value);

	s_dvars[name] = d;
	return true;
}

bool Dvar_RegisterNewFromString(
	const std::string& name, dvarFlags_t flags, const std::deque<std::string>& v
) {
	if (s_dvars.contains(name))
		return false;

	dvar_t d;
	Dvar_SetFromString(d, flags, v);

	s_dvars[name] = d;
	return true;
}

bool Dvar_ReregisterFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
) {
	if (!s_dvars.contains(name))
		return false;

	return Dvar_SetFromString(s_dvars[name], flags, value);
}

bool Dvar_ReregisterFromString(
	const std::string& name, dvarFlags_t flags, const std::deque<std::string>& v
) {
	if (!s_dvars.contains(name))
		return false;

	return Dvar_SetFromString(s_dvars[name], flags, v);
}

void Dvar_RegisterFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
) {
	if (Dvar_Exists(name))
		Dvar_ReregisterFromString(name, flags, value);
	else
		Dvar_RegisterNewFromString(name, flags, value);
}

void Dvar_RegisterFromString(
	const std::string& name, dvarFlags_t flags, const std::deque<std::string>& v
) {
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
	
	std::deque<std::string> v;

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
		if (!Dvar_SetFromString(*d, DVAR_FLAG_NONE, v))
			Dvar_ReregisterFromString(name, DVAR_FLAG_NONE, v);
	} else {
		Dvar_RegisterFromString(name, DVAR_FLAG_NONE, v);
	}
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