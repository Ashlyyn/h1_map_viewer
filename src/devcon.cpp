#include "devcon.hpp"

#include <iostream>
#include <string>

#include <cstdio>

#include <SDL3/SDL.h>

#include "com_print.hpp"

std::string devcon_buffer;
bool devcon_hasText;

SDL_Mutex* devcon_textMutex;

int DevCon_Thread(void*) {
	DevCon_Init();
	for (;;) {
		if (!devcon_hasText) {
			std::getline(std::cin, devcon_buffer);
			devcon_hasText = true;
		}
	}
}

void DevCon_Init() {
	devcon_buffer.resize(4 * 1024);
	Com_DPrintln("Hello from DevCon!");
	devcon_textMutex = SDL_CreateMutex();
}

NO_DISCARD bool DevCon_HasText() {
	return devcon_hasText;
}

NO_DISCARD std::string DevCon_TakeText() {
	SDL_LockMutex(devcon_textMutex);
	devcon_hasText = false;
	std::string s = std::move(devcon_buffer);
	devcon_buffer = std::string();
	SDL_UnlockMutex(devcon_textMutex);
	return s;
}

void DevCon_Shutdown() {
	SDL_DestroyMutex(devcon_textMutex);
	devcon_buffer.clear();
	devcon_hasText = false;
}