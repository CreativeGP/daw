#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <string>
#include <map>
#include <SDL2/SDL.h>

#include "component.h"
#include "util.h"

class MainWindow {
public:
    MainWindow(std::string title, int w, int h);
    ~MainWindow();

    Message update(GlobalCargo *);
    void draw();

    void add_component(std::string name, Component *com);


    bool has_error() { return this->error; }
public:
    std::map<std::string, Component *> components;
    Component *active_window = 0; // Windowクラスに内部値としてselectedを導入する方法もあるが、ここのdrawクラスの挙動を
private:
    SDL_Window *window = 0;
    //SDL_Surface *surface = 0;
    SDL_Renderer *renderer = 0;

    bool error = false;
};

#endif