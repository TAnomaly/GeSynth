#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "WaveForm.hpp"

class Slider
{
public:
    int x, y, w, h;
    int value, min, max;
    bool dragging;
    std::string label;
    Slider(int x, int y, int w, int h, int min, int max, int value, const std::string &label);
    void draw(SDL_Renderer *renderer) const;
    bool handleEvent(const SDL_Event &event);
};

class WaveSelector
{
public:
    int x, y, w, h;
    WaveForm::Type currentWave;
    std::string waveNames[4];

    WaveSelector(int x_, int y_, int w_, int h_);
    void draw(SDL_Renderer *renderer) const;
    bool handleEvent(const SDL_Event &event);
    void nextWave();
};
