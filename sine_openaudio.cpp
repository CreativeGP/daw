#include <cmath>
#include <iostream>
#include <openaudio.h>

int main() {
    // Create an audio output device with default settings
    openaudio::AudioDevice device;

    // Set the sample rate to 44100 Hz and the buffer size to 512 samples
    device.setSampleRate(44100);
    device.setBufferSize(512);

    // Start the audio device
    device.start();

    // Generate a sine wave
    const double frequency = 440.0;  // Hz
    const double amplitude = 0.5;
    const double phase = 0.0;
    const double twoPi = 2.0 * M_PI;
    double t = 0.0;
    while (device.isRunning()) {
        double sample = amplitude * std::sin(twoPi * frequency * t + phase);
        device.write(sample);
        t += 1.0 / device.getSampleRate();
    }

    // Stop the audio device
    device.stop();

    return 0;
}