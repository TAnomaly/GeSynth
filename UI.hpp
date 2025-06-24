#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "WaveForm.hpp"
#include "LFO.hpp"

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

private:
    void drawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius) const;
    void drawRoundedRectBorder(SDL_Renderer *renderer, SDL_Rect rect, int radius) const;
    void drawGradientRect(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color start, SDL_Color end, bool vertical) const;
    void drawValueDisplay(SDL_Renderer *renderer, int x, int y, int val, int minVal, int maxVal) const;
    void drawDigit(SDL_Renderer *renderer, int x, int y, int digit) const;
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

class LFOTargetSelector
{
public:
    int x, y, w, h;
    LFOTarget currentTarget;
    std::string targetNames[4];

    LFOTargetSelector(int x_, int y_, int w_, int h_);
    void draw(SDL_Renderer *renderer) const;
    bool handleEvent(const SDL_Event &event);
    void nextTarget();

private:
    void drawTargetSymbol(SDL_Renderer *renderer, int x, int y, int type, bool active) const;
};
