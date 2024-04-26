#include "piano_roll.h"

#include <iostream>
#include <map>

PianoRoll::Note::Note() : key(0), pitch(0), length(0)  {
}
PianoRoll::Note::~Note() {}

int PianoRoll::Note::draw(PianoRoll &super, SDL_Renderer *ren, SDL_Color col) {
    if (this->key == 0) return 0;
    
    float note_x = (this->key - super.innerx/super.length_unit)*super.length_unit;
    float note_y = super.gh*(128-this->pitch)-super.innery;   // 128音と決め打ち
    float note_w = super.length_unit * (float)this->length;
    float note_h = super. gh;
    
    // multisetは順番に並んでいるはずなので...
    // 早期リターン
    if (note_x + note_w < 0)
        return 0;
    if (super.w < note_x)
        return -1;

    SDL_Rect r {super.x+note_x, super.y+note_y, note_w, note_h};
    
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
    SDL_RenderFillRect(ren, &r);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &r);
    r = SDL_Rect{super.x + note_x+1, super.y + note_y+1, note_w-1, note_h-1};
    SDL_RenderDrawRect(ren, &r);

    return 0; // ここにreturnをかかないとud2が生成された...
}


/*
戻り値 
0 -- 続行
1 -- 要素削除, 続行
-1 -- 終了
*/
int PianoRoll::Note::update(PianoRoll &super, GlobalCargo *globals) {
    float note_x = key*super.length_unit - super.innerx;
    float note_y = super.gh*(128-pitch)-super.innery;   // 128音と決め打ち
    float note_w = super.length_unit * (float)length;
    float note_h = super.gh;
    
    // std::cout << note_x << " " << super.inner_mousex << " " << note_x + note_w << std::endl;

    if (0 < super.inner_mousex && super.inner_mousex < super.w && 0 < super.inner_mousey && super.inner_mousey < super.h) {
        if (dragged) {
            key = (super.innerx + super.inner_mousex-super.drag_delta)/super.length_unit;
            pitch = 128 - (super.innery+super.inner_mousey)/super.gh;
        }

        if (resizing) {
            if (super.resizing_direction == 1) {
                length = (super.innerx + super.inner_mousex)/super.length_unit - key;
                super.length_memo = length;
            }
            if (super.resizing_direction == -1) {
                key = (super.innerx + super.inner_mousex)/super.length_unit;
                // length = 
            }
        }
    }

    if (globals->release[SDL_BUTTON_LEFT]) {
        if (dragged) {
            // ここの順番大事. 自分の状態更新 -> superの状態更新
            dragged = false;
            super.notes.emplace(std::make_pair(key, *this));
            super.dragged_note = Note{};
        }

        if (resizing) {
            resizing = false;
            super.notes.emplace(std::make_pair(key, *this));
            super.resizing_note = Note{};
        }
    }


    // 早期リターン
    if (note_x + note_w < 0)
        return 0;
    if (super.w < note_x)
        return -1;

    if (globals->click[SDL_BUTTON_LEFT]) {
    if (note_x < super.inner_mousex && super.inner_mousex < note_x+note_w &&
        note_y < super.inner_mousey && super.inner_mousey < note_y+note_h)
    {
        if (length*super.length_unit > super.note_edge*2) {
            // 左に引き伸ばす
            if (note_x < super.inner_mousex && super.inner_mousex < note_x+super.note_edge &&
                note_y < super.inner_mousey && super.inner_mousey < note_y+note_h) {
                    super.resizing_note = *this;
                    super.resizing_direction = -1;
                    super.resizing_delta = super.inner_mousex-note_x;
                    std::cout << "r" << super.resizing_delta << std::endl;
                    return 1;

            }

            // 右に引き伸ばす
            if (note_x+note_w-super.note_edge < super.inner_mousex && super.inner_mousex < note_x+note_w &&
                note_y < super.inner_mousey && super.inner_mousey < note_y+note_h) {
                resizing = true;
                super.resizing_note = *this;
                super.resizing_direction = 1;
                super.resizing_delta = super.inner_mousex-note_x;
                    std::cout << "r" << super.resizing_delta << std::endl;
                return 1;
            }
        }
//                selected_notes.emplace(*it);
        dragged = true;
        super.dragged_note = *this; 
        super.drag_delta = super.inner_mousex-note_x;
        return 1;
    }
    }

    return 0;
}


PianoRoll::PianoRoll(int x, int y, int w, int h, TTF_Font *font, SDL_Color note_col) :
    dragged_note(Note{}), font(font), note_color(note_col)

{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    
    innerx = 0;
    innery = 0;

    for (int i = 0; i < 128; i++) {
        playing_buffer[i] = 0;

        coll_piano[i].origin[0] = std::make_pair(ORIGIN_PARENT_BOX, this->parent);
        coll_piano[i].origin[1] = std::make_pair(ORIGIN_SCROLLABLE_Y_ONLY, this);
        coll_piano[i].fixed = true;
        coll_piano[i].f_on_click = [i](GlobalCargo *globals){
            // globals->inst1->note_on((int)i, 1.0f);
            std::cout << "playing " << i << std::endl;
        };
        coll_piano[i].x = 0;
        coll_piano[i].y = i*gh;
        coll_piano[i].w = piano_width;
        coll_piano[i].h = gh;
    }

    // notes.emplace(std::make_pair(3.0f, Note{3.0f, 110, 3}));
}

void PianoRoll::zoomx(float scale)
 {
    gw *= scale;
    length_unit = gw/(float)rhythmn;
    // std::cout << gw << " " << length_unit << std::endl;
}

void PianoRoll::add_note(float key, int pitch, float length) {
    Note note;
    note.key = key;
    note.pitch = pitch;
    note.length = length;
    notes.emplace(std::make_pair(key, note));
}

Message PianoRoll::update(GlobalCargo *globals) {
    this->globals = globals;

    inner_mousex = globals->mousex - x;
    inner_mousey= globals->mousey - y;

    selected_notes.clear();


    // ピアノロール内のノートの更新
    dragged_note.update(*this, globals);
    resizing_note.update(*this, globals);
    std::multimap<float, Note>::iterator it = notes.begin();
    while (it != notes.end()) {
        Note& n = it->second;

        switch(n.update(*this, globals)) {
            case 1: notes.erase(it++); std::cout << "a" << std::endl; break;
            case -1: goto update_end; break;
            default: ++it; break;
        }
    }
update_end:


    // ノートの生成
    if (globals->click[SDL_BUTTON_RIGHT]) {
        float key = (innerx + inner_mousex)/length_unit;
        int pitch = 128 - floor((innery+inner_mousey)/gh);
        add_note(key, pitch, length_memo);
    }

    // シークバーの移動
    if (globals->dblclick[SDL_BUTTON_LEFT]) {
        float key = (innerx + inner_mousex)/length_unit;
        globals->seek_key = key;
    }

    // 位置移動
    if (globals->key_pressing[SDL_SCANCODE_RIGHT]) innerx += 5;
    if (globals->key_pressing[SDL_SCANCODE_LEFT]) innerx -= 5;
    if (globals->key_pressing[SDL_SCANCODE_DOWN]) innery += 5;
    if (globals->key_pressing[SDL_SCANCODE_UP]) innery -= 5;

    // ズーム
    if (0 < inner_mousex && inner_mousex < piano_width) {
        if (globals->mod & KMOD_CTRL) zoomx(pow(1.2f, -globals->wheely));
        else gh *= pow(1.2f, -globals->wheely);
    } else {
        innerx += globals->wheelx * 10.0f;
        innery += globals->wheely * 5.0f;
    }
    if (innery <0) innery = 0;

    if (globals->key[SDL_SCANCODE_Q]) {
        zoomx(1.5f);
    }
    if (globals->key[SDL_SCANCODE_E]) {
        zoomx(1/1.5f);
    }
    if (globals->key[SDL_SCANCODE_W]) {
        gh *= 1.5f;
    }
    if (globals->key[SDL_SCANCODE_S]) {
        gh /= 1.5f;
    }

    for (int i = 0; i < 128; i++) {
        this->coll_piano[i].update(globals);
    }

    
    if (globals->Playing) {
        for (int i = 0; i < 128; i++)
            if (this->playing_buffer[i] > 0) {
                this->playing_buffer[i] -= globals->seek_step;
                if (this->playing_buffer[i] < 0) {
                    // std::cout << this->playing_buffer[i] << globals->seek_step << std::endl;
                    this->playing_buffer[i] = 0;
                    globals->inst1->note_off(i);
                }
            }

        // auto scrolling
        innerx = globals->seek_key*length_unit - 200;

        float sec_per_beat = 60.0f / (float)globals->BPM;
        while (globals->seek_key-globals->seek_step < notes_itr->first &&
               notes_itr->first < globals->seek_key) {
            this->playing_buffer[notes_itr->second.pitch] = notes_itr->second.length /*keys*/;
            
//            globals->inst2->note_on(notes_itr->second.pitch, notes_itr->second.length * sec_per_beat);
            globals->inst1->note_on(notes_itr->second.pitch);
            notes_itr++;
        }
    }

    return MSG_OK;
}

// 現在のシークバーの位置から再生を始める
void PianoRoll::
start_playing(GlobalCargo *globals) {
    notes_itr = notes.lower_bound(globals->seek_key);
}

void PianoRoll::
stop_playing() {
    for (int i = 0; i < 128; i++) {
        globals->inst1->note_off(i);
    }
}


void PianoRoll::
draw_label(SDL_Renderer *ren, std::string str, SDL_Rect r, SDL_Color col) {
    SDL_Surface *tmp_surface = TTF_RenderText_Solid(font, str.c_str(), col);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, tmp_surface);
    if (r.w == -1) r.w = ((float)r.h/tmp_surface->h) * tmp_surface->w;
    if (r.h == -1) r.h = ((float)r.w/tmp_surface->w) * tmp_surface->h;
    SDL_RenderCopy(ren, tex, 0, &r);
    
    // これ本当に毎回しないとだめかな...?
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(tmp_surface);
}

void PianoRoll::draw(SDL_Renderer *ren) {    
    // bg
    SDL_Rect r = SDL_Rect{x, y, w, h};
    SDL_SetRenderDrawColor(ren, 80, 80, 80, 255);
    SDL_RenderFillRect(ren, &r);

    // piano
    SDL_RenderSetClipRect(ren, &r);
    for (int basey = -(innery%(12*gh)); basey < h; basey += 12*gh) {
        for (int i = 0; i < 12 ; i++) {
            // i= 0 1  2 3  4 5  6 7 8  9 10 11
            //    b a# a g# g f# f e d# d c# c
            r = SDL_Rect{this->x, this->y+basey+gh*i, w, gh};
            if (i == 1 || i == 3 || i == 5 || i == 8 || i == 10) {
//                SDL_SetRenderDrawColor(ren, , 255);
//                SDL_RenderFillRect(ren, &r);
            } else {
                SDL_SetRenderDrawColor(ren, 90, 90, 90, 255);
                SDL_RenderFillRect(ren, &r);
            }
        }
    }


    // h2 grid gw/rhythmn x gh
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 100);
    for (float x = - std::remainder(innerx, gw/(float)rhythmn); x <= w; x += (gw/(float)rhythmn)) {
        SDL_RenderDrawLine(ren, this->x + (int)x, this->y + 0,
                            this->x + (int)x, this->y + h); // integer mode
    }
    for (float y =- innery%(gh); y <= h; y += gh) {
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y,
                            this->x + w, this->y + (int)y); // integer mode
    }

    // h1 grid gw x 12gh
    SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
    for (float x = - innerx%gw; x <= w; x += gw) {
        if (x < 0) continue;
        SDL_RenderDrawLine(ren, this->x + (int)x, this->y + 0,
                                this->x + (int)x, this->y + h); // integer mode
        SDL_RenderDrawLine(ren, this->x + (int)x+1, this->y + 0,
                                this->x + (int)x+1, this->y + h); // integer mode
        SDL_RenderDrawLine(ren, this->x + (int)x-1, this->y + 0,
                                this->x + (int)x-1, this->y + h); // integer mode
    }
    for (float y =- innery%(12*gh); y <= h; y += 12*gh) {
        if (y < 0) continue;
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y,
                                this->x + w, this->y + (int)y); // integer mode
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y+1,
                                this->x + w, this->y + (int)y+1); // integer mode
        SDL_RenderDrawLine(ren, this->x + 0, this->y + (int)y-1,
                                this->x + w, this->y + (int)y-1); // integer mode

    }

    


    SDL_Rect r2 {this->x, this->y, this->w, this->h};
    SDL_RenderSetClipRect(ren, &r2);
    std::multimap<float, Note>::iterator it = notes.begin();
    while (it != notes.end()) {
        switch(it->second.draw(*this, ren, note_color)) {
            // case 1: notes.erase(it++);  break;
            case -1: goto draw_end; break;
            default: ++it; break;
        }
    }  
draw_end:
    this->dragged_note.draw(*this, ren, SDL_Color{0,0,255,255});
    this->resizing_note.draw(*this, ren, SDL_Color{0,0,255,255});
    SDL_RenderSetClipRect(ren, 0);


    // piano
    r = SDL_Rect{this->x, this->y, piano_width, this->h};
    SDL_SetRenderDrawColor(ren, 255,255,255,255);
    SDL_RenderFillRect(ren, &r);
    SDL_RenderSetClipRect(ren, &r2);
    for (int basey = -(innery%(12*gh)); basey < h; basey += 12*gh) {
    for (int i = 0; i < 12 ; i++) {
        // i= 0 1  2 3  4 5  6 7 8  9 10 11
        //    b a# a g# g f# f e d# d c# c
        r = SDL_Rect{this->x, this->y+basey+gh*i, piano_width, gh};
        if (i == 1 || i == 3 || i == 5 || i == 8 || i == 10) {
            SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
        } else {
            SDL_SetRenderDrawColor(ren, 220, 220, 220, 255);
        }
        SDL_RenderFillRect(ren, &r);
    }
    }
    for (int i = 0; i < 128; i++) {
        coll_piano[i].draw(ren);
    }
    SDL_RenderSetClipRect(ren, 0);





    // note names
    for (float y =- innery%(12*gh); y <= h; y += 12*gh) {
        if (y < 0) continue;
        draw_label(ren, "C"+std::to_string(8 - (int)((innery+y)/(12*gh))),
            SDL_Rect{this->x+this->w-notename_margin, this->y + y-gh, -1, gh},
            SDL_Color{200,200,200,255});
    }
    for (float x =- innerx%gw; x <= w; x += gw) {
        if (x < 0) continue;
        draw_label(ren, std::to_string((int)((innerx+x)/gw)),
            SDL_Rect{this->x+x, this->y, -1, gh},
            SDL_Color{200,200,200,255});
    }


    // seek bar
    float beatw = (float)gw / rhythmn;
    float inner_key = globals->seek_key-(float)innerx/beatw;
    SDL_SetRenderDrawColor(ren, 255,255,0,255);
    SDL_RenderDrawLine(ren, inner_key*beatw + this->x, this->y, inner_key*beatw + this->x, this->y+this->h); 
}

PianoRoll::~PianoRoll() {
}




// Note::Note() {}
// Note::~Note() {}

// Message Note::update(GlobalCargo *globals);
// void Note::draw(SDL_Renderer *ren) {
    
// }
