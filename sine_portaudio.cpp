#include <cmath>
#include <portaudio.h>
#include <iostream>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (64)
#define TWO_PI (3.14159265f * 2.0f)

typedef struct {
    float phase;
} paTestData;

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;

    for (i = 0; i < framesPerBuffer; i++) {
        float sample = std::sin(data->phase * TWO_PI);
        *out++ = sample;
        data->phase += 0.01f;
        if (data->phase > 1.0f) {
            data->phase -= 1.0f;
        }
    }

    return paContinue;
}

int main(void)
{
    PaStream *stream;
    PaError err;
    paTestData data;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << "Error: Failed to initialize PortAudio" << std::endl;
        return 1;
    }

    data.phase = 0.0f;

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, paCallback, &data);
    if (err != paNoError) {
        std::cout << "Error: Failed to open stream" << std::endl;
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to start stream" << std::endl;
        return 1;
    }

    std::cout << "Press ENTER to stop stream." << std::endl;
    std::cin.ignore();

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to stop stream" << std::endl;
        return 1;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to close stream" << std::endl;
        return 1;
    }

    Pa_Terminate();

    return 0;
}