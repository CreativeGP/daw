#ifndef __UTIL_H__
#define __UTIL_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>

#include <map>

typedef struct {
    std::map<std::string, TTF_Font *> fonts;
    bool click[5], dblclick[5], release[5], pressing[5];
    bool mouseaction = false;
    Sint32 mousex, mousey;
    Sint32 mouse_xrel, mouse_yrel;
    float wheelx, wheely;

    bool key[256], key_pressing[256];
    Uint16 mod;

} GlobalCargo;

enum Message {
    MSG_RESIZED,
    MSG_ACTIVATED,
};

#endif