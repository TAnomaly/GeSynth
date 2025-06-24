#include "WaveForm.hpp"

float WaveForm::generate(Type type, float phase) {
    switch (type) {
        case Sine:     return std::sin(phase);
        case Square:   return (std::sin(phase) > 0) ? 1.0f : -1.0f;
        case Triangle: return 2.0f * std::abs(2.0f * (phase / (2 * M_PI) - std::floor(phase / (2 * M_PI) + 0.5f))) - 1.0f;
        case Saw:      return 2.0f * (phase / (2 * M_PI) - std::floor(phase / (2 * M_PI) + 0.5f));
    }
    return 0.0f;
}

void WaveForm::draw(SDL_Renderer* renderer, Type type, float freq, float phase, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    float localPhase = phase;
    float samplesPerCycle = 44100.0f / freq;
    int points = w;
    float step = samplesPerCycle / points;
    int prevY = y + h / 2;
    for (int i = 0; i < points; ++i) {
        float value = generate(type, localPhase);
        int drawY = y + static_cast<int>((1.0f - value) * h / 2);
        if (i > 0) {
            SDL_RenderDrawLine(renderer, x + i - 1, prevY, x + i, drawY);
        }
        prevY = drawY;
        localPhase += 2 * M_PI * step / samplesPerCycle;
        if (localPhase >= 2 * M_PI) localPhase -= 2 * M_PI;
    }
}
