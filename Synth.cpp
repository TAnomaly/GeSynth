#include "Synth.hpp"
#include <cmath>

Synth::Synth() : waveType(WaveForm::Sine), frequency(440.0f), amplitude(0.5f), env(), seq(), filter(), phase(0.0f) {}

float Synth::process(float dt)
{
    float envVal = env.process(dt);

    // Faz artımını hesapla
    phase += frequency * dt * 2.0f * M_PI;

    // Fazı 2π ile sınırla
    while (phase >= 2.0f * M_PI)
    {
        phase -= 2.0f * M_PI;
    }

    float sample = 0.0f;
    switch (waveType)
    {
    case WaveForm::Sine:
        sample = std::sin(phase);
        break;
    case WaveForm::Square:
        sample = (std::sin(phase) > 0) ? 1.0f : -1.0f;
        break;
    case WaveForm::Triangle:
    {
        float t = phase / (2.0f * M_PI);
        sample = 2.0f * std::abs(2.0f * (t - std::floor(t + 0.5f))) - 1.0f;
        break;
    }
    case WaveForm::Saw:
    {
        float t = phase / (2.0f * M_PI);
        sample = 2.0f * (t - std::floor(t + 0.5f));
        break;
    }
    }

    // Önce envelope uygula, sonra filter'dan geçir
    float processedSample = sample * amplitude * envVal;
    return filter.process(processedSample);
}
