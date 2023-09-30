#include <math.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include <iostream>
#include <chrono>
#include <ctime>


#define SAMPLE_RATE (44100)
#define PI (3.14159265)
#define 2PI (PI * 2.0)

using std::chrono::system_clock;
system_clock::time_point start;

int main() {
    start = system_clock::now();

    // Create a PulseAudio simple connection
    pa_simple *connection = NULL;
    pa_sample_spec sample_spec;
    sample_spec.format = PA_SAMPLE_FLOAT32LE;
    sample_spec.rate = SAMPLE_RATE;
    sample_spec.channels = 1;
    int error;      
    connection = pa_simple_new(NULL, "Sine wave", PA_STREAM_PLAYBACK, NULL, "playback", &sample_spec, NULL, NULL, &error);
    if (connection == NULL) {
        std::cout << "Error: Could not create PulseAudio connection" << std::endl;
        return 1;
    }

    // Generate a sine wave buffer
    const int buffer_size = 1024;
    float buffer[buffer_size];
    float frequency = 440.0;
    float phase = 0.0;
    while (true) {
        //int s = std::chrono::duration_cast<std::chrono::seconds>(system_clock::now()-start).count();
        int s = 0;

        for (int i = 0; i < buffer_size; i++) {
            std::cout << (880-40*s) << " Hz" << std::endl;
            buffer[i] = 0.5f * (sin(2PI*(880-40*s)*phase) + sin(2PI*400*phase));
            buffer[i] *= 0.1;
            phase += 1.0f / SAMPLE_RATE;
            if (phase > 1.0) {
                phase -= 1.0;
            }
        }

        // Output the sine wave buffer
        int num_written = pa_simple_write(connection, buffer, buffer_size * sizeof(float), &error);
        if (num_written < 0) {
            std::cout << "Error: Could not write to PulseAudio connection" << std::endl;
            return 1;
        }
    }

    // Clean up the PulseAudio connection
    pa_simple_free(connection);

    return 0;
}