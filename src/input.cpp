#include <array>
#include <unordered_map>
#include <string>

#include <cassert>

#include "input.hpp"

extern int vid_width, vid_height;

constexpr size_t MAX_LOCAL_CLIENTS = 1;

union MouseButtons {
	struct { bool ml, mm, mr, m4, m5; };
	bool array[5];
};

struct Mouse {
	MouseButtons buttons;
	float x, y;
};

struct Key {
	bool down;
	bool toggle;
	std::string bind;
	std::string bind2;
};

static std::array<
	std::unordered_map<SDL_Keycode, Key>, MAX_LOCAL_CLIENTS
> s_playerKeys;

static std::array<Mouse, MAX_LOCAL_CLIENTS> s_playerMouse;

static constexpr int IN_Mouse_SDLButtonToIndex(Uint8 button) {
	switch (button) {
	case SDL_BUTTON_LEFT:
		return 0;
	case SDL_BUTTON_RIGHT:
		return 1;
	case SDL_BUTTON_MIDDLE:
		return 2;
	case SDL_BUTTON_X1:
		return 3;
	case SDL_BUTTON_X2:
		return 4;
	default:
		assert(false);
	}
}

void IN_Init() {
	IN_Key_Init();
	IN_Mouse_Init();
}

void IN_Key_Init() {
	for (auto& c: s_playerKeys)
		c.clear();
}

NO_DISCARD bool IN_Key_IsDown(SDL_Keycode k) {
	return s_playerKeys.at(0)[k].down == true;
}

NO_DISCARD bool IN_Key_IsUp(SDL_Keycode k) {
	return s_playerKeys.at(0)[k].down == false;
}

NO_DISCARD bool IN_Key_IsToggled(SDL_Keycode k) {
	return s_playerKeys.at(0)[k].toggle;
}

bool IN_Key_Down(SDL_Keycode k) {
	bool b = IN_Key_IsDown(k);
	auto& key = s_playerKeys.at(0)[k];
	key.down = true;
	key.toggle = !key.toggle;
	return b;
}

bool IN_Key_Up(SDL_Keycode k) {
	bool b = IN_Key_IsUp(k);
	s_playerKeys.at(0)[k].down = false;
	return b;
}

std::string_view IN_Key_GetBinding(SDL_Keycode k, bool secondary) {
	if (secondary)
		return s_playerKeys.at(0)[k].bind2;
	else
		return s_playerKeys.at(0)[k].bind;
}

SDL_Keycode IN_Key_GetNum(std::string_view binding) {
	for (const auto& c : s_playerKeys.at(0))
		if (c.second.bind == binding || c.second.bind2 == binding)
			return c.first;

	return -1;
}

void IN_Key_Shutdown() {
	for (auto& c : s_playerKeys)
		c.clear();
}

void IN_Mouse_Init() {
	for (auto& c : s_playerMouse) {
		memset(&c, 0, sizeof(c));
		c.x = (float)vid_width / 2.0f;
		c.y = (float)vid_height / 2.0f;
	}
}

NO_DISCARD bool IN_Mouse_IsDown(Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	return s_playerMouse.at(0).buttons.array[i] == true;
}

NO_DISCARD bool IN_Mouse_IsUp(Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	return s_playerMouse.at(0).buttons.array[i] == false;
}

bool IN_Mouse_Down(Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	bool b = IN_Mouse_IsDown(button);
	s_playerMouse.at(0).buttons.array[i] = true;
	return b;
}

bool IN_Mouse_Up(Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	bool b = IN_Mouse_IsDown(button);
	s_playerMouse.at(0).buttons.array[i] = false;
	return b;
}

void IN_Mouse_Move(float x, float y) {
	s_playerMouse.at(0).x -= x;
	s_playerMouse.at(0).y -= y;
}

NO_DISCARD float IN_Mouse_X() {
	return s_playerMouse.at(0).x;
}

NO_DISCARD float IN_Mouse_Y() {
	return s_playerMouse.at(0).y;
}

void IN_Mouse_Shutdown() {
	for (auto& c : s_playerMouse)
		memset(&c, 0, sizeof(c));
}

void IN_Shutdown() {
	IN_Key_Shutdown();
	IN_Mouse_Shutdown();
}