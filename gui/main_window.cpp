#include "main_window.h"

#include <iostream>

MainWindow::
    MainWindow(std::string title, int w, int h)
{
    if (!(this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)))
    {
        std::cerr << SDL_GetError() << std::endl;
        error = true;
    }

    // this->surface = SDL_GetWindowSurface(window);
    // if (!this->surface) {
    //     std::cerr << SDL_GetError() << std::endl;
    //     error = true;
    // }

    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << SDL_GetError() << std::endl;
        error = true;
    }
}

// activeになれるcomponentは一つだけなので、ほかはdeactiveする
void MainWindow::change_active(Component *active_com)
{
    for (auto it = this->components.begin();
         it != this->components.end();
         it++)
    {
        if (active_com == it->second)
            it->second->active = true; // make sure
        else
            it->second->active = false;
    }
}

Message MainWindow::
    update(GlobalCargo *globals)
{
    for (auto it = this->components.begin();
         it != this->components.end();
         it++)
    {
        if (it->second->update(globals) == MSG_ACTIVATED)
            change_active(it->second);
    }

    return MSG_OK;
}

/*
レンダリングにはSDL_SurfaceよりSDL_Rendererを使ったほうが良いらしい.
そうすると、openglとあまり変わらないからskiaを使ってもいいかも。SDL_gfxをとりあえずつかう。
https://api.skia.org/classSkFont.html#details
https://chromium.googlesource.com/skia/+/chrome/m53/example/SkiaSDLExample.cpp

https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
*/
void MainWindow::
    draw()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // SDL_FillRect(this->surface, 0, SDL_MapRGB(this->surface->format, 0, 0, 0));

    Component *active_com = 0;
    for (auto it = this->components.begin();
         it != this->components.end();
         it++)
    {
        if (it->second->active)
        {
            active_com = it->second;
            continue;
        }
        it->second->draw(this->renderer);
    }
    if (active_com)
        active_com->draw(this->renderer);

    // SDL_UpdateWindowSurface(window);
    SDL_RenderPresent(renderer);
}

void MainWindow::add_component(std::string name, Component *com)
{
    com->name = name;
    this->components[name] = com;
}

MainWindow::
    ~MainWindow()
{
    SDL_DestroyRenderer(renderer);
    renderer = 0;
    SDL_DestroyWindow(window);
    window = 0;
}
