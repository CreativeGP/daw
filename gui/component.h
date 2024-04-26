#ifndef COMPONENT_H
#define COMPONENT_H


#include <string>

#include <SDL2/SDL.h>
#include "util.h"

enum Message {
    MSG_OK = 0,
    MSG_RESIZED,
    MSG_ACTIVATED,
    MSG_DEACTIVATED,
};

struct GlobalCargo;

class Component {
public:
    virtual Message update(GlobalCargo *globals) = 0;
    virtual void draw(/*SDL_Surface *surface*/ SDL_Renderer *) = 0;

    void move(int relx, int rely) { x += relx; y += rely; }
    void move_to(int x, int y) { this->x = x; this->y = y; }
    void resize(int w, int h) { this->w = w; this->h = h; }

    int x, y, w, h;
    int innerx, innery; // 面倒なのでここにおかせて

    Component *parent; // これも、良い設計かはわからんが. collのoriginの設定に使える.

    std::string name; // TODO: uuidのほうがいい？

    bool active = false;
private:
};

class Label : public Component {

};

#endif
