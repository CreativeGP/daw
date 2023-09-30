#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "main_window.h"
#include "fps_counter.h"
#include "piano_roll.h"
#include "window.h"
#include "project_view.h"
#include "util.h"

const int AMPLITUDE = 28000/8;
const int SAMPLE_RATE = 44100;



class Scroll { // スクロールバー
    int offset_x, offset_y;
    int width, height;
    int bar_width, bar_height;
    
};


class PianoData {
private:
    int data[128];
    
public:
    PianoData();
    ~PianoData();
    int *get_data() { return this->data; }
    void all_off() {
        memset(this->data,0,128*sizeof(int));
    }
};

inline Sint16 sinusoidal_wave(float freq, double time) {
    return (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time));
}

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    // master_thbuf.read(bytes);
    // Sint16 *buffer = (Sint16*)raw_buffer;
    // int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    // int &sample_nr(*(int*)user_data);

    // for(int i = 0; i < length; i++, sample_nr++)
    // {
    //     double time = (double)sample_nr / (double)SAMPLE_RATE;
    //     buffer[i] = 0;
    //     // buffer[i] += sinusoidal_wave(441.f, time); // render 441 HZ sine wave
    // }
}

void load_globals(GlobalCargo *globals) {
    globals->fonts["sans"] = TTF_OpenFont("sans.ttf", 24);
    if (!globals->fonts["sans"]) {
        std::cerr << TTF_GetError() << std::endl;
    }


}

void free_globals(GlobalCargo *globals) {
    // font free.. ?
}


int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    if(TTF_Init() != 0) {
        std::cerr << TTF_GetError() << std::endl;
    }

    int sample_nr = 0;

    SDL_AudioSpec want;
    want.freq = SAMPLE_RATE; // number of samples per second
    want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
    want.channels = 1; // only one channel
    want.samples = 2048; // buffer-size
    want.callback = audio_callback; // function SDL calls periodically to refill the buffer
    want.userdata = &sample_nr; // counter, keeping track of current sample number

    SDL_AudioSpec have;
    if(SDL_OpenAudio(&want, &have) != 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
    if(want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");

    GlobalCargo globals;
    load_globals(&globals);

    MainWindow main_window{"oto", 800, 600};
    FpsCounter fps_counter{1500, 0, globals.fonts["sans"]}; main_window.add_component("fps", &fps_counter);
    ProjectView projview{}; main_window.components["projview"] = &projview;
    Window window1{100, 100, 500, 500, globals.fonts["sans"], "Melody #1"};
    Window window2{200, 100, 500, 500, globals.fonts["sans"], "Melody #2"}; 
    Window window3{300, 100, 500, 500, globals.fonts["sans"], "Melody #3"};
    main_window.add_component("wnd1", &window1);
    main_window.add_component("wnd2", &window2);
    main_window.add_component("wnd3", &window3);
    PianoRoll melody1{100, 120, 500, 500, globals.fonts["sans"], SDL_Color{255,0,0,255}}; window1.inner_component = &melody1;
    PianoRoll melody2{200, 120, 500, 500, globals.fonts["sans"], SDL_Color{255,255,0,255}}; window2.components["pno"] = &melody2;
    PianoRoll melody3{300, 120, 500, 500, globals.fonts["sans"], SDL_Color{0,255,0,255}}; window3.components["pno"] = &melody3;

    if (main_window.has_error() 
        || fps_counter.has_error()) {
        return -1;
    }

    for (int i = 0; i <256; i++) {
        globals.key_pressing[i] = false;
    }



    bool Running = true;
    SDL_Event e;
    while (Running) {
        fps_counter.update_start();

        // TODO: intにしてもいい
        globals.click[0]=false; globals.click[1]=false; globals.click[2]=false; globals.click[3]=false; globals.click[4]=false; 
        globals.release[0]=false; globals.release[1]=false; globals.release[2]=false; globals.release[3]=false; globals.release[4]=false; 
        globals.dblclick[0]=false; globals.dblclick[1]=false; globals.dblclick[2]=false; globals.dblclick[3]=false; globals.dblclick[4]=false;
        globals.mouseaction = false;

        for (int i = 0; i <256; i++) globals.key[i] = false;

        globals.wheelx = 0; globals.wheely = 0;

        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_KEYUP: {
                    if (e.key.repeat != 0) continue;
                    globals.mod = e.key.keysym.mod;
                    if (e.key.keysym.scancode < 256) globals.key[e.key.keysym.scancode] = false;
                    if (e.key.keysym.scancode < 256) globals.key_pressing[e.key.keysym.scancode] = false;
                } break;
                case SDL_KEYDOWN: {
                    if (e.key.repeat != 0) continue;
                    globals.mod = e.key.keysym.mod;
                    if (e.key.keysym.scancode < 256) globals.key[e.key.keysym.scancode] = true;
                    if (e.key.keysym.scancode < 256) globals.key_pressing[e.key.keysym.scancode] = true;
                } break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    globals.pressing[e.button.button] = (e.button.state == SDL_PRESSED);

                    if (e.button.state == SDL_PRESSED) {
                        if (e.button.clicks == 1) {
                            globals.click[e.button.button] = true; globals.mouseaction = true;
                        } else if (e.button.clicks == 2) {
                            globals.dblclick[e.button.button] = true; globals.mouseaction = true;
                        }
                    } else if (e.button.state == SDL_RELEASED) {
                        globals.release[e.button.button] = true;
                    }
                } break;

                case SDL_MOUSEMOTION: {
                    globals.mousex = e.motion.x;
                    globals.mousey = e.motion.y;
                    globals.mouse_xrel = e.motion.xrel;
                    globals.mouse_yrel = e.motion.xrel;
                } break;

                case SDL_MOUSEWHEEL: {
                    globals.wheelx = e.wheel.preciseX;
                    globals.wheely = -e.wheel.preciseY;
                    globals.mouseaction = true;
                } break;
            }
        }

        main_window.update(&globals);
        main_window.draw();
        fps_counter.update_end();
    }

    
    // SDL_PauseAudio(0); // start playing sound
    // SDL_Delay(1000 * 100); // wait while sound is playing
    // SDL_PauseAudio(1); // stop playing sound

    free_globals(&globals);

    SDL_CloseAudio();

    SDL_Quit();

    return 0;
}
