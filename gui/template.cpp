#include "piano_roll.h"

#include <iostream>

PianoRoll::PianoRoll() {
}

Message PianoRoll::update\(GlobalCargo *globals) {
}

void PianoRoll::draw(SDL_Surface *window_surface) {
    const SDL_Color COLOR_GRAY = {100,100,100};
    
    SDL_Rect r = SDL_Rect{x, y, w, h};
    SDL_FillRect(window_surface, &r, COLOR_GRAY);
}

PianoRoll::~PianoRoll() {
}