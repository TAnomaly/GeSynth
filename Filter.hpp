#pragma once

class Filter
{
public:
    float cutoff;
    float resonance;
    float prevSample;

    Filter(float cutoff = 1000.0f, float resonance = 0.1f);
    float process(float input);
    void setCutoff(float freq);
};