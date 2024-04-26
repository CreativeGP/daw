#include "generator.h"

#define TSF_IMPLEMENTATION
#include "tsf.h"

Generator::Generator(unsigned short note) : note(note) {}
Generator::~Generator() {}

void Generator::write_buffer(GlobalCargo *globals, Sint16 *samples, int length) {
    if (state == 0) return;
    if (state > 0) {
        state -= (float)length/globals->Obtained.freq;
        if (state < 0) state = -2;
    }

    float samples_per_phase = globals->Obtained.freq/(std::pow(2, (float)(note-69)/12)*440);
    for (int i = 0; i < length; i++) {
        samples[i] += wave(phase);
        phase += 1/samples_per_phase;
        if (phase > 1) {
            phase = 0;
            if (state == -2) {
                state = 0;
                break;
            }
        }
    }

    // std::cout << length << std::endl;
}

void Generator::note_on() {
    state = -1;
}

void Generator::note_on(float sec) {
    // phase = 0;
    state = sec;
}

void Generator::note_off() {
    if (state == -1 || state > 0) state = -2;
    else state = 0;
}


Sint16 Generator::sine_wave (float t) {
    // return 3000*t;
    // if (t > 0.5) return 3000;
    // else return 0;
    return sin(2*3.14*t) * 3000;
};


SampleInstrument::SampleInstrument() {
    for (int i = 0; i < 128; i++) {
        gens[i] = new Generator(i);
    }
}
SampleInstrument::~SampleInstrument() {
    for (int i = 0; i < 128; i++) {
        delete gens[i];
    }
}

AUDIO_THREAD void SampleInstrument::write_buffer(GlobalCargo *globals, Sint16 *samples, int length) {
    for (int i = 0; i < 128; i++) {
        if (gens[i]->state != 0)
            gens[i]->write_buffer(globals, samples, length);
    }
}

void SampleInstrument::note_on(int note, float sec) {
    if (note < 0 || note > 127) return;
    gens[note]->note_on(sec);
}
void SampleInstrument::note_on(int note) {
    if (note < 0 || note > 127) return;
    gens[note]->note_on();
}
void SampleInstrument::note_off(int note) {
    if (note < 0 || note > 127) return;
    gens[note]->note_off();
}



SF2Instrument::SF2Instrument(std::string filename, int program, GlobalCargo *globals) {
    tinysoundfont = tsf_load_filename(filename.c_str());
    if (!tinysoundfont) {
        std::cerr << "Could not load SoundFont" << std::endl;
    }
	tsf_channel_set_bank_preset(this->tinysoundfont, 9, 128, 0);
	tsf_set_output(this->tinysoundfont, TSF_MONO, globals->Obtained.freq, 0.0f);

    // multi channelあるんだけど...
   tsf_channel_set_presetnumber(this->tinysoundfont, 2, program, false); 
}
SF2Instrument::~SF2Instrument() {
    // tsf_close(this->tinysoundfont); this->tinysoundfont = 0;
}

AUDIO_THREAD void SF2Instrument::write_buffer(GlobalCargo *globals, Sint16 *samples, int length) {
    tsf_render_short(this->tinysoundfont, samples, length, 0);

}

void SF2Instrument::change_program(int channel, int program) {
   tsf_channel_set_presetnumber(this->tinysoundfont, channel, program, (channel == 9)); 
}

void SF2Instrument::note_on(int note) {
    std::lock_guard<std::mutex> lock (mtx);
    std::cout << "note on " << note << std::endl;
    tsf_channel_note_on(this->tinysoundfont, 2, note, 0.01f);
}
void SF2Instrument::note_off(int note) {
    std::lock_guard<std::mutex> lock (mtx);
    std::cout << "note off " << note << std::endl;
    tsf_channel_note_off(this->tinysoundfont, 2, note);
}

