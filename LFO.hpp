#pragma once
#include "WaveForm.hpp"

enum class LFOTarget
{
    None = 0,
    Pitch = 1,     // Vibrato
    Amplitude = 2, // Tremolo
    Filter = 3     // Filter sweep
};

class LFO
{
public:
    float rate;  // LFO frequency (Hz)
    float depth; // Modulation depth (0-1)
    float phase; // Current phase
    WaveForm::Type waveform;
    LFOTarget target;
    bool enabled;

    LFO(float rate = 4.0f, float depth = 0.3f);
    float process(float dt);
    void reset();
};