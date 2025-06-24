#include "LFO.hpp"
#include <cmath>

LFO::LFO(float rate, float depth)
    : rate(rate), depth(depth), phase(0.0f),
      waveform(WaveForm::Sine), target(LFOTarget::None), enabled(false) {}

float LFO::process(float dt)
{
    if (!enabled)
    {
        return 0.0f;
    }

    // Update phase
    phase += rate * dt * 2.0f * M_PI;

    // Keep phase in range [0, 2π]
    while (phase >= 2.0f * M_PI)
    {
        phase -= 2.0f * M_PI;
    }

    // Generate LFO waveform
    float lfoValue = 0.0f;
    switch (waveform)
    {
    case WaveForm::Sine:
        lfoValue = std::sin(phase);
        break;
    case WaveForm::Square:
        lfoValue = (std::sin(phase) > 0) ? 1.0f : -1.0f;
        break;
    case WaveForm::Triangle:
    {
        float t = phase / (2.0f * M_PI);
        lfoValue = 2.0f * std::abs(2.0f * (t - std::floor(t + 0.5f))) - 1.0f;
        break;
    }
    case WaveForm::Saw:
    {
        float t = phase / (2.0f * M_PI);
        lfoValue = 2.0f * (t - std::floor(t + 0.5f));
        break;
    }
    }

    // Apply depth scaling
    return lfoValue * depth;
}

void LFO::reset()
{
    phase = 0.0f;
}