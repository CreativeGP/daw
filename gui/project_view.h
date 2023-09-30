#ifndef PROJECT_VIEW_H
#define PROJECT_VIEW_H

#include <string>
#include <SDL2/SDL.h>

#include "component.h"

class ProjectView : public Component {
public:
    ProjectView();
    ~ProjectView();

    Message update(GlobalCargo *globals);
    void draw(SDL_Renderer *ren);

    bool has_error() { return this->error; }
public:
    int /*x, y, */w, h;

private:
    // settings
    int gh = 50;
    int header_width = 100;

    int innerx, innery;
    
    bool error = false;
};

#endif