#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "com_print.hpp"

void Dvar_Init();
void Dvar_Shutdown();

enum dvarFlags_t {
	DVAR_FLAG_NONE    = 0x00000000,
	DVAR_FLAG_ARCHIVE = 0x00000001,
};

struct dvar_t;

NO_DISCARD bool Dvar_WasModified(const dvar_t& d);
void Dvar_SetModified(INOUT dvar_t& d);
void Dvar_ClearModified(INOUT dvar_t& d);

NO_DISCARD bool Dvar_IsBool  (const dvar_t& d);
NO_DISCARD bool Dvar_IsInt   (const dvar_t& d);
NO_DISCARD bool Dvar_IsFloat (const dvar_t& d);
NO_DISCARD bool Dvar_IsString(const dvar_t& d);
NO_DISCARD bool Dvar_IsEnum  (const dvar_t& d);
NO_DISCARD bool Dvar_IsVec2  (const dvar_t& d);
NO_DISCARD bool Dvar_IsVec3  (const dvar_t& d);
NO_DISCARD bool Dvar_IsVec4  (const dvar_t& d);

void Dvar_LatchValue(INOUT dvar_t& d);
bool Dvar_RestoreValue(INOUT dvar_t& d);
bool Dvar_HasLatchedValue(const dvar_t& d);

void Dvar_SetBool  (INOUT dvar_t& d, bool b);
void Dvar_SetInt   (INOUT dvar_t& d, int i);
void Dvar_SetFloat (INOUT dvar_t& d, float f);
void Dvar_SetString(INOUT dvar_t& d, const std::string& s);
// TODO - SetEnum
void Dvar_SetVec2(INOUT dvar_t& d, const glm::vec2& v);
void Dvar_SetVec3(INOUT dvar_t& d, const glm::vec3& v);
void Dvar_SetVec4(INOUT dvar_t& d, const glm::vec4& v);

NO_DISCARD bool             Dvar_GetBool  (const dvar_t& d);
NO_DISCARD int              Dvar_GetInt   (const dvar_t& d);
NO_DISCARD float            Dvar_GetFloat (const dvar_t& d);
NO_DISCARD std::string_view Dvar_GetString(const dvar_t& d);
NO_DISCARD std::string_view Dvar_GetEnum  (const dvar_t& d);
NO_DISCARD glm::vec2        Dvar_GetVec2  (const dvar_t& d);
NO_DISCARD glm::vec3        Dvar_GetVec3  (const dvar_t& d);
NO_DISCARD glm::vec4        Dvar_GetVec4  (const dvar_t& d);

void Dvar_AddFlags(INOUT dvar_t& d, dvarFlags_t flags);

NO_DISCARD bool    Dvar_Exists(const std::string& name);
NO_DISCARD dvar_t* Dvar_Find  (const std::string& name);

dvar_t* Dvar_RegisterNewBool(
	const std::string& name, dvarFlags_t flags, 
	bool value
);
dvar_t* Dvar_RegisterNewInt(
	const std::string& name, dvarFlags_t flags, 
	int value, int min, int max
);
dvar_t* Dvar_RegisterNewFloat(
	const std::string& name, dvarFlags_t flags, 
	float value, float min, float max
);
dvar_t* Dvar_RegisterNewString(
	const std::string& name, dvarFlags_t flags, 
	std::string value
);
dvar_t* Dvar_RegisterNewEnum(
	const std::string& name, dvarFlags_t flags, int value, 
	const std::vector<std::string>& domain
);
dvar_t* Dvar_RegisterNewVec2(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec2& value, float min, float max
);
dvar_t* Dvar_RegisterNewVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max
);
dvar_t* Dvar_RegisterNewVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max
);

dvar_t* Dvar_ReregisterBool(
	const std::string& name, dvarFlags_t flags, 
	bool value
);
dvar_t* Dvar_ReregisterInt(
	const std::string& name, dvarFlags_t flags, 
	int value, int min, int max
);
dvar_t* Dvar_ReregisterFloat(
	const std::string& name, dvarFlags_t flags, 
	float value, float min, float max
);
dvar_t* Dvar_ReregisterString(
	const std::string& name, dvarFlags_t flags, 
	std::string value
);
dvar_t* Dvar_ReregisterEnum(
	const std::string& name, dvarFlags_t flags, 
	int value, const std::vector<std::string>& domain
);
dvar_t* Dvar_ReregisterVec2(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec2& value, float min, float max
);
dvar_t* Dvar_ReregisterVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max
);
dvar_t* Dvar_ReregisterVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max
);

dvar_t& Dvar_RegisterBool(
	const std::string& name, dvarFlags_t flags, bool value
);
dvar_t& Dvar_RegisterInt(
	const std::string& name, dvarFlags_t flags, 
	int value, int min, int max
);
dvar_t& Dvar_RegisterFloat(
	const std::string& name, dvarFlags_t flags, 
	float value, float min, float max
);
dvar_t& Dvar_RegisterString(
	const std::string& name, dvarFlags_t flags, 
	const std::string& value
);
dvar_t& Dvar_RegisterEnum(
	const std::string& name, dvarFlags_t flags, 
	int value, const std::vector<std::string>& domain
);
dvar_t& Dvar_RegisterVec2(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec2& value, float min, float max
);
dvar_t& Dvar_RegisterVec3(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec3& value, float min, float max
);
dvar_t& Dvar_RegisterVec4(
	const std::string& name, dvarFlags_t flags, 
	const glm::vec4& value, float min, float max
);

bool Dvar_Unregister(const std::string& name);
void Dvar_ClearDvars();

NO_DISCARD bool Dvar_LocalExists(int localClientNum, const std::string& name);
NO_DISCARD dvar_t* Dvar_FindLocal(int localClientNum, const std::string& name);

dvar_t* Dvar_RegisterNewLocalBool(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	bool value
);
dvar_t* Dvar_RegisterNewLocalInt(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	int value, int min, int max
);
dvar_t* Dvar_RegisterNewLocalFloat(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	float value, float min, float max
);
dvar_t* Dvar_RegisterNewLocalString(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	std::string value
);
dvar_t* Dvar_RegisterNewLocalEnum(int localClientNum,
	const std::string& name, dvarFlags_t flags, int value,
	const std::vector<std::string>& domain
	);
dvar_t* Dvar_RegisterNewLocalVec2(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec2& value, float min, float max
);
dvar_t* Dvar_RegisterNewLocalVec3(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec3& value, float min, float max
);
dvar_t* Dvar_RegisterNewLocalVec4(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec4& value, float min, float max
);

dvar_t* Dvar_ReregisterLocalBool(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	bool value
);
dvar_t* Dvar_ReregisterLocalInt(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	int value, int min, int max
);
dvar_t* Dvar_ReregisterLocalFloat(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	float value, float min, float max
);
dvar_t* Dvar_ReregisterLocalString(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	std::string value
);
dvar_t* Dvar_ReregisterLocalEnum(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	int value, const std::vector<std::string>& domain
);
dvar_t* Dvar_ReregisterLocalVec2(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec2& value, float min, float max
);
dvar_t* Dvar_ReregisterLocalVec3(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec3& value, float min, float max
);
dvar_t* Dvar_ReregisterLocalVec4(int localClientNum,
	const std::string& name, dvarFlags_t flags,
	const glm::vec4& value, float min, float max
);

dvar_t* Dvar_RegisterLocalBool(
	int localClientNum, const std::string& name, dvarFlags_t flags, bool value
);
dvar_t* Dvar_RegisterLocalInt(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	int value, int min, int max
);
dvar_t* Dvar_RegisterLocalFloat(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	float value, float min, float max
);
dvar_t* Dvar_RegisterLocalString(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	const std::string& value
);
dvar_t* Dvar_RegisterLocalEnum(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	int value, const std::vector<std::string>& domain
);
dvar_t* Dvar_RegisterLocalVec2(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	const glm::vec2& value, float min, float max
);
dvar_t* Dvar_RegisterLocalVec3(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	const glm::vec3& value, float min, float max
);
dvar_t* Dvar_RegisterLocalVec4(
	int localClientNum, const std::string& name, dvarFlags_t flags,
	const glm::vec4& value, float min, float max
);

bool Dvar_UnregisterLocal(int localClientNum, const std::string& name);
void Dvar_ClearLocalDvars(int localClientNum);

bool Dvar_SetFromString(
	INOUT dvar_t& dvar, dvarFlags_t flags, 
	const std::deque<std::string>& v
);
bool Dvar_SetFromString(
	INOUT dvar_t& dvar, dvarFlags_t flags, std::string_view value
);
dvar_t* Dvar_RegisterNewFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
);
dvar_t* Dvar_RegisterNewFromString(
	const std::string& name, dvarFlags_t flags, 
	const std::deque<std::string>& v
);
dvar_t* Dvar_ReregisterFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
);
dvar_t* Dvar_ReregisterFromString(
	const std::string& name, dvarFlags_t flags, 
	const std::deque<std::string>& v
);
dvar_t& Dvar_RegisterFromString(
	const std::string& name, dvarFlags_t flags, std::string_view value
);
dvar_t& Dvar_RegisterFromString(
	const std::string& name, dvarFlags_t flags, 
	const std::deque<std::string>& v
);

void Dvar_Set_f();
void Dvar_SetA_f();