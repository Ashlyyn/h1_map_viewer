#include "in_input.hpp"

#include <array>
#include <unordered_map>
#include <string>
#include <vector>

#include <cassert>
#include <cstring>

#include "cl_client.hpp"

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

struct inl_t {
	std::unordered_map<SDL_Keycode, Key> keys;
	std::vector<SDL_Keycode>  keysPressedOnCurrentFrame;
	Mouse mouse;
};

static std::array<inl_t, 4> s_in;

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

static inl_t& IN_GetLocalClientLocals(int localClientNum) {
	return s_in.at(localClientNum);
}

void IN_Init() {
	IN_Key_Init();
	IN_Mouse_Init();
}

void IN_Key_Init() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++)
		IN_GetLocalClientLocals(i).keys.clear();
}

NO_DISCARD bool IN_Key_IsDown(int localClientNum, SDL_Keycode k) {
	return IN_GetLocalClientLocals(localClientNum).keys[k].down == true;
}

NO_DISCARD bool IN_Key_IsUp(int localClientNum, SDL_Keycode k) {
	return IN_GetLocalClientLocals(localClientNum).keys[k].down == false;
}

NO_DISCARD bool IN_Key_IsToggled(int localClientNum, SDL_Keycode k) {
	return IN_GetLocalClientLocals(localClientNum).keys[k].toggle;
}

NO_DISCARD bool IN_Key_WasPressedOnCurrentFrame(int localClientNum, SDL_Keycode k) {
	return IN_GetLocalClientLocals(localClientNum).keys[k].justDown;
}

NO_DISCARD std::span<int> IN_Key_AllPressedOnCurrentFrame(int localClientNum) {
	return std::span(IN_GetLocalClientLocals(localClientNum).keysPressedOnCurrentFrame);
}

bool IN_Key_Down(int localClientNum, SDL_Keycode k) {
	bool b = IN_Key_IsDown(localClientNum, k);

	inl_t& inl = IN_GetLocalClientLocals(localClientNum);
	auto& key = inl.keys[k];
	key.down = true;
	key.justDown = true;
	key.toggle = !key.toggle;
	inl.keysPressedOnCurrentFrame.push_back(k);
	return b;
}

bool IN_Key_Up(int localClientNum, SDL_Keycode k) {
	bool b = IN_Key_IsUp(localClientNum, k);
	IN_GetLocalClientLocals(localClientNum).keys[k].down = false;
	return b;
}

char IN_Key_Char(int localClientNum, SDL_Keycode k) {
	bool shift = IN_Key_IsDown(localClientNum, SDLK_LSHIFT) || 
				 IN_Key_IsDown(localClientNum, SDLK_RSHIFT);
	if (IN_Key_IsToggled(localClientNum, SDLK_CAPSLOCK))
		shift = !shift;

	return IN_Key_SDLKToChar(k, shift);
}

std::string_view IN_Key_GetBinding(int localClientNum, SDL_Keycode k, bool secondary) {
	if (secondary)
		return IN_GetLocalClientLocals(localClientNum).keys[k].bind2;
	else
		return IN_GetLocalClientLocals(localClientNum).keys[k].bind;
}

SDL_Keycode IN_Key_GetNum(int localClientNum, std::string_view binding) {
	for (const auto& c : IN_GetLocalClientLocals(localClientNum).keys)
		if (c.second.bind == binding || c.second.bind2 == binding)
			return c.first;

	return -1;
}

void IN_Key_Frame() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		if (!CL_HasKbmFocus(i))
			continue;

		inl_t& inl = IN_GetLocalClientLocals(i);
		for (auto& c : inl.keys)
			c.second.justDown = false;

		inl.keysPressedOnCurrentFrame.clear();
	}
}

void IN_Key_Shutdown() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		inl_t& inl = IN_GetLocalClientLocals(i);
		inl.keys.clear();
		inl.keysPressedOnCurrentFrame.clear();
	}
}

void IN_Mouse_Init() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		inl_t& inl = IN_GetLocalClientLocals(i);
		inl.mouse.x = (float)vid_width  / 2.0f;
		inl.mouse.y = (float)vid_height / 2.0f;
	}
}

NO_DISCARD bool IN_Mouse_IsDown(int localClientNum, Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	return IN_GetLocalClientLocals(localClientNum).mouse.buttons.array[i] == true;
}

NO_DISCARD bool IN_Mouse_IsUp(int localClientNum, Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	return IN_GetLocalClientLocals(localClientNum).mouse.buttons.array[i] == false;
}

bool IN_Mouse_Down(int localClientNum, Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	bool b = IN_Mouse_IsDown(localClientNum, button);
	IN_GetLocalClientLocals(localClientNum).mouse.buttons.array[i] = true;
	return b;
}

bool IN_Mouse_Up(int localClientNum, Uint8 button) {
	int i = IN_Mouse_SDLButtonToIndex(button);
	bool b = IN_Mouse_IsDown(localClientNum, button);
	IN_GetLocalClientLocals(localClientNum).mouse.buttons.array[i] = false;
	return b;
}

void IN_Mouse_Move(int localClientNum, float x, float y) {
	IN_GetLocalClientLocals(localClientNum).mouse.x -= x;
	IN_GetLocalClientLocals(localClientNum).mouse.y -= y;
}

NO_DISCARD float IN_Mouse_X(int localClientNum) {
	return IN_GetLocalClientLocals(localClientNum).mouse.x;
}

NO_DISCARD float IN_Mouse_Y(int localClientNum) {
	return IN_GetLocalClientLocals(localClientNum).mouse.y;
}

void IN_Mouse_Frame() {

}

void IN_Mouse_Shutdown() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		inl_t& inl = IN_GetLocalClientLocals(i);
		inl.mouse = Mouse{};
		inl.mouse.x = (float)vid_width / 2.0f;
		inl.mouse.y = (float)vid_height / 2.0f;
	}
}

void IN_Frame() {
	IN_Key_Frame();
	IN_Mouse_Frame();
}

void IN_Shutdown() {
	IN_Key_Shutdown();
	IN_Mouse_Shutdown();
}
