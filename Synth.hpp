#pragma once
#include "WaveForm.hpp"
#include "Envelope.hpp"
#include "Sequencer.hpp"
#include "Filter.hpp"
#include "LFO.hpp"

class Synth
{
public:
    WaveForm::Type waveType;
    float frequency, amplitude;
    float phase;         // Faz durumu için eklendi
    float baseFrequency; // LFO modülasyonu için orijinal frekans
    float baseCutoff;    // LFO modülasyonu için orijinal cutoff
    Envelope env;
    Sequencer seq;
    Filter filter;
    LFO lfo;

    Synth();
    float process(float dt);
    void setFrequency(float freq);
};
