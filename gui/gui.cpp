#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <mutex>

#include <libremidi/libremidi.hpp>
#include <libremidi/reader.hpp>

#include "main_window.h"
#include "fps_counter.h"
#include "piano_roll.h"
#include "window.h"
#include "project_view.h"
#include "generator.h"
#include "util.h"

#include "tsf.h"

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
    auto start = std::chrono::high_resolution_clock::now();

    // std::cout << "AUDIO CALLBACK " << bytes << std::endl;

    GlobalCargo *globals = (GlobalCargo *) user_data;
    Sint16 *samples = (Sint16 *) raw_buffer;
    // float nara byte/4

    // buffer clear
    for (int i = 0; i < bytes/2; i++) {
        samples[i] = 0;
    }

    globals->inst1->write_buffer(globals, samples, bytes/2);
    // globals->inst2->write_buffer(globals, (Sint16 *)samples, bytes/2);
    // tsf_render_float(globals->inst2->tinysoundfont, samples, bytes/4, 0);


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // std::cout << "AUDIO CALLBACK END (" << duration.count()*1000.0f << "ms)"<< std::endl;
}


void load_globals(GlobalCargo *globals) {
    globals->fonts["sans"] = TTF_OpenFont("sans.ttf", 24);
    if (!globals->fonts["sans"]) {
        std::cerr << TTF_GetError() << std::endl;
    }

    globals->Desired.freq= 44100; /* Sampling rate: 22050Hz */
    globals->Desired.format= AUDIO_S16; /* 16-bit signed audio */
    globals->Desired.channels= 0; /* Mono */
    globals->Desired.samples= 128; /* Buffer size: 8K = 0.37 sec. */
    globals->Desired.callback= audio_callback;
    globals->Desired.userdata= globals;

    globals->inst1 = new SampleInstrument();
    // globals->inst2 = new SF2Instrument("/home/cgp/Programs/SGM-V2.01.sf2", 3, globals);

    SDL_OpenAudio(&globals->Desired, &globals->Obtained);
    std::cout << "AUDIO SETTING: CALLBACK LIMIT = " << 1000 * (float)globals->Desired.samples/globals->Desired.freq << " ms" << std::endl;
}

void free_globals(GlobalCargo *globals) {
    // font free.. ?
    delete globals->inst1;
    // delete globals->inst2;
}


int main(int argc, char *argv[])
{
    std::mutex mtx;
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    if(TTF_Init() != 0) {
        std::cerr << TTF_GetError() << std::endl;
    }


  
    GlobalCargo globals;
    load_globals(&globals);

    // https://stackoverflow.com/questions/8744608/dlopen-in-multithreaded-application-exit-with-trace-bpt-trap
    // なんかよくわからんけど、dl_openはスレッド安全じゃなくて、オーディオスレッドが走ってるときに実行されるとメモリを破壊する？
    // SDL_CreateRendererとかは内部でdl_openする可能性がある
    // そういうエラーがたくさん起こったので、オーディオスレッドの開始は、そういうのが全部終わったあとで(うーん)
    MainWindow main_window{"oto [DAWDAW]", 800, 600};
    FpsCounter fps_counter{1500, 0, globals.fonts["sans"]}; main_window.add_component("fps", &fps_counter);
    ProjectView projview{}; main_window.components["projview"] = &projview;
    PianoRoll melody1{100, 120, 1000, 800, globals.fonts["sans"], SDL_Color{255,255,0,255}}; 
    PianoRoll melody2{200, 120, 500, 500, globals.fonts["sans"], SDL_Color{255,255,0,255}}; 
    PianoRoll melody3{300, 120, 500, 500, globals.fonts["sans"], SDL_Color{0,255,0,255}};
    
    // ここ右辺値でできる方法があれば最高なのにな
    Window window1{100, 100, 1000, 800, globals.fonts["sans"], "Melody #1", 
                    &melody1};
    Window window2{200, 100, 500, 500, globals.fonts["sans"], "Melody #2", &melody2}; 
    Window window3{300, 100, 500, 500, globals.fonts["sans"], "Melody #3", &melody3};
    main_window.add_component("wnd1", &window1);
    // main_window.add_component("wnd2", &window2);
    // main_window.add_component("wnd3", &window3);

    // Read raw from a MIDI file
    std::ifstream file{"/home/cgp/Music/mine/yamyu2.mid", std::ios::binary};

    std::vector<uint8_t> bytes;
    bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    // Initialize our reader object
    libremidi::reader r;

    // Parse
    libremidi::reader::parse_result result = r.parse(bytes);
    

    // If parsing succeeded, use the parsed data
    if (result != libremidi::reader::invalid) {
        for (int i = 2; i < 10; i++) {
            int notes[128];
            unsigned long long bar = 0;
            for (auto& event : r.tracks[i]) {
                libremidi::message_type type = event.m.get_message_type();
                bar += event.tick;

                if (type == libremidi::message_type::NOTE_ON) {
                    const int note_number = (int)event.m.bytes[1];
                    notes[note_number] = bar;
                }

                if (type == libremidi::message_type::NOTE_OFF) {
                    const int note_number = event.m.bytes[1];
                    float key = notes[note_number] / 480.0f;
                    float length = (bar - notes[note_number]) / 480.0f;
                    // melody1.add_note(key, note_number, length);
                    // std::cout << key << " " << 128-note_number << " " << length << std::endl;
                }
                // std::cout << "message type: " << (int)type << ", tick" << event.tick << std::endl;
            }
        }
    }

    // melody2.start_playing();
    // melody3.start_playing();


    if (main_window.has_error() 
        || fps_counter.has_error()) {
        return -1;
    }

    for (int i = 0; i <256; i++) {
        globals.key_pressing[i] = false;
    }

    SDL_PauseAudio(0); // 最後に行うこと! 

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
        Sint32 tempx = globals.mousex, tempy = globals.mousey;

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

                    if (e.key.keysym.sym == SDLK_SPACE) { // toggle playing
                        globals.Playing = !globals.Playing;
                        if (globals.Playing)
                            melody1.start_playing(&globals);
                        else
                            melody1.stop_playing();
                    }
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
                } break;

                case SDL_MOUSEWHEEL: {
                    globals.wheelx = e.wheel.preciseX;
                    globals.wheely = -e.wheel.preciseY;
                    globals.mouseaction = true;
                } break;
            }
        }
        // e.motion.xrelとかもあるけど、単位がよくわからないので使わない
        globals.mouse_xrel = globals.mousex - tempx;
        globals.mouse_yrel = globals.mousey - tempy;

        main_window.update(&globals);
        main_window.draw();

        if (globals.Playing) globals.seek_key += globals.seek_step;
        // std::cout << globals.seek_key << std::endl;
        fps_counter.update_end();
    }

    
    // SDL_PauseAudio(0); // start playing sound
    // SDL_Delay(1000 * 100); // wait while sound is playing
    // SDL_PauseAudio(1); // stop playing sound

    // SDL_CloseAudio();
    // free_globals(&globals);


    SDL_Quit();

    return 0;
}
