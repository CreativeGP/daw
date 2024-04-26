#ifndef __UTIL_H__
#define __UTIL_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <map>

#include "generator.h" /* 相互参照 */

#define AUDIO_THREAD

class Instrument;
class SF2Instrument;

struct GlobalCargo {
    std::map<std::string, TTF_Font *> fonts;
    bool click[5], dblclick[5], release[5], pressing[5];
    bool mouseaction = false;
    Sint32 mousex, mousey;
    Sint32 mouse_xrel, mouse_yrel;
    float wheelx, wheely;

    bool key[256], key_pressing[256];
    Uint16 mod;

    int BPM = 120;
    double seek_key = 0;
    double seek_step = (double)BPM / 60.0f / 60.0f;
    bool Playing = false;

    Instrument *inst1;
    SF2Instrument *inst2;

    SDL_AudioSpec Desired;
    SDL_AudioSpec Obtained;

};


#endif
