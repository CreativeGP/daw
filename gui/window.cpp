#include "window.h"

#include <iostream>
#include <map>


Window::Window(int x, int y, int w, int h, TTF_Font *font, std::string &&title, Component *inner_component) :
    w(w), h(h), font(font)
{
    if (inner_component) {
        this->inner_component = inner_component;
        inner_component->parent = this;
    }
    
    this->x = x;
    this->y = y;
    
    innerx = 0;
    innery = 0;

    this->title = title;

    // 当たり判定の処理
    coll_titlebar.origin[0] = std::make_pair(ORIGIN_PARENT_BOX, this);
    coll_titlebar.fixed = true;
    coll_titlebar.f_on_click = [](GlobalCargo *globals){};
    coll_titlebar.f_on_drag = [&](GlobalCargo *globals, int relx, int rely){
        this->x += relx;
        this->y += rely;
        this->inner_component->move(relx, rely);
        // this->set_collision();
    };

    coll_right.origin[0] = std::make_pair(ORIGIN_PARENT_BOX, this);
    coll_right.f_on_click = [](GlobalCargo *globals){};
    coll_right.f_on_drag = [&](GlobalCargo *globals, int relx, int rely){
        this->w += relx;
        this->inner_component->resize(this->w, this->h - this->title_bar_height);
        this->set_collision();
    };

    coll_left.origin[0] = std::make_pair(ORIGIN_PARENT_BOX, this);
    coll_left.f_on_click = [](GlobalCargo *globals){};
    coll_left.f_on_drag = [&](GlobalCargo *globals, int relx, int rely){
        this->x += relx;
        this->w -= relx;
        
        this->inner_component->move_to(this->x, this->y + this->title_bar_height);
        this->inner_component->resize(this->w, this->h - this->title_bar_height);
        this->set_collision();
    };

    coll_bottom.origin[0] = std::make_pair(ORIGIN_PARENT_BOX, this);
    coll_bottom.f_on_click = [](GlobalCargo *globals){};
    coll_bottom.f_on_drag = [&](GlobalCargo *globals, int relx, int rely){
        this->h += rely;
        this->inner_component->resize(this->w, this->h - this->title_bar_height);
        this->set_collision();
    };

    set_collision();
}

void Window::set_collision() {
    const int width = 10;
    
    coll_titlebar.x = 0;
    coll_titlebar.y = 0;
    coll_titlebar.w = w;
    coll_titlebar.h = title_bar_height; 

    coll_right.x = w-5;
    coll_right.y = 0;
    coll_right.w = width;
    coll_right.h = h; 

    coll_left.x = 0;
    coll_left.y = 0;
    coll_left.w = width;
    coll_left.h = h; 

    coll_bottom.x = 0;
    coll_bottom.y = h-width;
    coll_bottom.w = w;
    coll_bottom.h = width; 
    
}

Message Window::update(GlobalCargo *globals) {
    bool mouse_in_region = (x < globals->mousex && globals->mousex < x+w)
     && (y < globals->mousey && globals->mousey < y+title_bar_height);

    inner_component->update(globals);

    coll_titlebar.update(globals);
    coll_right.update(globals);
    coll_left.update(globals);
    coll_bottom.update(globals);

    // if (resized) {
    //     inner_component->resize(...); 的な
    // }x

    // こちらのほうで、リサイズ処理をした場合はinner_componentを強制的に書き換える
    // クリックがあったら自分をactiveにして、色を変える、inner_componentにもイベントを流すようにする.
    // 上方にメッセージをおくり、
    if (mouse_in_region && globals->mouseaction) {
        this->active = true;
        return MSG_ACTIVATED;
    }

    return MSG_OK;
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
    if (coll_titlebar.is_dragged()) {
        SDL_SetRenderDrawColor(ren, 80, 0, 0, 255);
    } else {
        SDL_SetRenderDrawColor(ren, 255,255,255, 255);
    }
    SDL_RenderFillRect(ren, &r);

    // title bar
    r = SDL_Rect{x, y, w, title_bar_height};
    if (this->active)
        SDL_SetRenderDrawColor(ren, 50, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
    SDL_RenderFillRect(ren, &r);

    // title
    draw_label(ren, title.c_str(),
            SDL_Rect{x+title_margin_x, y+title_margin_y, -1, title_bar_height-title_margin_y*2},
            SDL_Color{200,200,200,255});

    inner_component->draw(ren);


    coll_titlebar.draw(ren);
    coll_right.draw(ren);
    coll_left.draw(ren);
    coll_bottom.draw(ren);

    
}

Window::~Window() {
}




// Note::Note() {}
// Note::~Note() {}

// Message Note::update(GlobalCargo *globals);
// void Note::draw(SDL_Renderer *ren) {
    
// }
