#include "project_view.h"

#include <iostream>

/* 
* 枠線の描画
* メロディの描画
*/

ProjectView::ProjectView() {
    x = 0;
    y = 0;
    w = 1000;
    h = 800;

    innerx = 0;
    innery = 0;
}

Message ProjectView::update(GlobalCargo *globals) {
    return MSG_OK;
}

void ProjectView::draw(SDL_Renderer *ren) {
    SDL_Rect r = SDL_Rect{x, y, w, h};
    SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
    SDL_RenderFillRect(ren, &r);
    
    
    // 枠線
    SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
    for (float y =- innery%(gh); y <= h; y += gh) {
        if (y < 0) continue;
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y,
                                this->x + w, this->y + (int)y); // integer mode
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y+1,
                                this->x + w, this->y + (int)y+1); // integer mode
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y-1,
                                this->x + w, this->y + (int)y-1); // integer mode

    }

    // ヘッダー
    SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
    SDL_RenderDrawLine(ren, this->x+header_width, 0, this->x+header_width, h);
}

ProjectView::~ProjectView() {
}