#include <SDL2/SDL.h>
#include <cmath>

#define TSF_IMPLEMENTATION
#include "tsf.h"

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;
const int CHANNELS = 1;
const double PI = 3.14159265358979323846;

void audio_callback(void* userdata, Uint8* stream, int len) {
    static double phase = 0;
    double increment = 2 * PI * FREQUENCY / FREQUENCY;
    for (int i = 0; i < len; i++) {
        double value = AMPLITUDE * sin(phase);
        stream[i] = (Uint8)(value);
        phase += increment;
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;
    spec.freq = FREQUENCY;
    spec.format = AUDIO_U8;
    spec.channels = CHANNELS;
    spec.samples = 2048;
    spec.callback = audio_callback;
    SDL_OpenAudio(&spec, NULL);

    tsf* TinySoundFont = tsf_load_filename("soundfont.sf2");
    tsf_set_output(TinySoundFont, TSF_MONO, 44100, 0); //sample rate
    tsf_note_on(TinySoundFont, 0, 60, 1.0f); //preset 0, middle C
    short HalfSecond[22050]; //synthesize 0.5 seconds
tsf_render_short(TinySoundFont, HalfSecond, 22050, 0);


    SDL_PauseAudio(0);
    SDL_Delay(5000);
    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}