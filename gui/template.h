#ifndef PIANO_ROLL_H
#define PIANO_ROLL_H

#include <string>
#include <SDL2/SDL.h>

#include "component.h"

class PianoRoll : public Component {
public:
    PianoRoll();
    ~PianoRoll();

    Message update(GlobalCargo *globals);
    void draw(SDL_Surface *surface);

    bool has_error() { return this->error; }
public:
    int /*x, y, */w, h;

private:
    
    bool error = false;
};

#endif