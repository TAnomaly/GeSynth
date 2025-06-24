#include "Synth.hpp"
#include <cmath>

Synth::Synth() : waveType(WaveForm::Sine), frequency(440.0f), amplitude(0.5f),
                 baseFrequency(440.0f), baseCutoff(1000.0f), env(), seq(), filter(), lfo(), phase(0.0f) {}

void Synth::setFrequency(float freq)
{
    baseFrequency = freq;
    frequency = freq; // Will be modulated by LFO if enabled
}

float Synth::process(float dt)
{
    float envVal = env.process(dt);

    // LFO işleme
    float lfoValue = lfo.process(dt);

    // LFO modülasyonlarını uygula
    float modFrequency = baseFrequency;
    float modAmplitude = amplitude;
    float modCutoff = baseCutoff;

    if (lfo.target == LFOTarget::Pitch)
    {
        // Vibrato: ±50 cents (yaklaşık %3 frekans değişimi)
        modFrequency = baseFrequency * (1.0f + lfoValue * 0.03f);
    }
    else if (lfo.target == LFOTarget::Amplitude)
    {
        // Tremolo: amplitüd modülasyonu
        modAmplitude = amplitude * (1.0f + lfoValue * 0.5f);
        if (modAmplitude < 0)
            modAmplitude = 0;
    }
    else if (lfo.target == LFOTarget::Filter)
    {
        // Filter sweep: cutoff modülasyonu
        modCutoff = baseCutoff * (1.0f + lfoValue * 0.8f);
        if (modCutoff < 100.0f)
            modCutoff = 100.0f;
        if (modCutoff > 8000.0f)
            modCutoff = 8000.0f;
        filter.setCutoff(modCutoff);
    }

    // Faz artımını hesapla (modüle edilmiş frekansla)
    phase += modFrequency * dt * 2.0f * M_PI;

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

    // Envelope ve modüle edilmiş amplitüdü uygula, sonra filter'dan geçir
    float processedSample = sample * modAmplitude * envVal;
    return filter.process(processedSample);
}
