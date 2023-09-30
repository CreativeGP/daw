#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "component.h"
#include "util.h"


class Window : public Component {
public:
    Window(int, int, int, int, TTF_Font *, std::string &&, Component *inner_component);
    ~Window();

    Message update(GlobalCargo *globals);
    void draw(SDL_Renderer *);


    bool has_error() { return this->error; }
public:
    int /*x, y, */w, h;
    int innerx, innery;
    // w, hなどはinner_componentと共有したいが、メンバに対して参照を作ってそれを他の人にも見てもらうことは難しいため、メッセージでやりとりする
    Component *inner_component;

    bool active = false;

private:

    const int title_bar_height = 20;
    const int title_margin_x = 10;
    const int title_margin_y = 2;
    std::string title;

    bool dragged;
    int drag_delta_x, drag_delta_y;

    void draw_label(SDL_Renderer *, std::string str, SDL_Rect r, SDL_Color col);
    TTF_Font *font;

    bool error = false;
};

#endif