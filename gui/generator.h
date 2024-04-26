#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "util.h"
#include "component.h"

// #include "tsf.h"
struct tsf;

#include <SDL2/SDL.h>
#include <functional>
#include <set>
#include <iostream>
#include <unordered_map>
#include <mutex>

struct GlobalCargo;

class Generator {
public:
    Generator(unsigned short);
    ~Generator();
    
    // virtual Message update(GlobalCargo *globals);
    // virtual void draw(/*SDL_Surface *surface*/ SDL_Renderer *);

    void note_on();
    void note_on(float length);
    void note_off();
    void write_buffer(GlobalCargo *globals, Sint16 *samples, int length);
    std::function<Sint16(float)> wave = Generator::sine_wave;

    float phase = 0;
    float state = 0;
    unsigned short note;

    static Sint16 sine_wave (float t);
};


class Instrument {
public:
    // virtual ~Instrument() {}

    // virtual void note_on(int note, float length) = 0;
  virtual void note_on(int note) = 0;
    virtual void note_off(int note) = 0;
    virtual /*AUDIO_THREAD*/ void write_buffer(GlobalCargo *globals, Sint16 *samples, int length) = 0;

private:
};


class SampleInstrument : public Instrument {
public:
    SampleInstrument();
    ~SampleInstrument();

    virtual void note_on(int note);
    void note_on(int note, float length);
    virtual void note_off(int note);
    virtual /*AUDIO_THREAD*/ void write_buffer(GlobalCargo *globals, Sint16 *samples, int length);
    
private:
    Generator *gens[128];
};

// class SF2Instrument : public Instrument {
// public:
//     SF2Instrument(std::string filename, int program, GlobalCargo *globals);
//     ~SF2Instrument();
    
//     virtual void note_on(int note);
//     void note_on(int note, float length);
//     virtual void note_off(int note);
//     virtual /*AUDIO_THREAD*/ void write_buffer(GlobalCargo *globals, Sint16 *samples, int length);
//     void change_program(int channel, int program);

//     unsigned short velocity = 30;
//     tsf* tinysoundfont;
// private:
//     std::mutex mtx;
// };

#endif
