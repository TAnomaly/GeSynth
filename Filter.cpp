#include "Filter.hpp"
#include <cmath>

Filter::Filter(float cutoff, float resonance)
    : cutoff(cutoff), resonance(resonance), prevSample(0.0f) {}

float Filter::process(float input)
{
    // Basit bir low-pass filter
    float alpha = cutoff / (cutoff + 1.0f);
    float output = alpha * input + (1.0f - alpha) * prevSample;
    prevSample = output;
    return output;
}

void Filter::setCutoff(float freq)
{
    cutoff = freq;
}