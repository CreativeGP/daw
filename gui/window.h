#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "component.h"
#include "util.h"
#include "collision_detector.h"

class Window : public Component {
public:
    Window(int, int, int, int, TTF_Font *, std::string &&, Component *inner_component);
    ~Window();

    Message update(GlobalCargo *globals);
    void draw(SDL_Renderer *);

    bool has_error() { return this->error; }
public:
    int /*virtual x, y, */w, h;
    int innerx, innery;
    // w, hなどはinner_componentと共有したいが、メンバに対して参照を作ってそれを他の人にも見てもらうことは難しいため、メッセージでやりとりする
    Component *inner_component;

    /*virtual bool active = false;*/

private:
    void set_collision();

    const int title_bar_height = 20;
    const int title_margin_x = 10;
    const int title_margin_y = 2;
    std::string title;

    CollisionDetector coll_titlebar{true}, coll_right{true}, coll_left{true}, coll_bottom{true};

    void draw_label(SDL_Renderer *, std::string str, SDL_Rect r, SDL_Color col);
    TTF_Font *font;

    bool error = false;
};

#endif
