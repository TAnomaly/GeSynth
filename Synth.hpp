#pragma once
#include "WaveForm.hpp"
#include "Envelope.hpp"
#include "Sequencer.hpp"
#include "Filter.hpp"

class Synth
{
public:
    WaveForm::Type waveType;
    float frequency, amplitude;
    float phase; // Faz durumu için eklendi
    Envelope env;
    Sequencer seq;
    Filter filter;

    Synth();
    float process(float dt);
};
