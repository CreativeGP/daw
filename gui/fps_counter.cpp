#include "fps_counter.h"

#include <iostream>

FpsCounter::FpsCounter(int x, int y, TTF_Font *font) {
    this->x = x;
    this->y = y;
    this->sans_font = font;
}

Message FpsCounter::update\(GlobalCargo *globals) {
}

void FpsCounter::update_start() {
    start_count = SDL_GetPerformanceCounter();
}

void FpsCounter::update_end() {
    end_count = SDL_GetPerformanceCounter();
    elapsed_sec = (end_count - start_count) / (double)SDL_GetPerformanceFrequency();
    elapsed_ms = elapsed_sec * (double) 1000.0;
    fps = (double) 1.0 / elapsed_sec; if (fps > 60) fps = 60.0;

    double wait_ms = floor((double)16.667 - elapsed_ms);
    if (wait_ms > 0) {
        SDL_Delay(wait_ms);
    }
}

void FpsCounter::draw(SDL_Renderer *ren) {
    const SDL_Color COLOR_WHITE = {225,225,225};
    const SDL_Color COLOR_BLACK = {0,0,0};

    fps_surface = TTF_RenderText_Solid(sans_font, std::to_string(fps).c_str(), COLOR_WHITE);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, fps_surface);
    SDL_Rect r = SDL_Rect{this->x, this->y, fps_surface->w, fps_surface->h};
    SDL_RenderCopy(ren, tex, 0, &r);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(fps_surface);

}

FpsCounter::~FpsCounter() {
    // SDL_FreeSurface(fps_surface);
}