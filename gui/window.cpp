#include "window.h"

#include <iostream>
#include <map>


Window::Window(int x, int y, int w, int h, TTF_Font *font, std::string &&title, Component *inner_component) :
    w(w), h(h), font(font), inner_component(inner_component)

{
    this->x = x;
    this->y = y;
    
    innerx = 0;
    innery = 0;

    dragged = false;

    this->title = title;
}

Message Window::update(GlobalCargo *globals) {
    bool mouse_in_region = (x < globals->mousex && globals->mousex < x+w)
     && (y < globals->mousey && globals->mousey < y+title_bar_height);

    inner_component.update(globals);

    if (globals->click[SDL_BUTTON_LEFT]) {
        if (mouse_in_region) {
            dragged = true;
            drag_delta_x = globals->mousex-x;
            drag_delta_y = globals->mousey-y;
        }
    }

    if (globals->release[SDL_BUTTON_LEFT])
        dragged = false;

    if (dragged) {
        int relx = x;
        int rely = y;
        x = globals->mousex - drag_delta_x;
        y = globals->mousey - drag_delta_y;
        relx = x-relx;
        rely = y-rely;

        inner_component->x += relx;
        inner_component->y += rely;

        std::cout << inner_component->x << std::endl;
    }

    // 変数の共有ってどうやるのが一番いいんだろーねー
    if (mouse_in_region && globals->mouseaction) {
        this->active = true;
        return MSG_ACTIVATED;
    }
}


void Window::
draw_label(SDL_Renderer *ren, std::string str, SDL_Rect r, SDL_Color col) {
    SDL_Surface *tmp_surface = TTF_RenderText_Solid(font, str.c_str(), col);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, tmp_surface);
    if (r.w == -1) r.w = ((float)r.h/tmp_surface->h) * tmp_surface->w;
    if (r.h == -1) r.h = ((float)r.w/tmp_surface->w) * tmp_surface->h;
    SDL_RenderCopy(ren, tex, 0, &r);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(tmp_surface);
}

void Window::draw(SDL_Renderer *ren) {    
    // bg
    SDL_Rect r = SDL_Rect{x, y, w, h};
    if (dragged)
        SDL_SetRenderDrawColor(ren, 80, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(ren, 255,255,255, 255);
    SDL_RenderFillRect(ren, &r);

    // title bar
    r = SDL_Rect{x, y, w, title_bar_height};
    SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
    SDL_RenderFillRect(ren, &r);

    // title
    draw_label(ren, title.c_str(),
            SDL_Rect{x+title_margin_x, y+title_margin_y, -1, title_bar_height-title_margin_y*2},
            SDL_Color{200,200,200,255});

    inner_component->draw(ren);
}

Window::~Window() {
}




// Note::Note() {}
// Note::~Note() {}

// Message Note::update\(GlobalCargo *globals);
// void Note::draw(SDL_Renderer *ren) {
    
// }
