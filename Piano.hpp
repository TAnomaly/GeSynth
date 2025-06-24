#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Piano {
public:
    static constexpr int NUM_KEYS = 14;
    Piano();
    void draw(SDL_Renderer* renderer, int x, int y, int width, int height, int activeKey) const;
    int getKeyAtPosition(int px, int py, int pianoX, int pianoY, int pianoWidth, int pianoHeight) const;
    const std::string& getNoteName(int key) const;
private:
    std::vector<std::string> noteNames;
};
