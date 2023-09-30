#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "component.h"

class FpsCounter : public Component {
public:
    FpsCounter(int x, int y, TTF_Font *);
    ~FpsCounter();

    Message update(GlobalCargo *globals);
    void update_start();
    void update_end();
    void draw(SDL_Renderer *renderer);

    bool has_error() { return this->error; }
public:
    // int x;
    // int y;

private:
    TTF_Font *sans_font;
    SDL_Surface *fps_surface;

    Uint64 start_count, end_count;
    double elapsed_sec = 0;
    double elapsed_ms = 0;
    double fps = 0;


    bool error = false;
};

#endif