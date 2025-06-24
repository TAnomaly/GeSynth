#pragma once
#include <SDL2/SDL.h>
#include <cmath>

class WaveForm {
public:
    enum Type { Sine, Square, Triangle, Saw };
    static float generate(Type type, float phase);
    static void draw(SDL_Renderer* renderer, Type type, float freq, float phase, int x, int y, int w, int h);
};
