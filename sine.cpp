#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

#include <chrono>
#include <ctime>

using namespace std;
using std::chrono::system_clock;

const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 1767;

system_clock::time_point p, before;


void audio_callback(void* userdata, Uint8* stream, int len)
{
    before = p;
    p = system_clock::now();

    double time_step = 1.0 / SAMPLE_RATE;
    double phase_step = 2.0 * M_PI * 440 * time_step;
    double phase = 0.0;
    Sint16* buffer = reinterpret_cast<Sint16*>(stream);
    int sample_count = len / sizeof(Sint16);

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(p-before).count() << "ms "<< sample_count << std::endl; 

    for (int i = 0; i < sample_count; i++)
    {
        double sample = AMPLITUDE * std::sin(phase);
        buffer[i] = static_cast<Sint16>(sample);
        phase += phase_step;
    }
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_AudioSpec desired_spec;
    desired_spec.freq = SAMPLE_RATE;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = 1;
    desired_spec.samples = 4096;
    desired_spec.callback = audio_callback;
    desired_spec.userdata = nullptr;

    SDL_AudioSpec obtained_spec;
    if (SDL_OpenAudio(&desired_spec, &obtained_spec) < 0)
    {
        std::cerr << "SDL_OpenAudio failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudio(0);

    // Wait for user to press a key to exit
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}