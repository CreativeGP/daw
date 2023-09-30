#ifndef COMPONENT_H
#define COMPONENT_H


#include <string>

#include <SDL2/SDL.h>
#include "util.h"

class Component {
public:
    virtual Message update(GlobalCargo *globals) = 0;
    virtual void draw(/*SDL_Surface *surface*/ SDL_Renderer *) = 0;

    int x;
    int y;
    std::string name; // TODO: uuidのほうがいい？

    bool active;
private:
};

class Label : public Component {

};

#endif