#include "collision_detector.h"

CollisionDetector::CollisionDetector() {}
CollisionDetector::CollisionDetector(bool fixed) : fixed(fixed) {}
CollisionDetector::~CollisionDetector() {}

Message CollisionDetector::
update(GlobalCargo *globals) {
    int originx = 0;
    int originy = 0;
    for (int i = 0; i < 3; i++) {
        if (!this->origin[i].second) continue;
        if (this->origin[i].first == ORIGIN_PARENT_BOX) {
            originx += this->origin[i].second->x;
            originy += this->origin[i].second->y;
        }
        if (this->origin[i].first == ORIGIN_SCROLLABLE_WINDOW) {
            originx -= this->origin[i].second->innerx;
            originy -= this->origin[i].second->innery;
        }
        if (this->origin[i].first == ORIGIN_SCROLLABLE_Y_ONLY) {
            originy -= this->origin[i].second->innery;
        }
    }

    std::cout << originx << " " << originy << globals->mouse_xrel << " " << globals->mouse_yrel << std::endl;
    
    bool mouse_in_region = (x+originx < globals->mousex && globals->mousex < x+originx+w)
        && (y+originy < globals->mousey && globals->mousey < y+originy+h);

    if (mouse_in_region && globals->mouseaction) {
        this->f_on_click(globals);
    }

    if (globals->click[SDL_BUTTON_LEFT]) {
        if (mouse_in_region) {
            // ドラッグ開始位置を覚えておく
            this->dragged = true;
            this->drag_delta_x = globals->mousex-(x+originx);
            this->drag_delta_y = globals->mousey-(y+originy);
        }
    }    

    if (globals->release[SDL_BUTTON_LEFT])
        this->dragged = false;

    if (this->dragged) {
        // drag_deltaだけの差を保ちながらマウスに着いていく
        if (!fixed) {
            x = globals->mousex - drag_delta_x -originx;
            y = globals->mousey - drag_delta_y -originy;
        }

        // TODO: こちらも絶対にしなくて大丈夫かな？
        this->f_on_drag(globals, globals->mouse_xrel, globals->mouse_yrel);
    }

    return MSG_OK;
}

void CollisionDetector::
draw(SDL_Renderer *ren) {
    int originx = 0;
    int originy = 0;
    for (int i = 0; i < 3; i++) {
        if (!this->origin[i].second) continue;
        if (this->origin[i].first == ORIGIN_PARENT_BOX) {
            originx += this->origin[i].second->x;
            originy += this->origin[i].second->y;
        }
        if (this->origin[i].first == ORIGIN_SCROLLABLE_WINDOW) {
            originx -= this->origin[i].second->innerx;
            originy -= this->origin[i].second->innerx;
        }
    }

    // nothing ...
    SDL_Rect r = SDL_Rect{x+originx, y+originy, w, h};
    SDL_SetRenderDrawColor(ren, 100, 100 ,255, 100);
    SDL_RenderFillRect(ren, &r);
}
