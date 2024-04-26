#ifndef PIANO_ROLL_H
#define PIANO_ROLL_H

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "component.h"
#include "util.h"
#include "collision_detector.h"



class PianoRoll : public Component {
public:
    PianoRoll(int, int, int, int, TTF_Font *, SDL_Color col);
    ~PianoRoll();

    Message update(GlobalCargo *globals);
    void draw(SDL_Renderer *);

    void zoomx(float scale);

    void add_note(float key, int pitch, float length);

    void start_playing(GlobalCargo *globals);
    void stop_playing();

    bool has_error() { return this->error; }
public:
    // int x, y, w, h;
    // int innerx, innery;

    // piano roll settings (public)
    SDL_Color note_color;


private:

    class Note {
    public:
        Note();
        ~Note();
        int update(PianoRoll &super, GlobalCargo *globals);
        int draw(PianoRoll &super, SDL_Renderer *, SDL_Color);
    public:
        float key;
        int pitch;
        float length;
        bool selected = false, resizing = false, dragged = false;
        CollisionDetector coll;
    };

    int inner_mousex, inner_mousey;

    std::multimap<float, Note> notes, selected_notes;
    std::multimap<float, Note>::iterator notes_itr;
    SDL_Rect selection;
    Note dragged_note; float drag_delta;
    Note resizing_note; float resizing_delta; int resizing_direction;
    float length_memo = 3.0f;
    CollisionDetector coll_piano[128];
    

    float playing_buffer[128];

    GlobalCargo *globals;

    // piano roll settings
    int rhythmn=4, rhythmd=4;
    int gh=10, gw=100;
    float length_unit = gw/(float)rhythmn;
    const int notename_margin = 50;
    const int piano_width = 24;
    const int note_edge = 20;


    void draw_label(SDL_Renderer *, std::string str, SDL_Rect r, SDL_Color col);
    TTF_Font *font;

    bool error = false;
};

#endif
