#include <array>
#include <vector>
#include <unordered_map>
#include <string>
#include <span>

#include <cassert>
#include <cstring>

#include "input.hpp"

extern int vid_width, vid_height;

union MouseButtons {
	struct { bool ml, mm, mr, m4, m5; } m;
	bool array[5];
};

struct Mouse {
	MouseButtons buttons;
	float x, y;
};

struct Key {
	bool down;
	bool toggle;
	bool justDown;
	std::string bind;
	std::string bind2;
};

static std::array<
	std::unordered_map<SDL_Keycode, Key>, MAX_LOCAL_CLIENTS
> s_playerKeys;

static std::vector<SDL_Keycode> s_keysPressedOnCurrentFrame;

static std::array<Mouse, MAX_LOCAL_CLIENTS> s_playerMouse;

static constexpr char IN_Key_SDLKToChar(SDL_Keycode k, bool shift) {
	switch (k) {
	case SDLK_BACKQUOTE:    return shift ? '~'  : '`';
	case SDLK_1:            return shift ? '!'  : '1';
	case SDLK_2:            return shift ? '@'  : '2';
	case SDLK_3:            return shift ? '#'  : '3';
	case SDLK_4:            return shift ? '$'  : '4';
	case SDLK_5:            return shift ? '%'  : '5';
	case SDLK_6:            return shift ? '^'  : '6';
	case SDLK_7:            return shift ? '&'  : '7';
	case SDLK_8:            return shift ? '*'  : '8';
	case SDLK_9:            return shift ? '('  : '9';
	case SDLK_0:            return shift ? ')'  : '0';
	case SDLK_MINUS:        return shift ? '_'  : '-';
	case SDLK_EQUALS:       return shift ? '+'  : '=';
						    				    
	case SDLK_TAB:			return '\t';
	case SDLK_q:            return shift ? 'Q'  : 'q';
	case SDLK_w:            return shift ? 'W'  : 'w';
	case SDLK_e:            return shift ? 'E'  : 'e';
	case SDLK_r:            return shift ? 'R'  : 'r';
	case SDLK_t:            return shift ? 'T'  : 't';
	case SDLK_y:            return shift ? 'Y'  : 'y';
	case SDLK_u:            return shift ? 'U'  : 'u';
	case SDLK_i:            return shift ? 'I'  : 'i';
	case SDLK_o:            return shift ? 'O'  : 'o';
	case SDLK_p:            return shift ? 'P'  : 'p';
	case SDLK_LEFTBRACKET:  return shift ? '['  : '{';
	case SDLK_RIGHTBRACKET: return shift ? ']'  : '{';
	case SDLK_BACKSLASH:    return shift ? '\\' : '|';

	case SDLK_a:            return shift ? 'A'  : 'a';
	case SDLK_s:            return shift ? 'S'  : 's';
	case SDLK_d:            return shift ? 'D'  : 'd';
	case SDLK_f:            return shift ? 'F'  : 'f';
	case SDLK_g:            return shift ? 'G'  : 'g';
	case SDLK_h:            return shift ? 'H'  : 'h';
	case SDLK_j:            return shift ? 'J'  : 'j';
	case SDLK_k:            return shift ? 'K'  : 'k';
	case SDLK_l:            return shift ? 'L'  : 'l';
	case SDLK_SEMICOLON:    return shift ? ':'  : ':';
	case SDLK_QUOTE:        return shift ? '\'' : '"';
	case SDLK_RETURN:		return '\n';

	case SDLK_z:            return shift ? 'Z' : 'z';
	case SDLK_x:            return shift ? 'X' : 'x';
	case SDLK_c:            return shift ? 'C' : 'c';
	case SDLK_v:            return shift ? 'V' : 'v';
	case SDLK_b:            return shift ? 'B' : 'b';
	case SDLK_n:            return shift ? 'N' : 'n';
	case SDLK_m:            return shift ? 'M' : 'm';
	case SDLK_COMMA:        return shift ? '<' : ',';
	case SDLK_PERIOD:       return shift ? '>' : '.';
	case SDLK_SLASH:        return shift ? '?' : '/';

	case SDLK_SPACE:		return ' ';
	default:                return 0;
	}
}

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
		return -1;
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

NO_DISCARD bool IN_Key_WasPressedOnCurrentFrame(SDL_Keycode k) {
	return s_playerKeys.at(0)[k].justDown;
}

NO_DISCARD std::span<int> IN_Key_AllPressedOnCurrentFrame() {
	return std::span(s_keysPressedOnCurrentFrame);
}

bool IN_Key_Down(SDL_Keycode k) {
	bool b = IN_Key_IsDown(k);
	auto& key = s_playerKeys.at(0)[k];
	key.down = true;
	key.justDown = true;
	key.toggle = !key.toggle;
	s_keysPressedOnCurrentFrame.push_back(k);
	return b;
}

bool IN_Key_Up(SDL_Keycode k) {
	bool b = IN_Key_IsUp(k);
	s_playerKeys.at(0)[k].down = false;
	return b;
}

char IN_Key_Char(SDL_Keycode k) {
	bool shift = IN_Key_IsDown(SDLK_LSHIFT) || IN_Key_IsDown(SDLK_RSHIFT);
	if (IN_Key_IsToggled(SDLK_CAPSLOCK))
		shift = !shift;

	return IN_Key_SDLKToChar(k, shift);
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

void IN_Key_Frame() {
	for (auto& c : s_playerKeys.at(0))
		c.second.justDown = false;

	s_keysPressedOnCurrentFrame.clear();
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

void IN_Mouse_Frame() {

}

void IN_Mouse_Shutdown() {
	for (auto& c : s_playerMouse)
		memset(&c, 0, sizeof(c));
}

void IN_Frame() {
	IN_Key_Frame();
	IN_Mouse_Frame();
}

void IN_Shutdown() {
	IN_Key_Shutdown();
	IN_Mouse_Shutdown();
}
